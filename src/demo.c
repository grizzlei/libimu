#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include <locale.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#include "libimu/imu.h"

#define BUFLEN	128

////////////////////////////////////////////
/// GLOBAL DECLARATIONS (var)


imu_t imu;
pthread_t thr_serial;
pthread_mutex_t mtx_imu;

int terminate = 0;
// file descriptor for serial device
int fd_serial;
// serial device path
const char *fname_serial = "/dev/ttyUSB0";
// serial baud rate
const int32_t serial_baud = B115200;
// configurations to set and revert when we are done with serial port
struct termios tiosold, tiosnew;


////////////////////////////////////////////
/// GLOBAL DECLARATIONS (fun)


void draw_axes(GLfloat xlen, GLfloat ylen, GLfloat zlen);
void draw_imu();
void rotate_camera();
void runner_serial(void * arg);


////////////////////////////////////////////
/// EXTERN INITIALIZATIONS


int main(int argc, char *argv[])
{
	////////////////////////////////////////////

	if (glfwInit() != GLFW_TRUE)
	{
		return -1;
	}

	glutInit(&argc, argv);

	imu = imu_init();
	imu_set_calibration_mode(&imu, IMU_CALIBMODE_ONCE);
	// mpu6050 is initialized with ±1000 °/s gyro, ±4g accelerometer configurations
	imu_set_gyro_scale_factor(&imu, 2.f/131.f);
	imu_set_accelerometer_scale_factor(&imu, 2.f/16384.f);


	pthread_create(&thr_serial, NULL, &runner_serial, NULL);
	pthread_detach(thr_serial);
	pthread_mutex_init(&mtx_imu, NULL);

	GLFWwindow *window = glfwCreateWindow(640, 640, "libimu", NULL, NULL);
	glfwMakeContextCurrent(window);

	////////////////////////////////////////////

	glClearColor(0.14f, 0.14f, 0.14f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_RESCALE_NORMAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMaterial(GL_FRONT, GL_AMBIENT);
    glEnable(GL_COLOR_MATERIAL);

	GLfloat lightka[] = {.2f, .2f, .2f, 1.f};  // ambient light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightka);

	// position of the light
	float lightpos[4] = {0, -10, 0, 1};
	glEnable(GL_LIGHT0);

	while (!glfwWindowShouldClose(window))
	{
		////////////////////////////////////////////

     	setlocale(LC_ALL, "");
		static GLfloat rot_y = 0.0f;
		const GLfloat rotation_speed = 0.2f;

		GLint w, h;
		glfwGetWindowSize(window, &w, &h);
		if (h == 0)
			w = 1;
		GLfloat aspect = (GLfloat)w / (GLfloat)h;

		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0f, aspect, 0.1f, 100.0f);
	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();

		////////////////////////////////////////////

	    glEnable(GL_DEPTH_TEST);

		gluLookAt(1, 1, 2, 0, 0, 0, 0, 1, 0);

		glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
		draw_axes(5.0, 5.0, 5.0);

		glPushMatrix();
			pthread_mutex_lock(&mtx_imu);
			imu_euler_t eul = imu.orientation;
			imu_quaternion_t orn = imu.orientation_quat;
			pthread_mutex_unlock(&mtx_imu);

			// Tate-Bryant rotation sequence
			glRotatef(r2d(eul.yaw), 0, 1, 0);
			glRotatef(r2d(eul.pitch), 0, 0, 1);
			glRotatef(r2d(eul.roll), 1, 0, 0);
			
			draw_axes(1.0, 1.0, 1.0);
			glScalef(0.2, 0.04, 0.2);
				glEnable(GL_LIGHTING);
						draw_imu();
				glDisable(GL_LIGHTING);
		glPopMatrix();

		////////////////////////////////////////////
		// 2d overlay
		////////////////////////////////////////////
		
		// switching to 2d view

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.f, w, h, 0.f, -1.f, 10.f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glDisable(GL_CULL_FACE);
		glClear(GL_DEPTH_BUFFER_BIT);

		// left, top
		const float framel = 10.f, framet = 10.f;
		// width, height
		const float framew = 200.f, frameh = 200.f;
		// padding
		const float framep = 10.f;

		glBegin(GL_QUADS);
			glColor3f(0.f, 0.f, 0.f);
			glVertex2f(framel, framet);
			glVertex2f(framel + framew, framet);
			glVertex2f(framel + framew, framet + frameh);
			glVertex2f(framel, framet + frameh);
		glEnd();

		char rpybuf[128] = {0};
		char deg = '°';
		snprintf(rpybuf, 127, "roll:   %f %c\npitch:  %f %c\nyaw:    %f %c", r2d(eul.roll), deg, r2d(eul.pitch), deg, r2d(eul.yaw), deg);

		glPushAttrib(GL_COLOR_BUFFER_BIT);
			const int fonth  = 13;

			glColor3f(0.f, 1.f, 0.f);
			glRasterPos2f(framel + framep, framet + fonth * 2);
			glutBitmapString(GLUT_BITMAP_8_BY_13, "libimu v0.1\nhasan karaman\ngithub.com/grizzlei");

			glColor3f(1.f, 1.f, 1.f);
			glRasterPos2f(framel + framep, framet + fonth * 6);
			glutBitmapString(GLUT_BITMAP_8_BY_13, rpybuf);

			glColor3f(1.f, 0.f, 0.f);
			glRasterPos2f(framel + framep, framet + fonth * 9 + fonth);
			glutBitmapString(GLUT_BITMAP_8_BY_13, "x - axis");
			glColor3f(0.f, 0.f, 1.f);
			glRasterPos2f(framel + framep, framet + fonth * 10 + fonth);
			glutBitmapString(GLUT_BITMAP_8_BY_13, "y - axis");
			glColor3f(0.f, 1.f, 0.f);
			glRasterPos2f(framel + framep, framet + fonth * 11 + fonth);
			glutBitmapString(GLUT_BITMAP_8_BY_13, "z - axis");
		glPopAttrib();

		GLfloat stateindicatorl = framel + framep,
				stateindicatort = fonth * 13 + fonth;
		char statestr[32] = {0};
		switch (imu.state)
		{
			case IMU_STATE_UNCALIBRATED:
				glColor3f(1.f, 0.f, 0.f);
				strcpy(statestr, "state: uncalibrated");
				break;
			case IMU_STATE_CALIBRATING:
				glColor3f(1.f, 0.58f, 0.f);
				strcpy(statestr, "state: calibrating");
				break;
			case IMU_STATE_READY:
				glColor3f(0.f, 1.f, 0.f);
				strcpy(statestr, "state: ready");
				break;
		}

		glBegin(GL_QUADS);
			glVertex2f(stateindicatorl, stateindicatort);
			glVertex2f(stateindicatorl + 10, stateindicatort);
			glVertex2f(stateindicatorl + 10, stateindicatort + 10);
			glVertex2f(stateindicatorl, stateindicatort + 10);
		glEnd();
		glRasterPos2f(stateindicatorl + 15, stateindicatort + 10);
		glutBitmapString(GLUT_BITMAP_8_BY_13, statestr);


		// switching back to 3d view

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		////////////////////////////////////////////
		// end of 2d overlay
		////////////////////////////////////////////

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	terminate = 1;
	pthread_join(thr_serial, NULL);

	return (0);
}


////////////////////////////////////////////


void draw_axes(GLfloat xlen, GLfloat ylen, GLfloat zlen)
{
	glPushMatrix();
		// x-axis
		glBegin(GL_LINES);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(xlen, 0.0f, 0.0f);
		glEnd();
	
		glBegin(GL_TRIANGLES);
			glVertex3f(xlen, 0.f, 0.f);
			glVertex3f(xlen * 0.9, 0.05f, 0.f);
			glVertex3f(xlen * 0.9, -0.05f, 0.f);
		glEnd();

		// y-axis
		glBegin(GL_LINES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, ylen, 0.0f);
		glEnd();
	
		glBegin(GL_TRIANGLES);
			glVertex3f(0.f, ylen, 0.f);
			glVertex3f(0.f, ylen * 0.9, 0.05f);
			glVertex3f(0.f, ylen * 0.9, -0.05f);
		glEnd();

		// z-axis
		glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, zlen);
		glEnd();
	
		glBegin(GL_TRIANGLES);
			glVertex3f(0.f, 0.f, zlen);
			glVertex3f(0.f, 0.05f, zlen * 0.9);
			glVertex3f(0.f, -0.05f, zlen * 0.9);
		glEnd();

	glPopMatrix();
}


////////////////////////////////////////////


void draw_imu()
{
	glPushAttrib(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE | GL_FILL);
		glColor3f(0.199f, 0.199f, 0.199f);
		// glColor3f(1.0f, 0.6f, 0.19f);
		glBegin(GL_QUADS);
			// Top face
			glVertex3f(1.0f, 1.0f, -1.0f); // Top-right of top face
			glVertex3f(-1.0f, 1.0f, -1.0f); // Top-left of top face
			glVertex3f(-1.0f, 1.0f, 1.0f); // Bottom-left of top face
			glVertex3f(1.0f, 1.0f, 1.0f); // Bottom-right of top face
			glNormal3d(0, 1, 0);
			// Bottom face
			glVertex3f(1.0f, -1.0f, -1.0f); // Top-right of bottom face
			glVertex3f(-1.0f, -1.0f, -1.0f); // Top-left of bottom face
			glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom-left of bottom face
			glVertex3f(1.0f, -1.0f, 1.0f); // Bottom-right of bottom face
			glNormal3d(0, -1, 0);
			// Front face
			glVertex3f(1.0f, 1.0f, 1.0f); // Top-Right of front face
			glVertex3f(-1.0f, 1.0f, 1.0f); // Top-left of front face
			glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom-left of front face
			glVertex3f(1.0f, -1.0f, 1.0f); // Bottom-right of front face
			glNormal3d(0, 0, 1);
			// Back face
			glVertex3f(1.0f, -1.0f, -1.0f); // Bottom-Left of back face
			glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom-Right of back face
			glVertex3f(-1.0f, 1.0f, -1.0f); // Top-Right of back face
			glVertex3f(1.0f, 1.0f, -1.0f); // Top-Left of back face
			glNormal3d(0, 0, -1);
			// Left face
			glVertex3f(-1.0f, 1.0f, 1.0f); // Top-Right of left face
			glVertex3f(-1.0f, 1.0f, -1.0f); // Top-Left of left face
			glVertex3f(-1.0f, -1.0f, -1.0f); // Bottom-Left of left face
			glVertex3f(-1.0f, -1.0f, 1.0f); // Bottom-Right of left face
			glNormal3d(-1, 0, 0);
			// Right face
			glVertex3f(1.0f, 1.0f, 1.0f); // Top-Right of left face
			glVertex3f(1.0f, 1.0f, -1.0f); // Top-Left of left face
			glVertex3f(1.0f, -1.0f, -1.0f); // Bottom-Left of left face
			glVertex3f(1.0f, -1.0f, 1.0f); // Bottom-Right of left face
			glNormal3d(1, 0, 0);
		glEnd();
	glPopAttrib();
}


////////////////////////////////////////////


void runner_serial(void * arg)
{
	prwar("thread started");

	serial_port_init();

	while(!terminate) // && (terminate == 0)
	{
		char buffer[BUFLEN] = {0x00};
		char eol = 0x0A;
		int index = 0;

		unsigned char c = 0x00;
		for(int received = 0; !terminate && (index < BUFLEN); received = read(fd_serial, &c, 1))
		{
			if(received == 1)
			{
				buffer[index++] = c;
				if(c == eol)
					break;
			}
			else if(received == -1)
				goto break_out;
			c = received = 0x00;
		}

		int ax, ay, az, gx, gy, gz;
		double ts = 0.0;
		if(buffer[0] != 0x0A)
		{
			sscanf(buffer, "%d,%d,%d,%d,%d,%d,%lf", &ax, &ay, &az, &gx, &gy, &gz, &ts);
			pthread_mutex_lock(&mtx_imu);
				imu_set_accelerometer_raw(&imu, ax, ay, az);
				imu_set_gyro_raw(&imu, gx, gy, gz);
				imu_main_loop(&imu);
			pthread_mutex_unlock(&mtx_imu);
		}

		usleep(1000);
	}

	if(0) // we don't want this error message unless we jump here.
	{
		break_out:
		prerr("error while reading from serial port");
	}

	serial_port_cleanup();
	prwar("thread finished");
}


////////////////////////////////////////////


void serial_port_init()
{
	if ((fd_serial = open(fname_serial, O_RDWR | O_NOCTTY)) < 0)
	{
		prerr("cannot open %s. (%s)", fname_serial, strerror(errno));
		exit(-1);
	}

	tcgetattr(fd_serial, &tiosold);
	bzero(&tiosnew, sizeof(tiosnew));

	tiosnew.c_cflag |= serial_baud | CRTSCTS | CS8 | CLOCAL | CREAD;
	tiosnew.c_iflag |= IGNPAR | ICRNL;
	tiosnew.c_oflag = 0;
	tiosnew.c_cc[VTIME] = 0;
	tiosnew.c_cc[VMIN] = 0;
	
	cfsetispeed(&tiosnew, B115200);
	cfsetospeed(&tiosnew, B115200);

	tcflush(fd_serial, TCIOFLUSH);
	tcsetattr(fd_serial, TCSANOW, &tiosnew);
}


////////////////////////////////////////////


void serial_port_cleanup()
{
	tcsetattr(fd_serial, TCSANOW, &tiosold);
	close(fd_serial);
}


////////////////////////////////////////////