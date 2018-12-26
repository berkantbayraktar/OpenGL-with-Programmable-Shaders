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


GLuint cam_pos_location;
GLuint mvp_location;
GLuint width_texture_location;
GLuint height_texture_location;
GLuint texture_location;
GLuint height_factor_location;
GLuint normal_location;

glm::mat4 normal_matrix;
glm::vec3 camera_position;
glm::vec3 camera_gaze;
glm::mat4 view_matrix;
glm::mat4 mvp;
glm::mat4 mv;
glm::mat4 model_matrix;
glm::mat4 projection_matrix;

GLfloat height_factor;
GLfloat camera_speed;

GLfloat*  buffer;

bool isFullScreen = false;
int window_width = 600;
int window_height = 600;
int window_xpos = 0;
int window_ypos = 0;

int widthTexture, heightTexture;

void init();
void drawTriangles();
void render();
void setCamera();
void key_callback(GLFWwindow* window, int key , int scancode , int action , int mods);
void window_size_callback(GLFWwindow* window, int width, int height);
void full_screen();


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
  

  win = glfwCreateWindow(window_width, window_height, "CENG477 - HW3", NULL, NULL);

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

  glfwSetKeyCallback(win,key_callback);
  glfwSetWindowSizeCallback(win, window_size_callback);


  initShaders();
  glUseProgram(idProgramShader);
  initTexture(argv[1], & widthTexture, & heightTexture);

  // initialize program
  init();



  // MAIN LOOP
  while (!glfwWindowShouldClose(win)) {

    render();
    
    
    glfwSwapBuffers(win);
    glfwPollEvents();

  }
  
  glfwDestroyWindow(win);
  glfwTerminate();

  return 0;
}

void init()
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // height factor 10 initially
  height_factor = 10.0f;
  // camera speed 0 initially
  camera_speed = 0.0f;
  // camera position
  camera_position = glm::vec3(widthTexture / 2, widthTexture / 10, (-1) * (widthTexture / 4));
  //camera gaze
  camera_gaze = glm::vec3(0,0,1);
  // model matrix identity matrix initially
  model_matrix = glm::mat4(1);
  // perspective projection
  projection_matrix  = glm::perspective(45.0f, 1.0f , 0.1f, 1000.0f);

  // 12 * w * h vertex count
  buffer = new GLfloat[12*widthTexture*heightTexture];

  // insert triangles into buffer
  drawTriangles();


  // get variable locations from the shaders
  normal_location = glGetUniformLocation(idProgramShader,"normalMatrix");
  mvp_location = glGetUniformLocation(idProgramShader,"MVP");
  idMVPMatrix = glGetUniformLocation(idProgramShader,"MV");
  cam_pos_location = glGetUniformLocation(idProgramShader,"cameraPosition");
  width_texture_location = glGetUniformLocation(idProgramShader,"widthTexture");
  height_texture_location = glGetUniformLocation(idProgramShader,"heightTexture");
  texture_location = glGetUniformLocation(idProgramShader,"rgbTexture");
  height_factor_location = glGetUniformLocation(idProgramShader,"heightFactor");

  // Insert permanent ones
  glUniform1i(texture_location,0);
  glUniform1i(width_texture_location,widthTexture);
  glUniform1i(height_texture_location,heightTexture);

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

  GLuint vertexbuffer;

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
  delete[] buffer;
		
}

void render()
{ 
        
  //Black background
	glClearColor(0,0,0,1);
  glClearDepth(1.0f);
  glClearStencil(0);

  // Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  setCamera();

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4 * widthTexture * heightTexture); // 3 indices starting at 0 -> 1 triangle
  
  
  glUniform1f(height_factor_location,height_factor);
}

void setCamera()
{ 
  
  view_matrix = glm::lookAt(
    camera_position, // Camera position
    glm::vec3(camera_position + camera_gaze), // and looks to the z direction
    glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
  ); 

  normal_matrix = glm::inverseTranspose(view_matrix);
  
  mvp = projection_matrix * view_matrix * model_matrix;
  
  mv = view_matrix * model_matrix;
  
  
  glUniformMatrix4fv(mvp_location , 1 , GL_FALSE , &mvp[0][0]);
  glUniformMatrix4fv(idMVPMatrix , 1 , GL_FALSE , &mv[0][0]);
  glUniform3fv(cam_pos_location,1,&camera_position[0]);
  glUniformMatrix4fv(normal_location,1,GL_FALSE,&normal_matrix[0][0]);
}


void key_callback(GLFWwindow* window, int key , int scancode , int action , int mods)
{ 
  // If any key is not pressed or released
  if(action != GLFW_RELEASE && action != GLFW_PRESS)
  {
    return;
  }

  // If the key is "pressed"
  if(action == GLFW_PRESS)
  {
    switch(key)
    { 
      // If hit escape close the program
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(win , GLFW_TRUE);
      // O -> increase height factor
      case GLFW_KEY_O:
        height_factor += 0.5f;
        break;
      // L -> decrease height factor
      case GLFW_KEY_L:
        height_factor -= 0.5f;
        break;
      // F -> toggle full screen
      case GLFW_KEY_F:
        full_screen();
        break;
      default:
        break;
    }
  }
  
}

void window_size_callback(GLFWwindow* window, int width, int height)
{ 
  int w,h;
  glfwGetFramebufferSize(win , &w , &h);
  glViewport(0 , 0 , w, h);
}

void full_screen()
{ 
  isFullScreen = !isFullScreen;
  if(isFullScreen)
  { 
    // Store old width and height
    glfwGetWindowSize(win , &window_width , &window_height);
    // Store old window position
    glfwGetWindowPos(win , &window_xpos , & window_ypos);
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(win , monitor, 0 , 0,
    mode -> width , mode -> height , mode -> refreshRate);
  }
  else
  {
    glfwSetWindowMonitor(win , NULL , window_xpos , window_ypos,
    window_width, window_height, 0);
  }
  
 
}


