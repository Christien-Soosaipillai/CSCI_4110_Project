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

class Person {
	
public:

	Person(float thisTime, float rotSpeed, float yRot, float count, float step, float start, double jumpTime);
	float time, yRotationSpeed, yRotation, t, runStep, startInitial, x, y, z;
	double jTime;
	//return vector of mat4 for each body part
	std::vector<glm::mat4> getBodyVector();
	bool isFinished();


};

