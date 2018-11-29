//imports and includes
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

  //Hurdle class
  #include "Hurdle.h"

//end of includes

//variables
  GLuint skyboxTexture;
  GLuint allTextures[4];
  GLuint numVertices;
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

  //initial view/camera/projection
  glm::vec3 eyePosition = glm::vec3(0.0f, 50.0f, 150.0f);
  glm::vec3 centerPoint = glm::vec3(0.0f, 0.0f, 0.0f);
  glm::mat4 view;
  glm::mat4 projection;
  float aspectRatio;

  //other variables
  int width = 1024;
  int height = 768;
  bool isFinished = false;

  //BOID Variables
  const int BOIDS_COUNT = 300;
  float deltaTime = 0.0f;
  float prevTime = 0.0f;

  //Christian Variables/Functions
  //drawCube and animation
  float yRotation = -85.0f;
  float yRotationSpeed = 0.1f;
  float time = 0.0f;
  int count = 1;
  float t;
  int windowId = 0;

  //Calculating run and jump variables
  float run1 = 1.00f;
  float run2 = 1.00f;
  double HurdleDist1 = -60.0f;
  double HurdleDist2 = -120.0f;
  bool hit1 = false;
  bool hit2 = false;
  double jumpTime1;
  double jumpTime2;

//functions
  //Forward Declare Functions
  void drawCube(glm::mat4 model, glm::vec4 color);
  void getMiddleCamera(float point1, float point2);

  // static void createTexture(std::string filename);
  static GLuint createTexture(std::string filename);
  static GLuint createCubemap(std::vector<std::string> filenames);
  static void drawShip(glm::vec3 position, glm::vec3 direction, glm::vec4 color);

class Boid {
  public:
    glm::vec3 position;
    glm::vec3 velocity;

    float seperationFacotor = 0.4f;
    float cohesionFactor = 0.3f;
    float alignmentFactor = 0.3f;

    Boid(glm::vec3 a_position): position(a_position){
      velocity = glm::normalize(a_position)*2.0f;
    } 

    void CapVelocity(float maxSpeed){
      if(glm::length(velocity) > maxSpeed){
        velocity = glm::normalize(velocity)*maxSpeed;
      }
    }

    void ApplyCohesion(glm::vec3 cohesionVector){
      velocity += cohesionVector * cohesionFactor;
    }
    void ApplySeperation(glm::vec3 seperationVector){
      velocity += seperationFacotor*seperationVector;
    }
    void ApplyAlignment(glm::vec3 alignmentVector){
      velocity += alignmentVector*alignmentFactor;
    }
};

class BoidManager {
  public:
    BoidManager(int a_numBoids){
      for (int i = 0; i < a_numBoids; i++){
        glm::vec3 startingPosition = glm::vec3(sin((float)i)*10.0f, cos((float)i)*10.0f, 10.0f);
        // glm::vec3 startingPosition = glm::vec3(0.0f, 9.0f, 9.0f);

        Boid boid =  Boid(startingPosition);
        boids.push_back(boid);
      }
    }
    glm::vec3 GetGroupAlignment(glm::vec3 centerPosition, float radius){
      glm::vec3 averageAlignment = glm::vec3(0.0f);
      float count = 0.0f;

      for(int i = 0; i < boids.size(); i++){
        if (glm::length(boids[i].position - centerPosition) < radius){
          glm::vec3 direction = glm::normalize(boids[i].velocity);
          averageAlignment += direction;
          count += 1.0f;
        }
      }
      if (count == 0.0f) return glm::vec3(0.0f);
      return averageAlignment/count;
    }

    glm::vec3 GetGroupPosition(glm::vec3 centerPosition, float radius){
      glm::vec3 averagePosition = glm::vec3(0.0f);
      float count = 0.0f;

      for(int i = 0; i < boids.size(); i++){
        if (glm::length(boids[i].position - centerPosition) < radius){
          averagePosition += boids[i].position;
          count += 1.0f;
        }
      }
      if (count == 0.0f) return glm::vec3(0.0f);
      return averagePosition/count;
    }

    void UpdateBoids(float dt){
      for(int i =0; i < boids.size(); i++){
        boids[i].CapVelocity(5.0f);

        glm::vec3 alignmentVector = GetGroupAlignment(boids[i].position, 1.0f);
        glm::vec3 seperationVector = boids[i].position - GetGroupPosition(boids[i].position, 1.0f);
        glm::vec3 cohesionVector = GetGroupPosition(boids[i].position, 1.0f) - boids[i].position;

        boids[i].ApplySeperation(seperationVector);
        boids[i].ApplyCohesion(cohesionVector);
        boids[i].ApplyAlignment(alignmentVector);

        boids[i].position += boids[i].velocity*dt;

        float upperBound = 80.0f;
        float lowerBound = -80.0f;

        float upperYBound = 80.0f;
        float lowerYBound = -12.0f;

        if (boids[i].position.x > upperBound){
          boids[i].position.x = lowerBound;
        }
        if (boids[i].position.x < lowerBound){
          boids[i].position.x = upperBound;
        }
        if (boids[i].position.y > upperYBound){
          boids[i].position.y = lowerYBound;
        }
        if (boids[i].position.y < lowerYBound){
          boids[i].position.y = upperYBound;
        }
        if (boids[i].position.z > upperBound){
          boids[i].position.z = lowerBound;
        }
        if (boids[i].position.z < lowerBound){
          boids[i].position.z = upperBound;
        }
      }
    }
    std::vector<Boid> boids;
};

BoidManager* manager;

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
  
  time = ((float)milliseconds / 500.0f) * 5;

  float boidtime = (float)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
  deltaTime = boidtime - prevTime;
  prevTime = boidtime;
  manager->UpdateBoids(deltaTime);
    

  glutPostRedisplay();
}

glm::mat4 model;

static void render(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // view and perspective
  // view matrix - orient everything around our preferred view
  view = glm::lookAt( eyePosition,
                      centerPoint,    // where to look
                      glm::vec3(0,1,0)     // up
                    );
  // view = glm::rotate(view, glm::radians(90.0f), glm::vec3(0, 1, 0));
  // view = glm::rotate(view, glm::radians(yRotation), glm::vec3(0, 1, 0));
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
    model = glm::translate(model, glm::vec3(0.0f, -12.0f, 0.0f));    
    model = glm::scale(model, glm::vec3(1000.0f, 0.01f, 1000.0f));

    // model-view-projection matrix
    glm::mat4 mvp = projection * view * model;
    GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
    glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

    // texture
    GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
    glActiveTexture(GL_TEXTURE0);  // texture unit 0
    glBindTexture(GL_TEXTURE_2D, allTextures[0]);
    glUniform1i(textureId, 0);

    // the position of our camera/eye
    GLuint eyePosId = glGetUniformLocation(programId, "u_EyePosition");
    glUniform3f(eyePosId, eyePosition.x, eyePosition.y, eyePosition.z);
    
    //light position
    glm::vec3 lightPos = glm::vec3(0.0f, 100.0f, 100.0f);
    // the position of our light
    GLuint lightPosId = glGetUniformLocation(programId, "u_LightPos");
    // glUniform3f(lightPosId, 10, 8, -2);
    glUniform3fv(lightPosId, 1, &lightPos[0]);

    // the colour of our object
    glm::vec4 color = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f); 
    GLuint diffuseColourId = glGetUniformLocation(programId, "u_DiffuseColour");
    glUniform4fv(diffuseColourId, 1, &color[0]);

    // the shininess of the object's surface
    GLuint shininessId = glGetUniformLocation(programId, "u_Shininess");
    glUniform1f(shininessId, 1);

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
    model = glm::translate(model, glm::vec3(0.0f, -10.0f, -190.0f));
    model = glm::scale(model, glm::vec3(50.0f, 0.01f, 600.0f));

    // model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

    // model-view-projection matrix
    glm::mat4 mvp = projection * view * model;
    GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
    glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

    // texture
    GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
    glActiveTexture(GL_TEXTURE1);  // texture unit 0
    glBindTexture(GL_TEXTURE_2D, allTextures[2]);
    glUniform1i(textureId, 1);

    // the colour of our object
    glm::vec4 color = glm::vec4(0.3f, 0.1f, 0.1f, 1.0f); 
    GLuint diffuseColourId = glGetUniformLocation(programId, "u_DiffuseColour");
    glUniform4fv(diffuseColourId, 1, &color[0]);

    // the shininess of the object's surface
    GLuint shininessId = glGetUniformLocation(programId, "u_Shininess");
    glUniform1f(shininessId, 1);

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

  // draw the finishline
  {
    // activate our shader program
    glUseProgram(programId);

    // model matrix: translate, scale, and rotate the model
    model = glm::mat4(1.0f);
    // model = glm::mat4_cast(rotation);
    model = glm::translate(model, glm::vec3(0.0f, -9.5f, -200.0f));
    model = glm::scale(model, glm::vec3(50.0f, 0.01f, 10.0f));

    // model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

    // model-view-projection matrix
    glm::mat4 mvp = projection * view * model;
    GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
    glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

    // texture
    GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
    glActiveTexture(GL_TEXTURE1);  // texture unit 0
    glBindTexture(GL_TEXTURE_2D, allTextures[2]);
    glUniform1i(textureId, 1);

    // the colour of our object
    glm::vec4 color = glm::vec4(0.8f, 1.0f, 0.0f, 1.0f); 
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

  //CHARACTER & HURDLE RENDER INFORMATION
	{
		t += 1.0f / 7175.0f;

		//PERSON 1 Information
		{
			//checks if jump is initiated
			if (jumpTime1 >= 0.005 && jumpTime1 <= 5.000) {
				jumpTime1 += 0.005;		//increment our jumptime by a small value to display a more smother jump per each update we make
			}

			//check if player 1 hit the hurdle..
			double runDistance1 = (time / 5.00) + run1;
			hit1 = false;
			std::cout << "runTime1: " << runDistance1 << std::endl;
			if (runDistance1  >=57.0 && runDistance1 <=63.0 || runDistance1 >= 117.0 && runDistance1 <= 123.0) {
				//check if the height at this hurdle is greater than 12.0f( the height of our hurdle)
				if ((jumpTime1 <= 1.5 && jumpTime1 >= 0.0) && (runDistance1 >= 57.0 && runDistance1 <= 63.0 || runDistance1 >= 117.0 && runDistance1 <= 123.0)) {	//cleared the hurdle
					//slow our player down by not allowing them to press run button
					hit1 = true;
				}
			}

			Person person1(time, yRotationSpeed, yRotation, t, (float)runDistance1, 8.00, jumpTime1);
			std::vector<glm::mat4> vec1 = person1.getBodyVector();
			if (person1.isFinished() == true) {
				isFinished = true;
        {
          // activate our shader program
          glUseProgram(programId);

          // turn on depth buffering
          glEnable(GL_DEPTH_TEST);

          // draw the ship
          for (int i =0;  i < manager->boids.size(); i++){
          Boid b = manager->boids[i];
          glm::vec4 color = glm::vec4(0.0f,0.0f,0.8f,1.0f);
          drawShip(b.position, glm::normalize(b.velocity), color);
          }

          // disable the attribute array
          // glDisableVertexAttribArray(positionBufferId);
        }
				// std::cout << "BLUE Player 1 wins!" << std::endl;
				// glutDestroyWindow(windowId);
				// exit(0);
			}
			for (int i = 0; i < vec1.size(); i++) {
				drawCube(vec1[i], glm::vec4(0.0f, 0.0f, 0.8f, 1.0f));
			}

			if (jumpTime1 >= 5.000) {
				jumpTime1 = 0.000;
			}
		}

		//PERSON 2 Information
		{
			//checks if jump is initiated ~ player 2
			if (jumpTime2 >= 0.005 && jumpTime2 <= 5.000) {
				jumpTime2 += 0.005;		//increment our jumptime by a small value to display a more smother jump per each update we make
			}

			//check if player 2 hit the hurdle..
			double runDistance2 = (time / 5.00) + run2;
			hit2 = false;
			std::cout << "runTime1: " << runDistance2 << std::endl;
			if (runDistance2 >= 57.0 && runDistance2 <= 63.0 || runDistance2 >= 117.0 && runDistance2 <= 123.0) {
				//check if the height at this hurdle is greater than 12.0f( the height of our hurdle)
				if ((jumpTime2 <= 1.5 && jumpTime2 >= 0.0) && (runDistance2 >= 57.0 && runDistance2 <= 63.0 || runDistance2 >= 117.0 && runDistance2 <= 123.0)) {	//cleared the hurdle
					//slow our player down by not allowing them to press run button
					hit2 = true;
				}
			}

			Person person2(time, yRotationSpeed, yRotation, t, (float)runDistance2, -8.00, jumpTime2);
			std::vector<glm::mat4> vec2 = person2.getBodyVector();
			if (person2.isFinished() == true) {
				isFinished = true;
        {
          // activate our shader program
          glUseProgram(programId);

          // turn on depth buffering
          glEnable(GL_DEPTH_TEST);

          // draw the ship
          for (int i =0;  i < manager->boids.size(); i++){
          Boid b = manager->boids[i];
          glm::vec4 color = glm::vec4(0.8f,0.0f,0.0f,1.0f);
          drawShip(b.position, glm::normalize(b.velocity), color);
          }

          // disable the attribute array
          // glDisableVertexAttribArray(positionBufferId);
        }
				// std::cout << "RED Player 2 wins!" << std::endl;
				// glutDestroyWindow(windowId);
				// exit(0);
			}
			for (int i = 0; i < vec2.size(); i++) {
				drawCube(vec2[i], glm::vec4(0.8f, 0.0f, 0.0f, 1.0f));
			}

			if (jumpTime2 >= 5.000) {
				jumpTime2 = 0.000;
			}
		}

		//christian render function end
		getMiddleCamera((time / 5.00) + run1, (time / 5.00) + run2);

		//PLAYER 1 HURDLES
		{
			//hurdle1
			Hurdle p1hurdle1(10.00, HurdleDist1);
			std::vector<glm::mat4> p1hurd1 = p1hurdle1.getHurdleVector();
			for (int i = 0; i < p1hurd1.size(); i++) {
				drawCube(p1hurd1[i], glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
			}

			//hurdle2
			Hurdle p1hurdle2(10.00, HurdleDist2);
			std::vector<glm::mat4> p1hurd2 = p1hurdle2.getHurdleVector();
			for (int i = 0; i < p1hurd2.size(); i++) {
				drawCube(p1hurd2[i], glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
			}

		}

		//PLAYER 2 HURDLES
		{
			//hurdle1
			Hurdle p2hurdle1(-10.00, HurdleDist1);
			std::vector<glm::mat4> p2hurd1 = p2hurdle1.getHurdleVector();
			for (int i = 0; i < p2hurd1.size(); i++) {
				drawCube(p2hurd1[i], glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
			}

			//hurdle2
			Hurdle p2hurdle2(-10.00, HurdleDist2);
			std::vector<glm::mat4> p2hurd2 = p2hurdle2.getHurdleVector();
			for (int i = 0; i < p2hurd2.size(); i++) {
				drawCube(p2hurd2[i], glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
			}

		}

	}
  
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

	if (key == 27) {
		glutDestroyWindow(windowId);
		exit(0);
	}
	if (!isFinished) {
		//Player 1 functionality
		if (key == 'p') {	//jump
			if (jumpTime1 == 0.000) {
				jumpTime1 = 0.005;
			}
		}
		if (key == 'o' && hit1 == false) {	//run1
			run1 += 1.00f;
			std::cout << "run1: " << run1 << std::endl;
		}
		//Player 2 functionality
		if (key == 'w') {	//jump
			if (jumpTime2 == 0.000) {
				jumpTime2 = 0.005;
			}
		}
		if (key == 'q' && hit2 == false) {		//run2
			run2 += 1.00f;
			//std::cout << "Player 2 steps: " << run2 / 2 << std::endl;
		}
		
	}
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
  allTextures[1] = createTexture("textures/brickwall.jpg");
  allTextures[2] = createTexture("textures/track.jpg");
  //texture for testing
  allTextures[3] = createTexture("textures/checkered.jpg");

  //boids
  manager = new BoidManager(BOIDS_COUNT);

  //load the GLSL shader programs
  ShaderProgram program;
  program.loadShaders("shaders/my_vertex.glsl", "shaders/my_fragment.glsl");
  programId = program.getProgramId();

  ShaderProgram skyboxProgram;
  skyboxProgram.loadShaders("shaders/skybox_vertex.glsl", "shaders/skybox_fragment.glsl");
  skyboxProgramId = skyboxProgram.getProgramId();

  glutMainLoop();

  return 0;
}

static GLuint createCubemap(std::vector<std::string> filenames) {
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

static GLuint createTexture(std::string filename) {
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
  stbi_image_free(bitmap);

  return textureId;
}

void drawCube(glm::mat4 model, glm::vec4 color){
  // model-view-projection matrix
  glm::mat4 mvp = projection * view * model;
  GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
  glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

  // texture sampler - a reference to the texture we've previously created
  GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
  glActiveTexture(GL_TEXTURE2);  // texture unit 0
  glBindTexture(GL_TEXTURE_2D, allTextures[1]);
  glUniform1i(textureId, 2);

  // the colour of our object
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

void getMiddleCamera(float point1, float point2) {

	float middlePoint = (point1 + point2) / 2.00f;
	//std::cout << middlePoint << std::endl;
	eyePosition = glm::vec3(0.0f, 50.0f, 150.0f - middlePoint);	//move closer to our center point
	centerPoint = glm::vec3(0.0f, 0.0f, 0 - middlePoint);	//move our center point for our characters

}

static void drawShip(glm::vec3 position, glm::vec3 direction, glm::vec4 color) {

  // model matrix: translate, scale, and rotate the model
  float angle = glm::atan(direction.y, direction.x);

  model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::translate(model, glm::vec3(0.0f, 10.0f, -100.0f));
  // model = glm::rotate(model, angle+glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // model-view-projection matrix
  glm::mat4 mvp = projection * view * model;
  GLuint mvpMatrixId = glGetUniformLocation(programId, "u_MVPMatrix");
  glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &mvp[0][0]);

  // texture sampler - a reference to the texture we've previously created
  GLuint textureId  = glGetUniformLocation(programId, "u_TextureSampler");
  glActiveTexture(GL_TEXTURE2);  // texture unit 0
  glBindTexture(GL_TEXTURE_2D, allTextures[1]);
  glUniform1i(textureId, 2);

  // the colour of our object
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