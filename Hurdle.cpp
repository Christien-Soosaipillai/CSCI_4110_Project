#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <cmath>
#include <vector>
#include <GL/glew.h>
#include "Hurdle.h"
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

Hurdle::Hurdle(float point, float increment) {

	startPoint = point;
	incrementDistance = increment; // need to calculate distance to increment

}

std::vector<glm::mat4> Hurdle::getHurdleVector() {

	float x = startPoint;
	float z = incrementDistance;

	std::vector<glm::mat4> hurdleComponents;

	glm::mat4 baseMatrix = glm::mat4(1.0f);

	glm::mat4 hBar, vBar1, vBar2;

	hBar = baseMatrix;
	hBar = glm::translate(hBar, glm::vec3(x, -3.0f, incrementDistance));
	hurdleComponents.push_back(glm::scale(hBar, glm::vec3(15.0f, 2.0f, 1.0f)));		//instead of having to undo the scale we are calling the function instead of saving to the torso baseMatrix

	//right pillar
	vBar1 = hBar;															//no scale from our torso because of previous line of code
	vBar1 = glm::translate(vBar1, glm::vec3(8.0f, -4.0f, 0.0f));			//The pivot
	hurdleComponents.push_back(glm::scale(vBar1, glm::vec3(1.0f, 8.0f, 4.0f)));


	//left pillar
	vBar2 = hBar;
	vBar2 = glm::translate(vBar2, glm::vec3(-8.0f, -4.0f, 0.0f));
	hurdleComponents.push_back(glm::scale(vBar2, glm::vec3(1.0f, 8.0f, 4.0f)));


	return hurdleComponents;
}