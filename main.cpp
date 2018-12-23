#include "helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

glm::mat4 Projection;
glm::mat4 View;
glm::vec4 camPos;
glm::mat4 Model = glm::mat4(1.0f);


int widthTexture, heightTexture;
float heightFactor = 10.0f;

void setCamera();
void drawTriangles(GLfloat buffer[]);

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



	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

  glEnable(GL_DEPTH_TEST);
  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);

  MVPID = glGetUniformLocation(idProgramShader,"MVP");
  MVID = glGetUniformLocation(idProgramShader,"MV");
  cameraPositionID = glGetUniformLocation(idProgramShader,"cameraPosition");
  widthTextureID = glGetUniformLocation(idProgramShader,"widthTexture");
  heightTextureID = glGetUniformLocation(idProgramShader,"heightTexture");
  TextureID = glGetUniformLocation(idProgramShader,"rgbTexture");
  heightFactorID = glGetUniformLocation(idProgramShader,"heightFactor");

  widthTexture = 400;
  heightTexture = 400;
  GLfloat g_vertex_buffer_data[12*widthTexture*heightTexture] = {};

  glUniform1i(TextureID,0);
  glUniform1i(widthTextureID,widthTexture);
  glUniform1i(widthTextureID,heightTexture);
  glUniform1f(heightFactorID,heightFactor);

  drawTriangles(g_vertex_buffer_data);
  setCamera();
	
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  
		

  // MAIN LOOP
  while (!glfwWindowShouldClose(win)) {

    // White background
	  glClearColor(1,1,1,1);
    glClearDepth(1.0f);
    glClearStencil(0);
    
    // Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glUseProgram(idProgramShader);
    glViewport(0, 0, 600, 600);
    setCamera();
    // 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		

		// Draw the triangle ! 
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * widthTexture *heightTexture); // 3 indices starting at 0 -> 1 triangle

		glDisableVertexAttribArray(0);
    glfwSwapBuffers(win);
    glfwPollEvents();

  }
  
  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}

void setCamera()
{ 

  camPos = {0.5 *2 -1 , 0.1*2 -1 , -0.25*2 -1, 1};
  glUniform4f(cameraPositionID,camPos.x,camPos.y,camPos.z,camPos.w);


  Projection  = glm::perspective(glm::radians(45.0f)
  , 1.0f , 0.1f, 100.0f);
  
  View = glm::lookAt(
    glm::vec3(camPos.x,camPos.y,camPos.z /*widthTexture / 2, widthTexture / 10, -widthTexture / 4*/ ), // Camera position
    glm::vec3(0,0,1), // and looks to the z direction
    glm::vec3(0,-1,0)  // Head is up (set to 0,-1,0 to look upside-down)
  ); 

  glm::mat4 mvp = Projection * View * Model;
  
  glUniformMatrix4fv(MVPID , 1 , GL_FALSE , &mvp[0][0]);

  glm::mat4 mv = View * Model;
  
  glUniformMatrix4fv(MVID , 1 , GL_FALSE , &mv[0][0]);

  
  
}

// Triangles clock-wise direction
void drawTriangles(GLfloat buffer[])
{ 
 
  float x_coord = 0;
  float y_coord = 0;
  for(int i = 0  ; i < widthTexture ; i++)
  { 
    x_coord = i * 2.0 / widthTexture;
    for(int j = 0 ; j < heightTexture; j++)
    {   
        y_coord = j * 2.0 /heightTexture;


        buffer[i * heightTexture * 12 + j *12 ] = x_coord - 1;
        buffer[i * heightTexture * 12 + j * 12 + 1] = y_coord -1 + 2.0/heightTexture;
        buffer[i * heightTexture * 12 + j * 12 + 2] = 0;


        buffer[i * heightTexture * 12 + j * 12 + 3] = x_coord - 1;
        buffer[i * heightTexture * 12 + j * 12 + 4] = y_coord - 1;
        buffer[i * heightTexture * 12 + j * 12 + 5] = 0;

        
        
        buffer[i * heightTexture * 12 + j * 12 + 6] = x_coord - 1 + 2.0/ widthTexture;
        buffer[i * heightTexture * 12 + j * 12 + 7] =  y_coord -1 + 2.0/heightTexture;
        buffer[i * heightTexture * 12 + j * 12 + 8] = 0;
        
      
        
        buffer[i * heightTexture * 12 + j * 12 + 9] = x_coord - 1 + 2.0/ widthTexture;
        buffer[i * heightTexture * 12 + j * 12 + 10] = y_coord-1;
        buffer[i * heightTexture * 12 + j * 12 + 11] = 0;

        
    }
  }
}