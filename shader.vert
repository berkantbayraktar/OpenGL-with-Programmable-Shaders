#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform vec4 cameraPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;


void main()
{   
    // We are not sure ,may not work
    textureCoordinate = (position.xy + vec2(1,1) ) / 2.0;

    // texture or texture2D
    float z  = 0.2126 * texture2D(rgbTexture,textureCoordinate).r
                + 0.7152 * texture2D(rgbTexture,textureCoordinate).g
                + 0.0722 * texture2D(rgbTexture,textureCoordinate).b;
   
    // get texture value, compute height
    
    
    // compute normal vector using also the heights of neighbor vertices

    // compute toLight vector vertex coordinate in VCS
   
   // set gl_Position variable correctly to give the transformed vertex position

   gl_Position = MVP * vec4(position.x, /*heightFactor */ position.y ,1,1); // this is a placeholder. It does not correctly set the position 
    
}
