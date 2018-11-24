#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <cmath>
#include <vector>
#include <GL/glew.h>
#include "Part.h"
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

//constructor
Part::Part(glm::mat4 bP, glm::vec4 c){

	glm::mat4 bodyPart = bP;
	glm::vec4 color = c;

}


glm::mat4 Part::getPart(){
	return bodyPart;
}

glm::vec4 Part::getColor(){
	return color;
}