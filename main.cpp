#include "helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/rotate_vector.hpp>

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;


GLuint cameraPositionID;
GLuint MVPID;
GLuint MVID;
GLuint vertexbuffer;
GLuint widthTextureID;
GLuint heightTextureID;
GLuint TextureID;
GLuint heightFactorID;
GLuint NormalMatrixID;

glm::mat4 Projection;
glm::mat4 View;
glm::vec3 cameraPosition;
glm::mat4 Model = glm::mat4(1.0f);

GLfloat*  buffer;


int widthTexture, heightTexture;
float heightFactor = 10.0f;

void setCamera();
void drawTriangles();

static void errorCallback(int error,
  const char * description) {
  fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char * argv[]) {
  if (argc != 2) {
    printf("Only one texture image expected!\n");
    exit(-1);
  }

  glfwSetErrorCallback(errorCallback);

  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
  

  win = glfwCreateWindow(600, 600, "CENG477 - HW3", NULL, NULL);

  if (!win) {
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(win);
 
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

    glfwTerminate();
    exit(-1);
  }



  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);


  NormalMatrixID  = glGetUniformLocation(idProgramShader,"normalMatrix");
  MVPID = glGetUniformLocation(idProgramShader,"MVP");
  MVID = glGetUniformLocation(idProgramShader,"MV");
  cameraPositionID = glGetUniformLocation(idProgramShader,"cameraPosition");
  widthTextureID = glGetUniformLocation(idProgramShader,"widthTexture");
  heightTextureID = glGetUniformLocation(idProgramShader,"heightTexture");
  TextureID = glGetUniformLocation(idProgramShader,"rgbTexture");
  heightFactorID = glGetUniformLocation(idProgramShader,"heightFactor");

 
  glUniform1i(TextureID,0);
  glUniform1i(widthTextureID,widthTexture);
  glUniform1i(heightTextureID,heightTexture);
  glUniform1f(heightFactorID,heightFactor);

  buffer = new GLfloat[12*widthTexture*heightTexture];
  drawTriangles();
  setCamera();

  
		

  // MAIN LOOP
  while (!glfwWindowShouldClose(win)) {

    glViewport(0,0,600,600);
        

    // White background
	  glClearColor(0,0,0,1);
    glClearDepth(1.0f);
    glClearStencil(0);
    // Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    setCamera();

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT,0,buffer);
		// Draw the triangle  
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * widthTexture * heightTexture); // 3 indices starting at 0 -> 1 triangle
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glfwSwapBuffers(win);
    glfwPollEvents();

  }
  
  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}

void setCamera()
{ 

  Projection  = glm::perspective(45.0f
  , 1.0f , 0.1f, 1000.0f);

  cameraPosition = glm::vec3(widthTexture / 2, widthTexture / 10, (-1) * (widthTexture / 4));
  
  View = glm::lookAt(
    cameraPosition, // Camera position
    glm::vec3(cameraPosition + glm::vec3(0,0,1) * 0.1f), // and looks to the z direction
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
  ); 

  glm::mat4 normalMatrix = glm::inverseTranspose(View);
  
  glm::mat4 mvp = Projection * View * Model;
  
  glm::mat4 mv = View * Model;
  
  
  glUniformMatrix4fv(MVPID , 1 , GL_FALSE , &mvp[0][0]);
  glUniformMatrix4fv(MVID , 1 , GL_FALSE , &mv[0][0]);
  glUniform3fv(cameraPositionID,1,&cameraPosition[0]);
  glUniformMatrix4fv(NormalMatrixID,1,GL_FALSE,&normalMatrix[0][0]);

  
}

// Triangles clock-wise direction
void drawTriangles()
{ 
  
  for(int i = 0  ; i < widthTexture ; i++)
  { 
    for(int j = 0 ; j < heightTexture; j++)
    {  
      
      buffer[i * heightTexture * 12 + j * 12 ] = i;
      buffer[i * heightTexture * 12 + j * 12 + 1] = 0;
      buffer[i * heightTexture * 12 + j * 12 + 2] = j+1;

      buffer[i * heightTexture * 12 + j * 12 + 3 ] = i;
      buffer[i * heightTexture * 12 + j * 12 + 4] = 0;
      buffer[i * heightTexture * 12 + j * 12 + 5] = j;

      buffer[i * heightTexture * 12 + j * 12 + 6] = i+1;
      buffer[i * heightTexture * 12 + j * 12 + 7] = 0;
      buffer[i * heightTexture * 12 + j * 12 + 8] = j+1;

      buffer[i * heightTexture * 12 + j * 12 + 9] = i+1;
      buffer[i * heightTexture * 12 + j * 12 + 10] = 0;
      buffer[i * heightTexture * 12 + j * 12 + 11] = j;
        
    }
  }

  GLuint vertexBuffer;

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12 * widthTexture * heightTexture
  , buffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
  glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
	);
		
}