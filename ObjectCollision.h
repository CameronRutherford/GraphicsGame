#ifndef __OBJECT_COLLISION_H__
#define __OBJECT_COLLISION_H__

#include <vector>
#include "Brick.h"
#include "Angel.h"
#include <math.h>

using namespace std;

class Environment {
private:
	vector<vec3> bricks; // Stores the location of where to draw the bricks
	Brick brick; //Keep this here, and don't move it to a global variable
public:
	Environment() {}

	void initialize_bricks() {
		// Initialize the brick
		brick.init_data();	        // Setup the data for the skybox object
		brick.init_VAO();          // Initialize the vertex array object for this object
		brick.init_VBO();			// Initialize the data buffers for this object
		brick.init_shader();		// Initialize the shader objects and textures for skybox
		brick.init_texture_map();	// Initialize the texture map for this object
	}
	
	void add_brick(vec3 offset) {
		bricks.push_back(offset);
	}

	
	float brick_radius() {
		return brick.get_brick_radius();
	}

	float brick_clip_radius() {
		return brick.get_clip_radius();
	}

	bool collision(point4 new_pos) {

		//Check to see if you hit a brick
		for (unsigned int i = 0; i < bricks.size(); i++) {
			vec3 curr_centre = bricks[i];
			float radius = brick.get_clip_radius();
			if (pow((new_pos.x - curr_centre.x), 2) + pow((new_pos.y - curr_centre.y), 2) + pow((new_pos.z - curr_centre.z), 2) <= pow(radius, 2)) {
				return true;
			}
		}

		return false;
	}

	//Draw all the things in the environment
	void draw() {
		for (unsigned int i = 0; i < bricks.size(); i++) {
			brick.draw(bricks[i]);
		}	
	}
};

#endif