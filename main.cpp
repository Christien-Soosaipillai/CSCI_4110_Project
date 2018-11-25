#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <GL/glew.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "apis/stb_image.h"

#include "ShaderProgram.h"
#include "ObjMesh.h"

//person class
#include "Person.h"


int width = 1024;
int height = 768;

unsigned int skyboxTexture;
unsigned int allTextures[3];
unsigned int numVertices;

GLuint programId;
GLuint skyboxProgramId;

//buffers
GLuint vertexBuffer;
GLuint indexBuffer;
GLenum positionBufferId;
GLuint positions_vbo = 0;
GLuint textureCoords_vbo = 0;
GLuint normals_vbo = 0;
GLuint colours_vbo = 0;
GLuint skybox_vbo = 0;

//camera initial position
glm::vec3 eyePosition = glm::vec3(0.0f, 50.0f, 150.0f);
glm::mat4 view;
glm::mat4 projection;
float aspectRatio;

//for camera rotation
float angle = 0.0f;
bool rotateObject = true;


// backup of view and perspective
// view matrix - orient everything around our preferred view
// view = glm::lookAt( eyePosition,
//               glm::vec3(0,0,0),    // where to look
//               glm::vec3(0,1,0)     // up
// );
// view = glm::rotate(view, glm::radians(angle), glm::vec3(0, 1, 0));
// aspectRatio = (float)width / (float)height;
// projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);


//Christian Variables
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
  int run;

  //Forward Declare Functions
  void drawCube(glm::mat4 model);

// static void createTexture(std::string filename);
static unsigned int createTexture(std::string filename);
static unsigned int createCubemap(std::vector<std::string> filenames);

static void createSkybox(void) {
  float skyboxPositions[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };
  unsigned int numVertices = 36;

  glGenBuffers(1, &skybox_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(float) * 3, skyboxPositions, GL_STATIC_DRAW);
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

static void update(void) {
   int milliseconds = glutGet(GLUT_ELAPSED_TIME);
   //christian update function
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
  //end of his update function

   // rotate the shape about the y-axis so that we can see the shading
   if (rotateObject) {
      float degrees = (float)milliseconds / 80.0f;
      angle = degrees;
   }

   glutPostRedisplay();
}

glm::mat4 model;

static void render(void) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // view and perspective
    // view matrix - orient everything around our preferred view
    view = glm::lookAt( eyePosition,
                        glm::vec3(0,0,0),    // where to look
                        glm::vec3(0,1,0)     // up
    );
    view = glm::rotate(view, glm::radians(angle), glm::vec3(0, 1, 0));
    aspectRatio = (float)width / (float)height;
    projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 1000.0f);

    // draw the cube map sky box
    {
      // provide the vertex positions to the shaders
      GLint skyboxPositionAttribId = glGetAttribLocation(skyboxProgramId, "position");
      glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
      glEnableVertexAttribArray(skyboxPositionAttribId);
      glVertexAttribPointer(skyboxPositionAttribId, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

      // texture sampler - a reference to the texture we've previously created
      GLuint skyboxTextureId  = glGetUniformLocation(skyboxProgramId, "u_TextureSampler");
      glActiveTexture(GL_TEXTURE0);  // texture unit 0
      glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
      glUniform1i(skyboxTextureId, 0);

      glUseProgram(skyboxProgramId);

      glDepthMask(GL_FALSE);
      glDisable(GL_DEPTH_TEST);
      glFrontFace(GL_CCW);
      glDisable(GL_CULL_FACE);

      // set model-view matrix

      GLuint skyboxMVMatrixId = glGetUniformLocation(skyboxProgramId, "u_MVMatrix");
      glUniformMatrix4fv(skyboxMVMatrixId, 1, GL_FALSE, &view[0][0]);

      // set projection matrix
      GLuint skyboxProjMatrixId = glGetUniformLocation(skyboxProgramId, "u_PMatrix");
      glUniformMatrix4fv(skyboxProjMatrixId, 1, GL_FALSE, &projection[0][0]);

      glBindVertexArray(skyboxPositionAttribId);
      glDrawArrays(GL_TRIANGLES, 0, 36);

      // disable the attribute array
      glDisableVertexAttribArray(skyboxPositionAttribId);
    }



    // draw the floor
    {
      // activate our shader program
      glUseProgram(programId);

      // turn on depth buffering
      glDepthMask(GL_TRUE);
      glEnable(GL_DEPTH_TEST);

      // model matrix: translate, scale, and rotate the model
      model = glm::mat4(1.0f);
      // model = glm::mat4_cast(rotation);
      model = glm::translate(model, glm::vec3(0.0f, -12.0f, 0.0f));    
      model = glm::scale(model, glm::vec3(100.0f, 0.01f, 100.0f));

      // model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

      // model-view-projection matrix
      glm::mat4 mvp = projection * view * model;
      GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
      glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

      // model matrix
      GLuint mMatrixId = glGetUniformLocation(programId, "u_ModelMatrix");
      glUniformMatrix4fv(mMatrixId, 1, GL_FALSE, &model[0][0]);

      GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
      glActiveTexture(GL_TEXTURE0);  // texture unit 0
      glBindTexture(GL_TEXTURE_2D, allTextures[0]);
      glUniform1i(textureId, 0);

      // the position of our camera/eye
      GLuint eyePosId = glGetUniformLocation(programId, "u_EyePosition");
      glUniform3f(eyePosId, eyePosition.x, eyePosition.y, eyePosition.z);
      
      //light position. point to the right
      glm::vec3 lightPosDir = glm::vec3(0.0f, 50.0f, 150.0f);
      // the position of our light
      GLuint lightPosId = glGetUniformLocation(programId, "u_LightPos");
      // glUniform3f(lightPosId, 10, 8, -2);
      glUniform3fv(lightPosId, 1, &lightPosDir[0]);

      // the colour of our object
      glm::vec4 color = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f); 
      GLuint diffuseColourId = glGetUniformLocation(programId, "u_DiffuseColour");
      glUniform4fv(diffuseColourId, 1, &color[0]);

      // the shininess of the object's surface
      GLuint shininessId = glGetUniformLocation(programId, "u_Shininess");
      glUniform1f(shininessId, 3);

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

      // disable the attribute arrays
      glDisableVertexAttribArray(positionAttribId);
      glDisableVertexAttribArray(textureCoordsAttribId);
      glDisableVertexAttribArray(normalAttribId);
    }



    // draw the track
    {
      // activate our shader program
      glUseProgram(programId);

      // model matrix: translate, scale, and rotate the model
      model = glm::mat4(1.0f);
      // model = glm::mat4_cast(rotation);
      model = glm::translate(model, glm::vec3(0.0f, -10.0f, 0.0f));    
      model = glm::scale(model, glm::vec3(50.0f, 0.01f, 500.0f));

      // model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

      // model-view-projection matrix
      glm::mat4 mvp = projection * view * model;
      GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
      glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

      // model matrix
      GLuint mMatrixId = glGetUniformLocation(programId, "u_ModelMatrix");
      glUniformMatrix4fv(mMatrixId, 1, GL_FALSE, &model[0][0]);

      GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
      glActiveTexture(GL_TEXTURE0);  // texture unit 0
      glBindTexture(GL_TEXTURE_2D, allTextures[2]);
      glUniform1i(textureId, 0);

      // the colour of our object
      glm::vec4 color = glm::vec4(0.3f, 0.1f, 0.1f, 1.0f); 
      GLuint diffuseColourId = glGetUniformLocation(programId, "u_DiffuseColour");
      glUniform4fv(diffuseColourId, 1, &color[0]);

      // the shininess of the object's surface
      GLuint shininessId = glGetUniformLocation(programId, "u_Shininess");
      glUniform1f(shininessId, 2);

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

      // disable the attribute arrays
      glDisableVertexAttribArray(positionAttribId);
      glDisableVertexAttribArray(textureCoordsAttribId);
      glDisableVertexAttribArray(normalAttribId);
    }


    //christian render function start
      t += 1.0f/7175.0f;
      //std::cout << t << std::endl; 

      Person person(time,yRotationSpeed, yRotation, t, run);
      std::vector<glm::mat4> vec = person.getBodyVector();
	  if (person.isFinished() == true) {
		  glutDestroyWindow(windowId);
		  exit(0);
	  }
      count+=1;

      for(int i = 0; i < vec.size(); i++){
        drawCube(vec[i]); 
      }
      //christian render function end
    
    // make the draw buffer to display buffer (i.e. display what we have drawn)
    glutSwapBuffers();
}

static void reshape(int w, int h) {
   glViewport(0, 0, w, h);

   width = w;
   height = h;
}

static void drag(int x, int y) {
}

static void mouse(int button, int state, int x, int y) {
}

static void keyboard(unsigned char key, int x, int y) {
   // switch (key) {
   // case 'l':
   //    animateLight = !animateLight;
   //    break;
   // case 'r':
   //    rotateObject = !rotateObject;
   //    break;
   // case 's':
   //    animateShaders = !animateShaders;
   //    break;
   // case 'b':
   //    animateSkyboxes = !animateSkyboxes;
   //    break;
   // }
  //christian keyboard function
    if (key == 'r') {
      if (yRotationSpeed > 0.0) {
        yRotationSpeed = 0.0;
      }
      else {
        yRotationSpeed = 0.1;
      }
      std::cout << "Toggling rotation, speed: " << yRotationSpeed << std::endl;
    }
    else if(key == 'g'){
    	run+=2;


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
  glutInitWindowSize(width, height);
  glutCreateWindow("CSCI 4110U - Graphics Project: Brick Olympics");
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

  std::vector<std::string> filenames;
  filenames.push_back("textures/mp_sandcastle/sandcastle_rt.tga");
  filenames.push_back("textures/mp_sandcastle/sandcastle_lf.tga");
  filenames.push_back("textures/mp_sandcastle/sandcastle_up.tga");
  filenames.push_back("textures/mp_sandcastle/sandcastle_dn.tga");
  filenames.push_back("textures/mp_sandcastle/sandcastle_ft.tga");
  filenames.push_back("textures/mp_sandcastle/sandcastle_bk.tga");

  skyboxTexture = createCubemap(filenames);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  // create the scene geometry
  createSkybox();
  createGeometry();
  allTextures[0] = createTexture("textures/grass.jpg");
  allTextures[1] = createTexture("textures/checkered.jpg");
  allTextures[2] = createTexture("textures/track.jpg");

  // load the GLSL shader programs
  ShaderProgram program;
  program.loadShaders("shaders/my_vertex.glsl", "shaders/my_fragment.glsl");
  programId = program.getProgramId();

  ShaderProgram skyboxProgram;
  skyboxProgram.loadShaders("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl");
  skyboxProgramId = skyboxProgram.getProgramId();

  glutMainLoop();

  return 0;
}

static unsigned int createCubemap(std::vector<std::string> filenames) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, numChannels;
    for (unsigned int i = 0; i < filenames.size(); i++) {
        unsigned char *data = stbi_load(filenames[i].c_str(), &width, &height, &numChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cout << "Cubemap texture failed to load at path: " << filenames[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glActiveTexture(GL_TEXTURE0);

    return textureID;
}

static unsigned int createTexture(std::string filename) {
  int width, height, numChannels;
  unsigned char *bitmap = stbi_load(filename.c_str(), 
                                    &width, 
                                    &height, 
                                    &numChannels, 4);

  GLuint textureId;
  glGenTextures(1, &textureId);
  // make this texture active
  glBindTexture(GL_TEXTURE_2D, textureId);

  // specify the functions to use when shrinking/enlarging the texture image
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // send the data to OpenGL
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);

  glBindTexture(GL_TEXTURE_2D, 0);
  glActiveTexture(GL_TEXTURE0);

  // we don't need the bitmap data any longer
  //stbi_image_free(bitmap);

  return textureId;
}

void drawCube(glm::mat4 model){
  // model-view-projection matrix
  glm::mat4 mvp = projection * view * model;
  GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
  glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

  // texture sampler - a reference to the texture we've previously created
  GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
  glActiveTexture(GL_TEXTURE0);  // texture unit 0
  glBindTexture(GL_TEXTURE_2D, allTextures[1]);
  glUniform1i(textureId, 0);

  // the colour of our object
  glm::vec4 color = glm::vec4(0.8f, 0.2f, 0.2f, 1.0f); 
  GLuint diffuseColourId = glGetUniformLocation(programId, "u_DiffuseColour");
  glUniform4fv(diffuseColourId, 1, &color[0]);

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

  // disable the attribute arrays
  glDisableVertexAttribArray(positionAttribId);
  glDisableVertexAttribArray(textureCoordsAttribId);
  glDisableVertexAttribArray(normalAttribId); 
}