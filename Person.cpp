#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <cmath>
#include <vector>
#include <GL/glew.h>
#include "Person.h"
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


Person::Person(float thisTime, float rotSpeed, float yRot, float count, float step, float start, double jumpTime) {
	time = thisTime;
	yRotationSpeed = rotSpeed;
	yRotation = yRot;
	t = count;
	runStep = step;
	startInitial = start;
	jTime = jumpTime;

}

std::vector<glm::mat4> Person::getBodyVector() {

	//bezier control points
	glm::vec3 controlPoint1(00.00f, 0.00f, 0.00f);
	glm::vec3 controlPoint2(15.00f, 0.00f, 0.00f);
	glm::vec3 controlPoint3(32.00f, 0.00f, 0.00f);
	glm::vec3 controlPoint4(44.00f, 0.00f, 0.00f);

	//matrix for each heirarchial transformation
	glm::mat4 torso, neck, head, rEye, lEye, upperRLeg, lowerRLeg, rFoot, upperLLeg, lowerLLeg, lFoot,
		upperRArm, lowerRArm, rHand, upperLArm, lowerLArm, lHand;
	
	if (jTime > 0.0010){
		x = startInitial;
		y = (float) (2.00 * ((5.00 * jTime) - pow(jTime, 2.00)));				//formula used to calculate jump height ->2(5*dt - dt^2) ->formula will give us a noticable enough jump height
		z = -runStep;															//value of steps taken 
	
	}
	else {

		//beszier implementation
		x = startInitial;
		y = pow((1 - t), 3) * (controlPoint1.y) + pow((1 - t), 2) * (3 * t) * (controlPoint2.y) + (1 - t) * (3 * (pow(t, 2))) * (controlPoint3.y) + pow(t, 3) * (controlPoint4.y);
		z = -runStep;
	}

	//stores our body parts
	std::vector<glm::mat4> bodyParts;

	// model matrix: translate, scale, and rotate the model
	glm::vec3 rotationAxis(0, 1, 0);

	glm::mat4 model = glm::mat4(1.0f);

	torso = model;
	torso = glm::translate(torso, glm::vec3(x, y, z));         //running code
	torso = glm::rotate(torso, glm::radians(2.0f) * sin(time), glm::vec3(0.0f, 0.0f, 1.0f));
	torso = glm::rotate(torso, glm::radians(2.0f) * sin(time), glm::vec3(0.0f, 1.0f, 0.0f));


	bodyParts.push_back(glm::scale(torso, glm::vec3(3.0f, 4.0f, 1.1f)));

	neck = torso;
	neck = glm::translate(neck, glm::vec3(0.0f, 3.5f, 0.0f));
	neck = glm::rotate(neck, glm::radians(-2.0f) * sin(time), glm::vec3(0.0f, 0.0f, 1.0f));
	bodyParts.push_back(glm::scale(neck, glm::vec3(1.0f, 1.0f, 0.8f)));

	head = neck;
	neck = glm::translate(neck, glm::vec3(0.0f, 3.0f, 0.0f));
	bodyParts.push_back(glm::scale(neck, glm::vec3(2.0f, 2.0f, 2.0f)));

	rEye = head;
	rEye = glm::translate(rEye, glm::vec3(1.0f, 3.0f, -2.0f));
	bodyParts.push_back(glm::scale(rEye, glm::vec3(0.3f, 0.3f, 0.3f)));

	lEye = head;
	lEye = glm::translate(lEye, glm::vec3(-1.0f, 3.0f, -2.0f));
	bodyParts.push_back(glm::scale(lEye, glm::vec3(0.3f, 0.3f, 0.3f)));

	//RIGHT SIDE OF BODY


  //right arm upper
	upperRArm = torso;                              //no scale from our torso because of previous line of code
	upperRArm = glm::translate(upperRArm, glm::vec3(3.5f, 3.0f, 0.0f));     //The pivot
	upperRArm = glm::rotate(upperRArm, glm::radians(-40.0f) * sin(time), glm::vec3(1.0f, 0.0f, 0.0f));
	upperRArm = glm::rotate(upperRArm, glm::radians(10.0f) * sin(time) + 31.8f, glm::vec3(0.0f, 1.0f, 0.0f));
	bodyParts.push_back(glm::scale(upperRArm, glm::vec3(0.8f)));           //yellow Cube
	upperRArm = glm::translate(upperRArm, glm::vec3(1.0f, -1.0f, 0.0f));
	bodyParts.push_back(glm::scale(upperRArm, glm::vec3(1.0f, 2.0f, 1.0f)));

	//right arm lower
	lowerRArm = upperRArm;                              //no scale from our torso because of previous line of code
	lowerRArm = glm::translate(lowerRArm, glm::vec3(0.0f, -2.0f, 0.0f));      //The pivot
	lowerRArm = glm::rotate(lowerRArm, glm::radians(-20.0f) * sin(time) + 31.8f, glm::vec3(1.0f, 0.0f, 0.0f));
	bodyParts.push_back(glm::scale(lowerRArm, glm::vec3(0.7f)));           //yellow Cube
	lowerRArm = glm::translate(lowerRArm, glm::vec3(0.0f, -2.0f, 0.0f));
	bodyParts.push_back(glm::scale(lowerRArm, glm::vec3(0.8f, 2.0f, 0.8f)));

	//right hand
	rHand = lowerRArm;                              //no scale from our torso because of previous line of code
	rHand = glm::translate(rHand, glm::vec3(0.0f, -2.0f, 0.0f));      //The pivot
	rHand = glm::rotate(rHand, glm::radians(20.0f) * sin(time) + 31.8f, glm::vec3(1.0f, 0.0f, 0.0f));
	bodyParts.push_back(glm::scale(rHand, glm::vec3(0.7f)));           //yellow Cube

	//right leg upper
	upperRLeg = torso;                              //no scale from our torso because of previous line of code
	upperRLeg = glm::translate(upperRLeg, glm::vec3(2.0f, -4.0f, 0.0f));      //The pivot
	upperRLeg = glm::rotate(upperRLeg, glm::radians(-40.0f) * sin(time), glm::vec3(1.0f, 0.0f, 0.0f));
	bodyParts.push_back(glm::scale(upperRLeg, glm::vec3(0.8f)));           //yellow Cube
	upperRLeg = glm::translate(upperRLeg, glm::vec3(0.0f, -2.2f, 0.0f));
	bodyParts.push_back(glm::scale(upperRLeg, glm::vec3(1.0f, 2.0f, 1.0f)));

	//right leg lower
	lowerRLeg = upperRLeg;
	lowerRLeg = glm::translate(lowerRLeg, glm::vec3(0.0f, -2.0f, 0.0f));
	lowerRLeg = glm::rotate(lowerRLeg, glm::radians(10.0f) * sin(time) - 32.1f, glm::vec3(1.0f, 0.0f, 0.0f));
	bodyParts.push_back(glm::scale(lowerRLeg, glm::vec3(0.75f)));            //yellow Cube
	lowerRLeg = glm::translate(lowerRLeg, glm::vec3(0.0f, -2.0f, 0.0f));
	bodyParts.push_back(glm::scale(lowerRLeg, glm::vec3(0.8f, 2.0f, 1.0f)));

	//right foot
	rFoot = lowerRLeg;
	rFoot = glm::translate(rFoot, glm::vec3(0.0f, -1.0f, 0.0f));
	rFoot = glm::rotate(rFoot, glm::radians(-25.0f) * sin(time), glm::vec3(1.0f, 0.0f, 0.0f)); //1st: model 2nd: rotation 3rd:axis we rotate on
	bodyParts.push_back(glm::scale(rFoot, glm::vec3(0.75f, 0.50f, 0.75f)));            //yellow Cube
	rFoot = glm::translate(rFoot, glm::vec3(0.0f, -1.0f, -0.5f));
	bodyParts.push_back(glm::scale(rFoot, glm::vec3(1.0f, 0.5f, 1.5f)));


	//LEFT SIDE OF BODY

	//Left arm upper 
	upperLArm = torso;                              //no scale from our torso because of previous line of code
	upperLArm = glm::translate(upperLArm, glm::vec3(-3.5f, 3.0f, 0.0f));      //The pivot
	upperLArm = glm::rotate(upperLArm, glm::radians(40.0f) * sin(time), glm::vec3(1.0f, 0.0f, 0.0f));
	upperLArm = glm::rotate(upperLArm, glm::radians(-10.0f) * sin(time) - 31.8f, glm::vec3(0.0f, 1.0f, 0.0f));
	bodyParts.push_back(glm::scale(upperLArm, glm::vec3(0.8f)));           //yellow Cube
	upperLArm = glm::translate(upperLArm, glm::vec3(-1.0f, -1.0f, 0.0f));
	bodyParts.push_back(glm::scale(upperLArm, glm::vec3(1.0f, 2.0f, 1.0f)));

	//left arm lower
	lowerLArm = upperLArm;                              //no scale from our torso because of previous line of code
	lowerLArm = glm::translate(lowerLArm, glm::vec3(0.0f, -2.0f, 0.0f));      //The pivot
	lowerLArm = glm::rotate(lowerLArm, glm::radians(20.0f) * sin(time) + 31.8f, glm::vec3(1.0f, 0.0f, 0.0f));
	bodyParts.push_back(glm::scale(lowerLArm, glm::vec3(0.7f)));           //yellow Cube
	lowerLArm = glm::translate(lowerLArm, glm::vec3(0.0f, -2.0f, 0.0f));
	bodyParts.push_back(glm::scale(lowerLArm, glm::vec3(0.8f, 2.0f, 0.8f)));

	//left hand
	lHand = lowerLArm;                              //no scale from our torso because of previous line of code
	lHand = glm::translate(lHand, glm::vec3(0.0f, -2.0f, 0.0f));      //The pivot
	lHand = glm::rotate(lHand, glm::radians(20.0f) * sin(time) + 31.8f, glm::vec3(1.0f, 0.0f, 0.0f));
	bodyParts.push_back(glm::scale(lHand, glm::vec3(0.7f)));           //yellow Cube

	//Left leg upper
	upperLLeg = torso;                              //no scale from our torso because of previous line of code
	upperLLeg = glm::translate(upperLLeg, glm::vec3(-2.0f, -4.0f, 0.0f));     //The pivot
	upperLLeg = glm::rotate(upperLLeg, glm::radians(40.0f) * sin(time), glm::vec3(1.0f, 0.0f, 0.0f));
	bodyParts.push_back(glm::scale(upperLLeg, glm::vec3(0.8f)));           //yellow Cube
	upperLLeg = glm::translate(upperLLeg, glm::vec3(0.0f, -2.0f, 0.0f));
	bodyParts.push_back(glm::scale(upperLLeg, glm::vec3(1.0f, 2.0f, 1.0f)));

	//left leg lower
	lowerLLeg = upperLLeg;
	lowerLLeg = glm::translate(lowerLLeg, glm::vec3(0.0f, -2.0f, 0.0f));
	lowerLLeg = glm::rotate(lowerLLeg, glm::radians(-10.0f)* sin(time) - 32.1f, glm::vec3(1.0f, 0.0f, 0.0f));
	bodyParts.push_back(glm::scale(lowerLLeg, glm::vec3(0.75f)));            //yellow Cube
	lowerLLeg = glm::translate(lowerLLeg, glm::vec3(0.0f, -2.0f, 0.0f));
	bodyParts.push_back(glm::scale(lowerLLeg, glm::vec3(0.8f, 2.0f, 1.0f)));

	//left foot
	lFoot = lowerLLeg;
	lFoot = glm::translate(lFoot, glm::vec3(0.0f, -1.0f, 0.0f));
	lFoot = glm::rotate(lFoot, glm::radians(25.0f) * sin(time), glm::vec3(1.0f, 0.0f, 0.0f)); //1st: model 2nd: rotation 3rd:axis we rotate on
	bodyParts.push_back(glm::scale(lFoot, glm::vec3(0.75f, 0.50f, 0.75f)));            //yellow Cube
	lFoot = glm::translate(lFoot, glm::vec3(0.0f, -1.0f, -0.5f));
	bodyParts.push_back(glm::scale(lFoot, glm::vec3(1.0f, 0.5f, 1.5f)));


	return bodyParts;
}

bool Person::isFinished() {

	if (runStep >= 200) {		//check if the distance is around 200
		return true;
	}
	else {
		return false;
	}
}

