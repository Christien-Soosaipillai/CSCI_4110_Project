#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "Person.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// include the STB image library
#define STB_IMAGE_IMPLEMENTATION
#include "apis/stb_image.h"

#include "ShaderProgram.h"
#include "ObjMesh.h"

int width, height;

GLuint programId;
GLuint vertexBuffer;
GLuint indexBuffer;
GLenum positionBufferId;
GLuint positions_vbo = 0;
GLuint textureCoords_vbo = 0;
GLuint normals_vbo = 0;
GLuint colours_vbo = 0;

//drawCube and animation
float yRotation = -85.0f;
float yRotationSpeed = 0.1f;
float time = 0.0f;
bool isAnimating = true;
int frame_delay = 500;
int keyFrame = 1;
int lastFrameMillis = 0;
glm::mat4 torso;
int count = 1;
float t;
int windowId = 0;


unsigned int numVertices;

unsigned int loadTexture(char const * path);

//Forward Declare Functions
void drawCube(glm::mat4 model);


static void createTexture(std::string filename) {
   int imageWidth, imageHeight;
   int numComponents;

   unsigned char* bitmap = stbi_load(filename.c_str(),
                                     &imageWidth,
                                     &imageHeight,
                                     &numComponents, 4);

   GLuint textureId;
   glGenTextures(1, &textureId);

   glBindTexture(GL_TEXTURE_2D, textureId);

   // resizing settings
   glGenerateTextureMipmap(textureId);
   glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

   // provide the image data to OpenGL
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                imageWidth, imageHeight,
                0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);

   glBindTexture(GL_TEXTURE_2D, 0);
   glActiveTexture(GL_TEXTURE0);

   // free up the bitmap
   stbi_image_free(bitmap);
}

static void createGeometry(void) {
  ObjMesh mesh;
  mesh.load("meshes/cube.obj", true, true);

  numVertices = mesh.getNumIndexedVertices();
  Vector3* vertexPositions = mesh.getIndexedPositions();
  Vector2* vertexTextureCoords = mesh.getIndexedTextureCoords();
  Vector3* vertexNormals = mesh.getIndexedNormals();

  glGenBuffers(1, &positions_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertexPositions, GL_STATIC_DRAW);

  glGenBuffers(1, &textureCoords_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, textureCoords_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2), vertexTextureCoords, GL_STATIC_DRAW);

  glGenBuffers(1, &normals_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3), vertexNormals, GL_STATIC_DRAW);

  unsigned int* indexData = mesh.getTriangleIndices();
  int numTriangles = mesh.getNumTriangles();

  glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numTriangles * 3, indexData, GL_STATIC_DRAW);
}

float angle = 0.0f;

static void update(void) {
    int milliseconds = glutGet(GLUT_ELAPSED_TIME);
  	time = ((float)milliseconds / 500.0f) * (isAnimating ? 1.0f : 0.0f) * 5;

      // we'll rotate our model by an ever-increasing angle so that we can see the texture
  	// rotate the entire model, to that we can examine it
  	yRotation += yRotationSpeed;

  	// update the bones
  	if (isAnimating && ((milliseconds - lastFrameMillis) > frame_delay)) {
  		lastFrameMillis = milliseconds;
  		keyFrame++;

  		if (keyFrame > 7) {
  			keyFrame = 0;
  		}
  	}
      glutPostRedisplay();
}

static void render(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// activate our shader program
	glUseProgram(programId);

	// turn on depth buffering
	glEnable(GL_DEPTH_TEST);
	
	if(count == 7175){
		glutDestroyWindow(windowId);
      	exit(0);
	}

	t += (float)1/ (float)7175;
	//std::cout << t << std::endl; 

	Person person(time,yRotationSpeed, yRotation, t);
    std::vector<glm::mat4> vec = person.getBodyVector();
    count+=1;

   for(int i = 0; i < vec.size(); i++){
   		drawCube(vec[i]);	
   }

	// make the draw buffer to display buffer (i.e. display what we have drawn)
	glutSwapBuffers();
}

void drawCube(glm::mat4 model){ 

	glm::mat4 view = glm::lookAt(
		glm::vec3(0, 20, 80), // eye/camera location
		glm::vec3(0, 0, 0),    // where to look
		glm::vec3(0, 1, 0)     // up
	);

	float aspectRatio = (float)width / (float)height;
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

  // model-view-projection matrix
  glm::mat4 mvp = projection * view * model;
  GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVP");
  glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

  // texture sampler - a reference to the texture we've previously created
  GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
  glActiveTexture(GL_TEXTURE0);  // texture unit 0
  glBindTexture(GL_TEXTURE_2D, textureId);
  glUniform1i(textureId, 0);

  // find the names (ids) of each vertex attribute
  GLint positionAttribId = glGetAttribLocation(programId, "position");
  GLint textureCoordsAttribId = glGetAttribLocation(programId, "textureCoords");
  GLint normalAttribId = glGetAttribLocation(programId, "normal");

  // provide the vertex positions to the shaders
  glBindBuffer(GL_ARRAY_BUFFER, positions_vbo);
  glEnableVertexAttribArray(positionAttribId);
  glVertexAttribPointer(positionAttribId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // provide the vertex texture coordinates to the shaders
  glBindBuffer(GL_ARRAY_BUFFER, textureCoords_vbo);
  glEnableVertexAttribArray(textureCoordsAttribId);
  glVertexAttribPointer(textureCoordsAttribId, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

  // provide the vertex normals to the shaders
  glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
  glEnableVertexAttribArray(normalAttribId);
  glVertexAttribPointer(normalAttribId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// draw the triangles
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, (void*)0);

  // for testing purposes
  //glutSolidTorus(0.5f, 1.5f, 12, 10);

	// disable the attribute arrays
  glDisableVertexAttribArray(positionAttribId);
  glDisableVertexAttribArray(textureCoordsAttribId);
  glDisableVertexAttribArray(normalAttribId);	
}

static void reshape(int w, int h) {
  // efficiency note:  calculating the projection matrix here only when resizing
  //                   would save much computation in the render() method.
  glViewport(0, 0, w, h);

  width = w;
  height = h;
}

static void drag(int x, int y) {
}

static void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    }
}

static void keyboard(unsigned char key, int x, int y) {
	if (key == 'r') {
		if (yRotationSpeed > 0.0) {
			yRotationSpeed = 0.0;
		}
		else {
			yRotationSpeed = 0.1;
		}
		std::cout << "Toggling rotation, speed: " << yRotationSpeed << std::endl;
	}
	else if (key == 'a') {
		isAnimating = !isAnimating;
		std::cout << "Toggling animation: " << isAnimating << std::endl;
		std::cout << "Key frame: " << keyFrame << std::endl;
	}
	

	std::cout << "Key pressed: " << key << std::endl;
}

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize(800, 600);
   windowId = glutCreateWindow("Brick Olympics");
   glutIdleFunc(&update);
   glutDisplayFunc(&render);
   glutReshapeFunc(&reshape);
   glutMotionFunc(&drag);
   glutMouseFunc(&mouse);
   glutKeyboardFunc(&keyboard);

  
   glewInit();
   if (!GLEW_VERSION_2_0) {
        std::cerr << "OpenGL 2.0 not available" << std::endl;
        return 1;
   }
   std::cout << "Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	 std::cout << "Using OpenGL " << glGetString(GL_VERSION) << std::endl;

   createGeometry();
   createTexture("textures/planks.jpg");

   ShaderProgram program;
   program.loadShaders("shaders/vertex.glsl", "shaders/fragment.glsl");
   programId = program.getProgramId();

   glutMainLoop();

   return 0;
}
