#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#define _USE_MATH_DEFINES
#include <cstdlib> 
#include <glut.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <string>
#include <Windows.h>
#include <mmsystem.h>
#include <thread>
#include <vector>


#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
//game variables
bool gameRunning = true;
float timeRemaining = 600.0f;
int score = 0;
std::string gameMessage = ""; //string that appears at end of game
//player movement variables
bool directionX = false; // 0: true: right, false: left PLAYERs
bool directionZ = true; // 0: true: forward, false: backwards
float playerX = 0.0f; //initially centre bottom
float playerZ = 2.5f;
float error = 0.0; //error from cursor: red greatest error, green smallest error
bool ballMoving = false;
int cylinder = 4;
bool cyl1 = true;
bool cyl2 = true;
bool cyl3 = true;
//can shoot only when locked, locked when standing in shooting point
bool locked = true;
//glitch variables
double target = 1.35601e-07;
double epsilon = 1e-05;
int glitchCounter = 0;
//animation variables
float isAnimating = false;
int wallColor = 0;
int animTimer = 0;
bool animating = false;
bool rotated = false;

//cursor effect
int currentSection = 0;
int cursorSpeedCounter = 0;
std::vector<std::vector<float>> colors = {
	{7.0f, 0.0f, 0.0f}, // Red
	{7.0f, 3.5f, 0.0f}, // Orange
	{7.0f, 7.0f, 0.0f}, // Yellow
	{0.0f, 7.0f, 0.0f}, // Green
	{7.0f, 7.0f, 0.0f}, // Yellow
	{7.0f, 3.5f, 0.0f}, // Orange
	{7.0f, 0.0f, 0.0f}, // Red
};

//Threads
void playBackground() {
	PlaySound(TEXT("Sounds/background.wav"), NULL, SND_ASYNC | SND_FILENAME | SND_LOOP);
}

void playCrash() {
	PlaySound(TEXT("Sounds/crash.wav"), NULL, SND_ASYNC | SND_FILENAME);
}

void playPoint() {
	PlaySound(TEXT("Sounds/point.wav"), NULL, SND_ASYNC | SND_FILENAME);
}

void playWin() {
	PlaySound(TEXT("Sounds/win.wav"), NULL, SND_ASYNC | SND_FILENAME);
}

void playLose() {
	PlaySound(TEXT("Sounds/lose.wav"), NULL, SND_ASYNC | SND_FILENAME);
}




class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}

	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		gluLookAt(
			eye.x, eye.y, eye.z,
			center.x, center.y, center.z,
			up.x, up.y, up.z
		);
	}

	void setView(char viewType) {
		switch (viewType) {
		case 'F': // Front View
			eye = Vector3f(0.0f, 1.0f, 3.0f);  // Camera positioned along the Z-axis
			center = Vector3f(0.0f, 0.0f, 0.0f); // Looking towards the origin
			up = Vector3f(0.0f, 1.0f, 0.0f); // Y-axis up
			break;
		case 'T': // Top View
			eye = Vector3f(0.0f, 2.0f, 0.0f);  // Camera positioned above the scene
			center = Vector3f(0.0f, 0.0f, 0.0f); // Looking towards the origin
			up = Vector3f(0.0f, 0.0f, -1.0f); // Z-axis up (camera looking down)
			break;
		case 'R': // Side View
			eye = Vector3f(2.0f, 1.0f, 0.0f);  // Camera positioned on the X-axis
			center = Vector3f(0.0f, 0.0f, 0.0f); // Looking towards the origin
			up = Vector3f(0.0f, 1.0f, 0.0f); // Y-axis up
			break;
		}
	}
};

Camera camera;

class Ball {
public:
	float x, y, z;
	Ball(float x, float y, float z) : x(x), y(y), z(z) {}
	void shoot() {

		if (glitchCounter >= 65) {
			glitchCounter = 0;
			ballMoving = false;  // Stop moving when the ball reaches the border
			switch (cylinder) {
			case 4: x = 0.0; z = 2.3; break;
			case 3: x = 0.0; z = -2.3; break;
			case 2: x = 2.3; z = 0.0; break;
			case 1: x = -2.3; z = 0.0; break;
			default: x = 0.0; z = 0.0; break;
			}
			std::thread t9(playLose);
			t9.detach();
			return;
		}

		glitchCounter++;
		//std::cout << glitchCounter << std::endl;

		float directionX = -x;
		float directionZ = -z;

		switch (cylinder) {
			case 4: directionX += error; break;
			case 2: directionZ += error; break;
			case 3: directionX += error; break;
			case 1: directionZ += error; break;
			default: break;
		}

		// Normalize the direction vector
		/*float length = sqrt(directionX * directionX + directionZ * directionZ);
		if (length > 1e-5) { // Avoid division by a near-zero length
			directionX /= length;
			directionZ /= length;
		}*/

			//condition for scoring a point
		if ((z >= -0.1 && z <= 0.1) && (x >= -0.1 && x <= 0.1)) {
			std::thread t3(playWin);
			t3.detach();
			ballMoving = false;
			score++;
			locked = false;

			switch (cylinder) {
			case 4:
				cylinder = 1;
				z = 0.0f;
				x = -2.3f;
				cyl1 = false;
				cyl2 = true;
				cyl3 = true;
				break;
			case 1:
				cylinder = 2;
				z = 0.0f;
				x = 2.3f;
				cyl1 = true;
				cyl2 = false;
				cyl3 = true;
				break;
			case 2:
				cylinder = 3;
				z = -2.3f;
				x = 0.0f;
				cyl1 = true;
				cyl2 = true;
				cyl3 = false;
				break;
			default:
				std::cout << "Win" << std::endl;
			}
		}
		else {
			//continue moving on your way
			x += directionX * 0.1f;
			z += directionZ * 0.1f;
		}

		/*if (std::abs(x) >= 3.0f || std::abs(z) >= 3.0f) {
			ballMoving = false;  // Stop moving when the ball reaches the border
			switch (cylinder) {
				case 4: x = 0.0; z = 2.3; break;
				case 3: x = 0.0; z = -2.3; break;
				case 2: x = 2.3; z = 0.0; break;
				case 1: x = -2.3; z = 0.0; break;
				default: x = 0.0; z = 0.0; break;
			}
			std::cout << "break" << std::endl;
		}*/
		
		draw(x, y, z);

	}
	void draw(float x, float y, float z) {
		glPushMatrix();
		glTranslatef(x, y + 0.05, z);
		glScalef(0.1, 0.1, 0.1);
		// Draw the main golf ball as a sphere
		if (animating) {
			if (wallColor % 2 == 0) {
				glColor3f(7.0f, 7.0f, 0.0f);
			}
			else {
				glColor3f(7.0f, 7.0f, 7.0f);
			}
		}
		else {
			glColor3f(1.0f, 1.0f, 1.0f);
		}
		glutSolidSphere(0.5, 20, 20); // Main ball with radius 0.5, 20 slices, 20 stacks

		// Draw dimples on the golf ball (small indentations)
		glColor3f(0.8f, 0.8f, 0.8f); // Light gray color for dimples
		for (float lat = -40.0f; lat <= 40.0f; lat += 20.0f) { // Latitude of dimples
			for (float lon = -40.0f; lon <= 40.0f; lon += 20.0f) { // Longitude of dimples
				// Calculate position for each dimple
				float x = 0.5f * cosf(lat * M_PI / 180.0f) * cosf(lon * M_PI / 180.0f);
				float y = 0.5f * cosf(lat * M_PI / 180.0f) * sinf(lon * M_PI / 180.0f);
				float z = 0.5f * sinf(lat * M_PI / 180.0f);

				glPushMatrix();
				glTranslatef(x, y, z); // Position each dimple on the surface
				glutSolidSphere(0.05, 8, 8); // Small dimples with radius 0.05
				glPopMatrix();
			}
		}
		glPopMatrix();
	};
};

Ball ball(0.0,0.0,2.4);

void drawPond(float x, float y, float z, float holeRadius = 0.3f) {
	glPushMatrix();
	glTranslatef(x, y, z);
	if (animating) {
		if (wallColor % 5 == 0) {
			glScalef(1.1f, 1.0f,1.1f);
		}
		else if (wallColor % 5 == 1) {
			glScalef(1.2f, 1.0f, 1.2f);
		}
		else if (wallColor % 5 == 2) {
			glScalef(1.3f,1.0f,1.3f);
		}
		else if (wallColor % 5 == 3) {
			glScalef(1.2f,1.0f,1.2f);
		}
		else {
			glScalef(1.1f, 1.0f, 1.1f);
		}
	}

	glPushMatrix();
	glColor3f(0.0f, 0.0f, 1.0f);
	glTranslatef(0.05f, 0.02f, 0.0f); // Slightly above ground level to avoid z-fighting
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, 0.0f); // Center of the circle
	for (int i = 0; i <= 50; i++) {
		float angle = i * 2.0f * M_PI / 50; // Calculate angle for each segment
		float dx = holeRadius * cos(angle);
		float dz = holeRadius * sin(angle);
		glVertex3f(dx, 0.0f, dz); // Outer circle vertex
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f); // Dark grey color for rock
	glTranslatef(-0.2f, 0.02f, 0.2f); // Position the rock near the pond
	glutSolidSphere(0.1, 10, 10); // Rock as a small sphere
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.2f, 0.2f, 0.2f); // Dark grey color for rock
	glTranslatef(-0.1f, 0.02f, 0.3f); // Position the rock near the pond
	glutSolidSphere(0.1, 10, 10); // Rock as a small sphere
	glPopMatrix();

	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
}

void drawCrocodile(float x, float y, float z) {
	glPushMatrix();

	glTranslatef(x, y + 0.2f, z);
	glScalef(0.3f, 0.3f, 0.3f);

	// Crocodile body (elongated scaled cylinder)
	glColor3f(0.0f, 0.5f, 0.0f); // Dark green color
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(2.0f, 0.3f, 0.6f);   // Scale to make an elongated body
	glutSolidSphere(0.5, 20, 20); // Body base
	glPopMatrix();

	// Crocodile head (smaller elongated sphere)
	glPushMatrix();
	glTranslatef(1.1f, 0.15f, 0.0f);
	glScalef(1.2f, 0.3f, 0.5f);   // Narrowed and elongated head
	glColor3f(0.0f, 0.5f, 0.0f);
	glutSolidSphere(0.3, 20, 20); // Head
	glPopMatrix();

	// Eyes (small white spheres positioned on the head)
	glPushMatrix();
	glTranslatef(1.2f, 0.3f, 0.15f); // Position for right eye
	glColor3f(1.0f, 1.0f, 1.0f);     // White color
	glutSolidSphere(0.05, 10, 10);   // Right eye
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1.2f, 0.3f, -0.15f); // Position for left eye
	glutSolidSphere(0.05, 10, 10);    // Left eye
	glPopMatrix();

	// Legs (short cylinders)
	glColor3f(0.0f, 0.4f, 0.0f); // Darker green for the legs
	for (float lx = -0.7f; lx <= 0.7f; lx += 1.4f) {
		for (float lz = -0.3f; lz <= 0.3f; lz += 0.6f) {
			glPushMatrix();
			glTranslatef(lx, -0.25f, lz);    // Position of each leg
			glRotatef(-90, 1.0f, 0.0f, 0.0f); // Rotate to align vertically
			GLUquadric* leg = gluNewQuadric();
			gluCylinder(leg, 0.05, 0.05, 0.2, 10, 10); // Leg cylinder
			gluDeleteQuadric(leg);
			glPopMatrix();
		}
	}

	// Tail (thin tapered cylinder)
	glPushMatrix();
	glTranslatef(-1.0f, 0.05f, 0.0f);
	glRotatef(-20, 0.0f, 0.0f, 1.0f); // Tilt the tail to the side
	glRotatef(30, 1.0f, 0.0f, 0.0f);  // Lift slightly up
	GLUquadric* tail = gluNewQuadric();
	gluCylinder(tail, 0.1, 0.05, 1.0, 10, 10); // Tapered tail
	gluDeleteQuadric(tail);
	glPopMatrix();

	// Spikes on the back (small cones for a textured look)
	glColor3f(0.0f, 0.3f, 0.0f); // Even darker green for spikes
	for (float spikePos = -0.8f; spikePos <= 0.8f; spikePos += 0.3f) {
		glPushMatrix();
		glTranslatef(spikePos, 0.2f, 0.0f); // Position spikes along the back
		glRotatef(-90, 1.0f, 0.0f, 0.0f);  // Rotate to make spikes point up
		glutSolidCone(0.05, 0.2, 10, 2);   // Cone for each spike
		glPopMatrix();
	}

	glColor3f(0.0f, 0.0f, 0.0f);


	glPopMatrix(); // Restore transformations
}

void drawTiger(float x, float y, float z) {
	glPushMatrix();

	if (animating) {
		if (wallColor % 5 == 0) {
			glRotatef(5.0,0.0f, 1.0f, 0.0f);
		}
		else if (wallColor % 5 == 1) {
			glRotatef(6.0, 0.0f, 1.0f, 0.0f);
		}
		else if (wallColor % 5 == 2) {
			glRotatef(7.0, 0.0f, 1.0f, 0.0f);
		}
		else if (wallColor % 5 == 3) {
			glRotatef(6.0, 0.0f, 1.0f, 0.0f);
		}
		else {
			glRotatef(5.0, 0.0f, 1.0f, 0.0f);
		}
	}
	glTranslatef(x, y+0.1f, z);
	glRotatef(-135, 0.0f, 1.0f, 0.0f); // Rotate to face the camera
	//glRotatef(90, 0.0f, 0.0f, 1.0f);  // Rotate to stand vertically
	glScalef(0.3f, 0.3f, 0.3f);

	// Tiger body (scaled sphere)
	glColor3f(1.0f, 0.5f, 0.0f);  // Orange color
	glPushMatrix();
	glScalef(1.5f, 0.5f, 0.8f);   // Scale to represent the elongated body
	glutSolidSphere(0.5, 20, 20); // Body
	glPopMatrix();

	// Tiger head (smaller sphere)
	glPushMatrix();
	glTranslatef(0.7f, 0.3f, 0.0f);
	glScalef(0.6f, 0.6f, 0.6f);    // Smaller sphere for the head
	glColor3f(1.0f, 0.5f, 0.0f);
	glutSolidSphere(0.3, 20, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.85f, 0.25f, 0.1f); // Position for the right eye
	glColor3f(1.0f, 1.0f, 1.0f);      // White color for eyes
	glutSolidSphere(0.05, 10, 10);    // Right eye
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.85f, 0.25f, -0.1f); // Position for the left eye
	glutSolidSphere(0.05, 10, 10);     // Left eye
	glPopMatrix();

	// Legs (4 cylinders)
	glColor3f(1.0f, 0.5f, 0.0f);  // Same orange color
	for (float lx = -0.5f; lx <= 0.5f; lx += 1.0f) {
		for (float lz = -0.3f; lz <= 0.3f; lz += 0.6f) {
			glPushMatrix();
			glTranslatef(lx, -0.3f, lz);    // Position of each leg
			glRotatef(-90, 1.0f, 0.0f, 0.0f); // Rotate to align vertically
			GLUquadric* leg = gluNewQuadric();
			gluCylinder(leg, 0.05, 0.05, 0.4, 10, 10); // Leg cylinder
			gluDeleteQuadric(leg);
			glPopMatrix();
		}
	}

	// Tail (thin cylinder with a curve)
	glColor3f(1.0f, 0.5f, 0.0f);
	glPushMatrix();
	glTranslatef(-0.8f, 0.1f, 0.0f);
	glRotatef(30, 0.0f, 0.0f, 1.0f);
	glRotatef(-60, 1.0f, 0.0f, 0.0f);
	GLUquadric* tail = gluNewQuadric();
	gluCylinder(tail, 0.05, 0.02, 0.7, 10, 10); // Tapered tail
	gluDeleteQuadric(tail);
	glPopMatrix();

	// Optional: Stripes (thin black cylinders or lines on the body)
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for stripes
	for (float stripe = -0.4f; stripe <= 0.4f; stripe += 0.2f) {
		glPushMatrix();
		glTranslatef(stripe, 0.0f, 0.4f); // Side of the body
		glRotatef(45, 0.0f, 0.0f, 1.0f);
		glScalef(0.3f, 0.05f, 0.05f);
		glutSolidCube(1.0);  // Stripe cube
		glPopMatrix();
	}

	glColor3f(0.0f, 0.0f, 0.0f);

	glPopMatrix(); // Restore transformations
}


void drawFlag(float x, float y, float z, float poleHeight = 1.0f, float flagWidth = 0.2f, float flagHeight = 0.1f, float holeRadius = 0.08f) {
	// Push the current transformation matrix onto the stack
	glPushMatrix();

	// Move to the specified coordinates
	glTranslatef(x, y, z);

	glPushMatrix();
	glColor3f(0.1f, 0.1f, 0.1f);
	glTranslatef(0.05f, 0.02f, 0.0f); // Slightly above ground level to avoid z-fighting
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, 0.0f); // Center of the circle
	for (int i = 0; i <= 50; i++) {
		float angle = i * 2.0f * M_PI / 50; // Calculate angle for each segment
		float dx = holeRadius * cos(angle);
		float dz = holeRadius * sin(angle);
		glVertex3f(dx, 0.0f, dz); // Outer circle vertex
	}
	glEnd();
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);

	// Pole color and thickness
	glColor3f(0.8f, 0.8f, 0.8f); // Gray color for the pole
	float poleRadius = 0.01f;

	// Draw pole as a cylinder
	glPushMatrix();
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate the pole to stand vertically
	glTranslatef(0.0f, 0.0f, 0.0f); // Center the pole at half the height
	GLUquadric* quadric = gluNewQuadric();
	gluCylinder(quadric, poleRadius, poleRadius, poleHeight, 15, 15);
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);

	glColor3f(5.0f, 0.0f, 0.0f); // Red color for the flag
	// Draw flag
	glPushMatrix();
	glRotatef(-90.0f, 0.0f, 1.0f, 0.0f); // Rotate the flag to stand vertically
	glTranslatef(0.0f, poleHeight-0.1f, 0.0f); // Move to the top of the pole

	glPushMatrix();
	if (wallColor % 2 == 0) {
		glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);
	}
	glBegin(GL_QUADS);
	// Define the four vertices of the rectangular flag
	glVertex3f(0.0f, 0.0f, 0.0f);               // Bottom-left corner (attached to the pole)
	glVertex3f(0.0f, flagHeight, 0.0f);          // Top-left corner (attached to the pole)
	glVertex3f(0.0f, flagHeight, flagWidth);     // Top-right corner
	glVertex3f(0.0f, 0.0f, flagWidth);           // Bottom-right corner
	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);
	glPopMatrix();

	glPopMatrix();

	// Clean up
	gluDeleteQuadric(quadric);
	glPopMatrix();
}

void drawRectangleSection(int sectionIndex, float sectionWidth) {
	glColor3f(colors[sectionIndex][0], colors[sectionIndex][1], colors[sectionIndex][2]);
	glBegin(GL_QUADS);
	glVertex2f(sectionIndex * sectionWidth, 440);
	glVertex2f((sectionIndex + 1) * sectionWidth, 440);
	glVertex2f((sectionIndex + 1) * sectionWidth, 470);
	glVertex2f(sectionIndex * sectionWidth, 470);
	glEnd();
}

void drawCursor(float sectionWidth) {
	float cursorX = (currentSection + 0.5f) * sectionWidth;
	float cursorY = 480 + 10.0f; // Position above the sections

	// Draw the black outline triangle (slightly larger)
	glColor3f(0.0f, 0.0f, 0.0f); // Black color for outline
	glBegin(GL_TRIANGLES);
	glVertex2f(cursorX - 12, cursorY);
	glVertex2f(cursorX + 12, cursorY);
	glVertex2f(cursorX, cursorY - 22); // Slightly larger for outline
	glEnd();

	// Draw the inner white triangle (cursor)
	glColor3f(7.0f, 7.0f, 7.0f); // White color for cursor
	glBegin(GL_TRIANGLES);
	glVertex2f(cursorX - 10, cursorY);
	glVertex2f(cursorX + 10, cursorY);
	glVertex2f(cursorX, cursorY - 20); // Smaller, inside the outline
	glEnd();
}

void drawCursosrRect() {
	if (locked && !ballMoving) {
		float sectionWidth = 640 / 7.0f; // Width for each section

		for (int i = 0; i < 7; i++) {
			drawRectangleSection(i, sectionWidth);
		}

		drawCursor(sectionWidth);
	}

}

void drawWall1(double thickness) {

	if (wallColor % 2 == 0) {
		glColor3f(0.68f, 0.85f, 0.90f);
	}
	else {
		glColor3f(0.85f, 0.85f, 0.85f);
	}
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(0.5,  3.25, 0.0);
	glScaled(1.0, thickness, 6.5);
	glutSolidCube(1);
	glPopMatrix();

	if (wallColor % 2 == 0) {
		glColor3f(0.85f, 0.85f, 0.85f);
	}
	else {
		glColor3f(0.68f, 0.85f, 0.90f);
	}
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	glTranslated(1.5, 3.25, 0.0);
	glScaled(1.0, thickness, 6.5);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
}
void drawWall2(double thickness) {
	if (wallColor % 2 == 0) {
		glColor3f(0.68f, 0.85f, 0.90f);
	}
	else {
		glColor3f(0.85f, 0.85f, 0.85f);
	}	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	glTranslated(0.0, 3.0, 0.5);
	glScaled(6.5, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
	if (wallColor % 2 == 0) {
		glColor3f(0.85f, 0.85f, 0.85f);
	}
	else {
		glColor3f(0.68f, 0.85f, 0.90f);
	}
	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	glTranslated(0.0, 3.0, 1.5);
	glScaled(6.5, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
}
void drawWall3(double thickness) {
	if (wallColor % 2 == 0) {
		glColor3f(0.68f, 0.85f, 0.90f);
	}
	else {
		glColor3f(0.85f, 0.85f, 0.85f);
	}	
	glPushMatrix();
	glRotated(90, 0.0, 0.0, 1.0);
	glTranslated(0.0, -1.0, 0.0);
	glTranslated(0.5, -2.25, 0.0);
	glScaled(1.0, thickness, 6.5);
	glutSolidCube(1);
	glPopMatrix();
	if (wallColor % 2 == 0) {
		glColor3f(0.85f, 0.85f, 0.85f);
	}
	else {
		glColor3f(0.68f, 0.85f, 0.90f);
	}
	glPushMatrix();
	glRotated(90, 0.0, 0.0, 1.0);
	glTranslated(0.0, -1.0, 0.0);
	glTranslated(1.5, -2.25, 0.0);
	glScaled(1.0, thickness, 6.5);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(1.0, 1.0, 1.0);
}
void drawWall4(double thickness) {
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 1.5);
	glScaled(1.0, thickness, 1.0);
	glutSolidCube(1);
	glPopMatrix();
}
void drawFloor(double thickness) {
	glColor3f(0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glScaled(6.5, thickness, 6.5);
	glutSolidCube(1);
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
}
void drawTableLeg(double thick, double len) {
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}
void drawJackPart() {
	glPushMatrix();
	glScaled(0.2, 0.2, 1.0);
	glutSolidSphere(1, 15, 15);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 1.2);
	glutSolidSphere(0.2, 15, 15);
	glTranslated(0, 0, -2.4);
	glutSolidSphere(0.2, 15, 15);
	glPopMatrix();
}
void drawJack() {
	glPushMatrix();
	drawJackPart();
	glRotated(90.0, 0, 1, 0);
	drawJackPart();
	glRotated(90.0, 1, 0, 0);
	drawJackPart();
	glPopMatrix();
}
void drawTable(double topWid, double topThick, double legThick, double legLen) {
	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95 * topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();
}

void drawCar(float x, float y, float z) {
	glPushMatrix();
	if (animating) {
		if (wallColor % 5 == 0) {
			glTranslatef(x, y + 0.25f, z + 0.1f);
		}
		else if (wallColor % 5 == 1) {
			glTranslatef(x, y + 0.25f, z + 0.2f);
		}
		else if (wallColor % 5 == 2) {
			glTranslatef(x, y + 0.25f, z + 0.3f);
		}
		else if (wallColor % 5 == 3) {
			glTranslatef(x, y + 0.25f, z + 0.2f);
		}
		else {
			glTranslatef(x, y + 0.25f, z + 0.1f);
		}
	}
	else {
		glTranslatef(x, y + 0.25f, z);
	}
	glScalef(0.5f, 0.5f, 0.5f);

	// Body of the golf cart
	glColor3f(0.8f, 0.8f, 0.8f); // Light gray color
	glPushMatrix();
	glScalef(1.0f, 0.5f, 2.0f);  // Scale to make a rectangular body
	glutSolidCube(1.0);
	glPopMatrix();

	// Roof of the golf cart
	glColor3f(0.6f, 0.6f, 0.6f); // Darker gray color
	glPushMatrix();
	glTranslatef(0.0f, 0.75f, 0.0f);
	glScalef(1.0f, 0.1f, 2.0f);  // Scale to make a thin roof
	glutSolidCube(1.0);
	glPopMatrix();

	// Front windshield
	glColor3f(0.0f, 0.0f, 0.0f); // Black color
	glPushMatrix();
	glTranslatef(0.0f, 0.5f, 1.0f);
	glScalef(1.0f, 0.5f, 0.1f);  // Scale to make a thin windshield
	glutSolidCube(1.0);
	glPopMatrix();

	// Wheels of the golf cart
	glColor3f(0.0f, 0.0f, 0.0f); // Black color
	for (float wx = -0.6f; wx <= 0.6f; wx += 1.2f) {
		for (float wz = -0.5f; wz <= 0.5f; wz += 1.0f) {
			glPushMatrix();
			glTranslatef(wx, -0.25f, wz); // Position of each wheel
			glRotatef(90, 0.0f, 1.0f, 0.0f); // Rotate to align horizontally
			glutSolidTorus(0.1, 0.2, 10, 10); // Wheel torus
			glPopMatrix();
		}
	}
	glColor3f(1.0f, 1.0f, 1.0f); // Black color

	glPopMatrix(); // Restore transformations
}

void drawCylinder(float x, float y, float z) {
	glPushMatrix();
	if (wallColor % 5 == 0) {
		glTranslatef(x, y, z);
	}
	else if (wallColor % 5 == 1) {
		glTranslatef(x, y + 0.15f, z);
	}
	else if (wallColor % 5 == 2) {
		glTranslatef(x, y + 0.25f, z);
	}
	else if (wallColor % 5 == 3) {
		glTranslatef(x, y + 0.15f, z);
	}
	else {
		glTranslatef(x, y, z);
	}
	//glTranslatef(x, y, z);
	glRotatef(-90, 1.0f, 0.0f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1.0f, 1.0f, 0.0f, 0.5f);

	GLUquadricObj* quadric = gluNewQuadric();
	gluCylinder(quadric, 0.3f, 0.3f, 1.0f, 32, 32); // Radius=0.5, height=2.0

	glDisable(GL_BLEND);
	glPopMatrix();
}

void drawGolfBagWithClubs(float x, float y, float z) {
	glPushMatrix();
	if (animating) {
		if (wallColor % 5 == 0) {
			glTranslatef(x, y + 0.55f, z);
		}
		else if (wallColor % 5 == 1) {
			glTranslatef(x, y + 0.65f, z);
		}
		else if (wallColor % 5 == 2) {
			glTranslatef(x, y + 0.75f, z);
		}
		else if (wallColor % 5 == 3) {
			glTranslatef(x, y + 0.65f, z);
		}
		else {
			glTranslatef(x, y + 0.55f, z);
		}
	}
	else {
		glTranslatef(x, y + 0.55f, z);
	}
	glScalef(0.1f, 0.4f, 0.1f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

	// Golf bag (cylinder)
	glColor3f(0.2f, 0.2f, 0.2f); // Dark gray for the bag
	glPushMatrix();
	glTranslatef(0.0f, 0.5f, 0.0f);
	glRotatef(-90, 1.0f, 0.0f, 0.0f); // Rotate to make the cylinder vertical
	GLUquadric* bag = gluNewQuadric();
	gluCylinder(bag, 0.3, 0.3, 1.0, 30, 30); // Main bag cylinder
	gluDisk(bag, 0.0, 0.3, 30, 1); // Base cap
	glTranslatef(0.0f, 0.0f, 1.0f);
	gluDisk(bag, 0.0, 0.3, 30, 1); // Top cap
	gluDeleteQuadric(bag);
	glPopMatrix();

	// Handle (a thin cylinder on the side)
	glColor3f(0.2f, 0.2f, 0.2f); // Same color as the bag
	glPushMatrix();
	glTranslatef(0.3f, 0.8f, 0.0f);
	glRotatef(90, 0.0f, 1.0f, 0.0f); // Rotate to align horizontally
	GLUquadric* handle = gluNewQuadric();
	gluCylinder(handle, 0.05, 0.05, 0.4, 10, 10); // Handle
	gluDeleteQuadric(handle);
	glPopMatrix();

	// Golf clubs (thin cylinders inside the bag)
	glColor3f(0.7f, 0.7f, 0.7f); // Light gray for the clubs
	float clubPositions[][2] = {
		{0.1f, 0.1f},
		{-0.1f, 0.1f},
		{0.1f, -0.1f},
		{-0.1f, -0.1f},
	};

	for (int i = 0; i < 4; ++i) {
		glPushMatrix();
		glTranslatef(clubPositions[i][0], 1.0f, clubPositions[i][1]);
		glRotatef(90, 1.0f, 0.0f, 0.0f); // Rotate to make club vertical
		GLUquadric* club = gluNewQuadric();
		gluCylinder(club, 0.02, 0.02, 1.0, 10, 10); // Club shaft
		gluDeleteQuadric(club);

		// Club head (small flattened sphere)
		glTranslatef(0.0f, 0.0f, 1.0f);
		glColor3f(0.4f, 0.2f, 0.1f); // Dark brown for club head
		glScalef(1.0f, 0.5f, 0.3f);
		glutSolidSphere(0.07, 10, 10); // Club head
		glPopMatrix();
	}

	glPopMatrix(); // Restore transformations
}

void drawHuman() {
	
	glPushMatrix();
	if (rotated) {
		glRotatef(90.0, 0.0, 1.0, 0.0);	
	}

	// Head
	glPushMatrix();
	glTranslatef(0.0f, 1.35f, 0.0f);
	glColor3f(1.0f, 0.8f, 0.6f);
	glutSolidSphere(0.1, 15, 15); // Head as a sphere
	glPopMatrix();

	// Body
	glPushMatrix();
	glTranslatef(0.0f, 1.0f, 0.0f);
	glScalef(0.2f, 0.5f, 0.1f); // Body as a cuboid
	glColor3f(7.0f, 0.0f, 0.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Left Arm
	glPushMatrix();
	glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
	glTranslatef(0.0, -2.2f, 0.0);
	glPushMatrix();
	glTranslatef(-0.15f, 1.1f, 0.0f);
	glRotatef(20.0, 0, 0, 1);
	glScalef(0.05f, 0.3f, 0.05f); // Arm as a cylinder
	glColor3f(1.0f, 0.8f, 0.6f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Right Arm
	glPushMatrix();
	glTranslatef(0.15f, 1.1f, 0.0f);
	glRotatef(-20.0, 0, 0, 1);
	glScalef(0.05f, 0.3f, 0.05f); // Arm as a cylinder
	glColor3f(1.0f, 0.8f, 0.6f);
	glutSolidCube(1.0);
	glPopMatrix();
	glPopMatrix();


	// Left Leg
	glPushMatrix();
	glTranslatef(-0.07f, 0.75f, 0.0f);
	glScalef(0.05f, 0.3f, 0.05f); // Leg as a cylinder
	glColor3f(0.0f, 0.0f, 7.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	// Right Leg
	glPushMatrix();
	glTranslatef(0.07f, 0.75f, 0.0f);
	glScalef(0.05f, 0.3f, 0.05f); // Leg as a cylinder
	glColor3f(0.0f, 0.0f, 7.0f);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);

}

void setupLights() {
	GLfloat ambient[] = { 0.7f, 0.7f, 0.7, 1.0f };
	GLfloat diffuse[] = { 0.6f, 0.6f, 0.6, 1.0f };
	GLfloat specular[] = { 1.0f, 1.0f, 1.0, 1.0f };
	GLfloat shininess[] = { 50 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 1, 1.0f };
	GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//glRotatef(90.0, 1.0, 1.0, 0.0);
	camera.look();
}
void setupScene() {
	drawFloor(0.02);
	if (!cyl1) {
		drawCylinder(-2.5, 0.0, 0.0);
	}
	if (!cyl2) {
		drawCylinder(2.5, 0.0, 0.0);
	}
	if (!cyl3) {
		drawCylinder(0.0, 0.0, -2.5);
	}
	glColor3f(1.0f, 1.0f, 1.0f);
	drawWall1(0.02);
	drawWall2(0.02);
	drawWall3(0.02);
	drawFlag(0.0, 0.0, 0.0); //the goal
	drawPond(-2.5, 0.0, -2.0);
	drawTiger(2.5, 0.0, -2.0);
	drawCar(-2.5, 0.0, 2.0);
	drawGolfBagWithClubs(2.5f, 0.0f, 2.0f);
	glColor3f(1.0, 1.0, 1.0);
}

void drawPlayer() {
	glPushMatrix();
	/*if (directionX) {
		glTranslatef(playerX, 0, playerZ);
		glRotatef(90, 0.0, 1.0, 0.0);
		glTranslatef(playerX, 0, playerZ);
	}*/
	glTranslatef(playerX, -0.6, playerZ);
	drawHuman();
	glPopMatrix();
}

void gameOver() {
	gameMessage = "GAME LOSE! Score: " + std::to_string(score); // Convert score to string
}

void gameWon() {
	gameMessage = "GAME WON! Score:" + std::to_string(score); // Convert score to string
}

void timer(int) {
	animTimer++;
	if (animTimer % 8 == 0) {
		wallColor++;
	}
	if (locked) {
		cursorSpeedCounter++;
		if (cursorSpeedCounter >= 7) {
			cursorSpeedCounter = 0;
			currentSection = (currentSection + 1) % 7; // Move the cursor to the next section
		}
	}
	if (gameRunning) {
		glutPostRedisplay();
		glutTimerFunc(20, timer, 0);

		timeRemaining -= 0.2f;

		if (ballMoving) {
			ball.shoot();
		}
		if (score == 4) {
			gameRunning = false;
			gameWon();
		}
		else {
			if (timeRemaining <= 0) {
				gameRunning = false;
				gameOver();
			}
		}
	}
	else {
		std::cout << "Game ended" << std::endl;
	}
	
}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	setupCamera();
	setupLights();
	setupScene();

	drawPlayer();

	ball.draw(ball.x, ball.y, ball.z);

	// Step 2: Switch to orthographic projection for 2D overlay
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, 640, 0, 480);  // Set up 2D orthographic projection
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	drawCursosrRect();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	if (!gameMessage.empty()) {
		if (gameMessage.find("GAME WON!") != std::string::npos) {
			PlaySound(TEXT("Sounds/win.wav"), NULL, SND_ASYNC | SND_FILENAME);
			glClearColor(7.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(0.0f, 7.0f, 0.0f); // Set text color to green for "Game Won!"
		}
		else {
			PlaySound(TEXT("Sounds/lose.wav"), NULL, SND_ASYNC | SND_FILENAME);
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(7.0f, 0.0f, 0.0f); // Set text color to red for other messages
		}

		glRasterPos2f(-0.2f, 0.0f); // Position the text in the center

		for (const char& c : gameMessage) {
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c); // Draw each character
		}
	}

	glFlush();
	//glutSwapBuffers();
}

bool infrontOfMeIsOrigin(){
	return ((playerX >= -0.3 && playerX <= 0.3) && (playerZ <= 0.3));
}

bool behindOfMeIsOrigin() {
	return ((playerX >= -0.3 && playerX <= 0.3) && (playerZ >= -0.3));
}

bool leftOfMeIsOrigin() {
	return ((playerZ >= -0.3 && playerZ <= 0.3) && (playerX <= 0.3));
}

bool rightOfMeIsOrigin() {
	return ((playerZ >= -0.3 && playerZ <= 0.3) && (playerX >= -0.3));
}

// hamada
void Keyboard(unsigned char key, int x, int y) {
	float d = 0.01;

	switch (key) {
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;

	case 'f':  // Front view
		camera.setView('F');
		break;
	case 't':  // Top view
		camera.setView('T');
		break;
	case 'r':  // Side view
		camera.setView('R');
		break;
	case 'o':  
		if (directionX) {
			directionX = false;
			directionZ = true;
		}
		else {
			directionX = true;
			directionZ = false;
		}
		rotated = !rotated;
		break;
	case '=':
		if (!locked) {
			if (directionX && playerX < 2.4f) {
				playerX += 0.1;
			}
			else if (directionZ && playerZ > -2.4f) {
				playerZ -= 0.1;
			}
			if ((playerX <= -2.4 && playerX >= -2.6) && (playerZ >= (target - epsilon) && playerZ <= (target + epsilon))) {
				cyl1 = true;
				locked = true;
				cylinder = 1;
				std::cout << "ana wa2ef 3al shemal" << std::endl;

			}
			if ((playerX >= 2.4 && playerX <= 2.6) && (playerZ >= -0.1 && playerZ <= 0.1)) {
				cyl2 = true;
				locked = true;
				cylinder = 2;
				std::cout << "ana wa2ef 3al yemeen" << std::endl;

			}
			if ((playerX >= -0.1 && playerX <= 0.1) && (playerZ >= -2.6 && playerZ <= -2.4)) {
				cyl3 = true;
				locked = true;
				cylinder = 3;
				std::cout << "ana wa2ef odam" << std::endl;

			}
		}
		break;
	case '-':
		if (!locked) {
			if (directionX && playerX > -2.4f) {
				playerX -= 0.1;
			}
			else if (directionZ && playerZ < 2.4f) {
				playerZ += 0.1;
			}
			if ((playerX <= -2.4 && playerX >= -2.6) && (playerZ >= (target - epsilon) && playerZ <= (target + epsilon))) {
				cyl1 = true;
				locked = true;
				cylinder = 1;
				std::cout << "ana wa2ef 3al shemal" << std::endl;

			}
			if ((playerX >= 2.4 && playerX <= 2.6) && (playerZ >= -0.1 && playerZ <= 0.1)) {
				cyl2 = true;
				locked = true;
				cylinder = 2;
				std::cout << "ana wa2ef 3al yemeen" << std::endl;

			}
			if ((playerX >= -0.1 && playerX <= 0.1) && (playerZ >= -2.6 && playerZ <= -2.4)) {
				cyl3 = true;
				locked = true;
				cylinder = 3;
				std::cout << "ana wa2ef odam" << std::endl;

			}
		}
		break;
	case 'p':
		std::cout << playerX << std::endl;
		std::cout << playerZ << std::endl;
		animating = !animating;
		break;
	case ' ' :
		if (!ballMoving) {
			switch (currentSection) {
				case 0: error = -1.5;break;
				case 1: error = -1.0;break;
				case 2: error = -0.5;break;
				case 3: error = 0.0;break;
				case 4: error = 0.5;break;
				case 5: error = 1.0;break;
				case 6: error = 1.5;break;
			}
			//std::cout << currentSection << std::endl;
			if (locked)
				ballMoving = true;
		}
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
		break;
	}

	glutPostRedisplay();
}
void Special(int key, int x, int y) {
	float a = 1.0;

	switch (key) {
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void runPythonScript() {
	//system("\"C:/Users/DELL/AppData/Local/Microsoft/WindowsApps/python.exe\" script.py");
	system("python script.py");
}

void init() {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.68f, 0.85f, 0.90f, 1.0f);  // Set background color
	
}



void main(int argc, char** argv) {

	/*std::thread t2(playCrash);
	t2.detach();*/

	std::thread pythonThread(runPythonScript);
	pythonThread.detach();
	

	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);

	init();

	glutCreateWindow("Olympic Sport");
	glutDisplayFunc(Display);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
}
