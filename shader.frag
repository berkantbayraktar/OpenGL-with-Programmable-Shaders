#version 410

// Output Color
out vec4 color;

uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;

// Texture-related data;
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;

// Data from Vertex Shader
in vec2 textureCoordinate;
in vec3 vertexNormal; // For Lighting computation
in vec3 ToLightVector; // Vector from Vertex to Light;
in vec3 ToCameraVector; // Vector from Vertex to Camera;


void main() {

  // Assignment Constants below
  // get the texture color
  vec3 textureColor = texture(rgbTexture, textureCoordinate).xyz;

  // apply Phong shading by using the following parameters
  vec4 ka = vec4(0.25,0.25,0.25,1.0); // reflectance coeff. for ambient
  vec4 Ia = vec4(0.3,0.3,0.3,1.0); // light color for ambient
  vec4 Id = vec4(1.0, 1.0, 1.0, 1.0); // light color for diffuse
  vec4 kd = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for diffuse
  vec4 Is = vec4(1.0, 1.0, 1.0, 1.0); // light color for specular
  vec4 ks = vec4(1.0, 1.0, 1.0, 1.0); // reflectance coeff. for specular
  int specExp = 100; // specular exponent

  // Ambient coefficient
  vec4 ambient = ka * Ia;

  // Specular coefficient
  vec3 h = normalize(ToLightVector + ToCameraVector); 
  float cos_alpha = clamp(dot(vertexNormal, h),0,1);
  vec4 specular = ks * pow(cos_alpha,specExp) * Is;
  
  // Diffuse coefficient
  float cos_theta = clamp(dot(vertexNormal, ToLightVector),0.0f,1.0f);
  vec4 diffuse = kd * cos_theta * Id;
  
  // Total coefficient multiplier of the texture color 
  vec3 color_coefficient = vec3(ambient + diffuse + specular);

  // compute the color using the following equation
  color = vec4(clamp( textureColor * color_coefficient, 0.0, 1.0), 1.0);

}