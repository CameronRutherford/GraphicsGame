// Walk through maze for computer graphics
// Made by: Cameron Rutherford
// Last Edited: 4/15/2019

#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <stdio.h>
#include <time.h>
#include <string>
#include <cassert>
#include <vector>
#include "Angel.h"
#include <GL/glew.h> // for OpenGL extensions
#include <GL/glut.h> // for Glut utility kit

// Global Projection Matrices
mat4 projection, modelview, translate;  
vec4 view(0.0, 0.0, -2.0, 0.0);
vec4 move_back_or_forward;
vec4 move_left_or_right;
GLfloat dir = 1.0;
GLint axis = 1;

#include "texture.h" // for the bitmap texture loader
#include "SkyBox.h"
#include "Brick.h"
#include "GraphicsObject.h"
#include "ObjectCollision.h"

using namespace std;

// THE global SkyBox Object
SkyBox go_skybox;

// Used to determine object collision and stores the location of all the walls and floor
Environment env;

// The objects
GLfloat  aspect = 1.0;       // Viewport aspect ratio
point4  eye(0.0, 0.0, 1.0, 1.0);
point4  at;
vec4    up(0.0, 1.0, 0.0, 0.0);

int width = 0, height = 0;
float mouse_sensitivity_x = 0.05f;
float mouse_sensitivity_y = 0.0015f;
bool a_down = false;
bool s_down = false;
bool d_down = false;
bool w_down = false;
float locked_mouse = true;
int accum_dy = 0;
const int maze_size = 13;

void display( void )
{
	static float angle = 0.0;

	glClearColor(1.0,1.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );  /*clear the window */

	GLfloat  fovy = 60.0;		 // Field-of-view in Y direction angle (in degrees)
	GLfloat  zNear = 0.01f, zFar = 1000.0f;

	projection = Perspective(fovy, aspect, zNear, zFar);
	modelview = Translate(0.0, 0.0, 1.0)*LookAt(eye, at, up);
	GLfloat theta[] = { 0.0,0.0,0.0 };

	// tell the skybox to draw its vertex
	go_skybox.draw( theta );

	// tell the bricks to draw themselves and rotate too!
	env.draw();

	// The new coordinates the player would have moved to
	point4 new_eye = eye;
	point4 new_at = at;

	// Figure out the velocity
	if (!(a_down && d_down)) {
		if (a_down) {
			new_eye += move_left_or_right;
			new_at += move_left_or_right;
		}
		else if (d_down) {
			new_eye -= move_left_or_right;
			new_at -= move_left_or_right;
		}
	}
	if (!(w_down && s_down)) {
		if (w_down) {
			new_eye += move_back_or_forward;
			new_at += move_back_or_forward;
		}
		else if (s_down) {
			new_eye -= move_back_or_forward;
			new_at -= move_back_or_forward;
		}
	}

	// Check to see if there would be collision by the player moving to the new point
	if (!env.collision(new_eye)) {
		at = new_at;
		eye = new_eye;
	}

	// swap the buffers
	glutSwapBuffers();

	glutPostRedisplay();
}

//Function called when the mouse passively moves
void mouse_moving(int x, int y) {

	if (locked_mouse) {
		int center_x = int(width / 2);
		int center_y = int(height / 2);

		int dx = x - center_x;
		int dy = y - center_y;

		//Matrix that will rotate the various vectors
		mat4 Rotator = RotateY(-dx * mouse_sensitivity_x);

		//Change the view vector based on the mouse movement
		accum_dy += dy;
		view = Rotator * view;
		at = eye + view;
		at.y -= accum_dy * mouse_sensitivity_y;

		//Change the direction that you are moving in the same way
		move_back_or_forward = Rotator * move_back_or_forward;
		move_left_or_right = Rotator * move_left_or_right;

		//Move the cursor back to the middle of the screen, and make it invisible again
		glutWarpPointer(center_x, center_y);
		glutSetCursor(GLUT_CURSOR_NONE);
	}
	else {
		glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
	}
}

//Called when the screen is reshaped
void myReshape(int w, int h)
{
	width = w;
	height = h;
    glViewport(0, 0, w, h);
	aspect =  GLfloat (w) / h;
}

//Function that detects when a key is pressed
void press_key(unsigned char key, int x, int y) //MAKE ZOOM IN AND OUT WORK
{
	if (key == 27) locked_mouse = !locked_mouse; //This is the escape key
	if (key == 'w') w_down = true;
	if (key == 'a')	a_down = true;
	if (key == 's') s_down = true;
	if (key == 'd') d_down = true;
    if(key == 'q') exit(0);
	glutPostRedisplay();
}

//Function that detects when a key is released
void release_key(unsigned char key, int x, int y) {
	if (key == 'a') a_down = false;
	if (key == 'w') w_down = false;
	if (key == 's') s_down = false;
	if (key == 'd') d_down = false;
}

void init_gl()
{
	glEnable(GL_DEPTH_TEST);
}

void init()
{   
	init_gl();			    // Setup general OpenGL stuff of the object //could do all of this by creating  skybox.init function that does all 5 things

	go_skybox.init_data();	        // Setup the data for the this object
	go_skybox.init_VAO();           // Initialize the vertex array object for this object
	go_skybox.init_VBO();			// Initialize the data buffers for this object
	go_skybox.init_shader();		// Initialize the shader objects and textures for skybox
	go_skybox.init_texture_map();	// Initialize the texture map for this object
	
	env.initialize_bricks();
	float br = env.brick_radius();
	float cr = env.brick_clip_radius();

	// Chose which maze file to load "randomly" and then get the information from the text file
	srand((unsigned int)time(NULL));
	int i = rand() % 10;
	string file = "Mazes/maze_" + to_string(i) + ".txt";

	cout << "Using maze number " + to_string(i) << endl;
	ifstream fs(file);

	int maze[maze_size][maze_size]; // Will have a 1 stored if the grid location of the maze is filled in, a 0 if not

	int y_count = 0, x_count = 0;

	for (string line; std::getline(fs, line); )
	{
		for (char& c : line) {
			if (c == '1') {
				maze[x_count][y_count] = 1;
			}
			else {
				maze[x_count][y_count] = 0;
			}
			x_count++;
		}
		y_count++;
		x_count = 0;
	}

	float max = go_skybox.get_max_brick_coord(br);
	point4 delta;
	vec3 hope;

	for (float x = -max, x_i = 0, x_m = 0; x <= max; x += 2 * br, x_i++) {
		for (float y = -max, y_i = 0, y_m = 0; y <= max; y += 2 * br, y_i++) {
			if (maze[(int)x_m][(int)y_m] == 1) {
				env.add_brick(vec3(x, 0, y));
			}
			else {
				delta = vec4(x - br, 0.0, y -br, 1.0) - eye;
				env.add_brick(vec3(x, -2 * br, y)); //Comment out this line to remove the floor
				//env.add_brick(vec3(x, 2 * br, y)); //Comment out this line to remove the ceiling
			}
			if ((int)y_i % 2 == 1) {
				y_m++;
			}
		}
		if ((int)x_i % 2 == 1) {
			x_m++;
		}
	}

	//Update the player's position, and initialize the movement vectors
	{
		eye += delta;
		at = eye + point4(0.0, 0.0, -2.0, 0.0);

		float movement_sensitivity = 0.09f;
		move_back_or_forward = normalize((at - eye)) * movement_sensitivity;
		move_left_or_right = -normalize(cross(move_back_or_forward, up)) * movement_sensitivity;
	}

	GL_CHECK_ERRORS
}

void OnShutdown()
{
	go_skybox.cleanup(); // release the textures on the graphics card
}

void checkGlew()
{
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)	{
		cerr<<"Error: " << glewGetErrorString(err)<<endl;
	} else {
		if (GLEW_VERSION_3_3)
		{
			cout<<"Driver supports OpenGL 3.3\nDetails:"<<endl;
		}
	}
	cout<<"Using GLEW "<<glewGetString(GLEW_VERSION)<<endl;
	cout<<"Vendor: "<<glGetString (GL_VENDOR)<<endl;
	cout<<"Renderer: "<<glGetString (GL_RENDERER)<<endl;
	cout<<"Version: "<<glGetString (GL_VERSION)<<endl;
	cout<<"GLSL: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<endl;
}

int main(int argc, char **argv)
{
	atexit(OnShutdown);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow( "SkyBox" );

	checkGlew();
    init();
    glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(mouse_moving);
    glutKeyboardFunc(press_key);
	glutKeyboardUpFunc(release_key);


	cout << "*****************************************************" << endl;
	cout << "*   wasd moves around" << endl;
	cout << "*   mouse moves camera" << endl;
	cout << "*   esc unlocks mouse" << endl;
	cout << "*****************************************************" << endl;

    glutMainLoop();

    return 0;
}
