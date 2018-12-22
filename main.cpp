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

GLuint MVPID;


int widthTexture, heightTexture;

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

  // Dark blue background
	glClearColor(1,1,1,1);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);

  static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  // MAIN LOOP
  while (!glfwWindowShouldClose(win)) {

    // Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );

		// Use our shader
		glUseProgram(idProgramShader);

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
		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

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
  glm::mat4 Projection = glm::perspective(glm::radians(45.0f)
  , 1.0f , 0.1f, 100.0f);
  
  glm::mat4 View = glm::lookAt(
    glm::vec3(widthTexture / 2, widthTexture / 10, -widthTexture / 4 ), // Camera is at (4,3,3), in World Space
    glm::vec3(0,0,1), // and looks at the origin
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
  );

  glm::mat4 Model = glm::mat4(1.0f);

  glm::mat4 mvp = Projection * View * Model;

  GLuint MVPID = glGetUniformLocation(idProgramShader,"MVP");

  glUniformMatrix4fv(MVPID , 1 , GL_FALSE , &mvp[0][0]);

}

// Triangles clock-wise direction
void drawTriangles()
{
  glColor3f(0,1,0);
  glBegin(GL_TRIANGLES);
  for(int i = 0  ; i < widthTexture ; i++)
  {
    for(int j = 0 ; j < heightTexture; j++)
    { 
      // First triangle
      glVertex3f(i , 0 , j);
      glVertex3f(i + 1 , 0 , j);
      glVertex3f(i , 0 , j + 1);
      
      // Second triangle
      glVertex3f(i + 1 , 0 , j + 1);
      glVertex3f(i + 1 , 0 , j);
      glVertex3f(i , 0 , j + 1);
    }
  }
  glEnd();
}