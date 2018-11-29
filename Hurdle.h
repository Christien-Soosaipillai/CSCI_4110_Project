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

class Hurdle {


public:

	Hurdle(float point, float increment);
	float startPoint;
	float incrementDistance;
	std::vector<glm::mat4> Hurdle::getHurdleVector();


};