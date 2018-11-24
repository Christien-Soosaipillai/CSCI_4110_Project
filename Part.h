//Function Declarations
#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <cmath>
#include <vector>
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#pragma once

class Part{

public:

	//Overloaded Constructor
	Part(glm::mat4, glm::vec4);

	//Getter Functions

	//get matrix for body part
	glm::mat4 getPart();

	//get color
	glm::vec4 getColor();

private:

	glm::mat4 bodyPart;
	glm::vec4 color;

};
