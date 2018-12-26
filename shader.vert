#version 410

layout(location = 0) in vec3 position;

// Data from CPU 
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform mat4 normalMatrix;
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

vec3 light_pos = vec3(widthTexture/2, widthTexture + heightTexture, heightTexture/2);
vec3 limunance_coeff = vec3(.2126, .7152, .0722);


bool isAvailable(vec3 pos) {
  return pos.x >= 0  && pos.x < widthTexture  && pos.z >= 0 && pos.z < heightTexture;
}


void main()
{
    vec3 neighbors[6],  v[6];
    int neighbor_count = 0;
    vec3 normal_v = vec3(.0,.0,.0);

    vec3 current_position = position;
    textureCoordinate = vec2(1 - (float(position.x) / (widthTexture+1)), 1 - (float(position.z) / (heightTexture+1)));
    vec4 textureColor = texture(rgbTexture, textureCoordinate);
    current_position.y = heightFactor* dot(limunance_coeff, textureColor.xyz);
     
    if(isAvailable(vec3(position.x - 1, 0 , position.z))){
        neighbors[neighbor_count] = vec3(position.x - 1, 0 , position.z);
        neighbor_count++;
    }
    if (isAvailable(vec3(position.x - 1 , 0 , position.z + 1))){
        neighbors[neighbor_count] = vec3(position.x - 1 , 0 , position.z + 1);
        neighbor_count++;
    }
    if (isAvailable(vec3(position.x - 1 , 0 , position.z - 1))){
        neighbors[neighbor_count] = vec3(position.x - 1 , 0 , position.z - 1);
        neighbor_count++;
    }

    if(isAvailable(vec3(position.x , 0 , position.z + 1))){
        neighbors[neighbor_count] = vec3(position.x , 0 , position.z + 1);
        neighbor_count++;
    }
    if(isAvailable(vec3(position.x , 0 , position.z - 1))){
        neighbors[neighbor_count] = vec3(position.x , 0 , position.z - 1);
        neighbor_count++;
    }

    if(isAvailable(vec3(position.x + 1 , 0 , position.z))){
        neighbors[neighbor_count] = vec3(position.x + 1 , 0 , position.z);
        neighbor_count++;
    }
    if(isAvailable(vec3(position.x + 1, 0 , position.z - 1))){
        neighbors[neighbor_count] = vec3(position.x + 1, 0 , position.z - 1);
        neighbor_count++;
    }
    if(isAvailable(vec3(position.x + 1, 0 , position.z + 1))){
        neighbors[neighbor_count] = vec3(position.x + 1, 0 , position.z + 1);
        neighbor_count++;
    }
    

    for(int i = 0 ; i < neighbor_count; i++ ){
 
        vec3 texture_RGB = texture(rgbTexture, vec2((neighbors[i].x-widthTexture)/widthTexture,(neighbors[i].z-heightTexture)/heightTexture)).xyz;
        neighbors[i].y = heightFactor* dot(texture_RGB,limunance_coeff);
    }
    for(int i = 0 ; i < neighbor_count; i++ ){
        
        v[i] = neighbors[i] - current_position;
    }
    for(int i = 0 ; i < neighbor_count; i++ ){

        normal_v += cross(v[i],v[(i+1) % neighbor_count ]);
    }

    normalize(normal_v);

    vertexNormal = normalize(vec3(normalMatrix*vec4(normal_v,0)));
    ToLightVector = normalize(vec3(MV*vec4(light_pos - current_position, 0)));
    ToCameraVector = normalize(vec3(MV*(vec4(vec3(cameraPosition) - current_position, 0))));
  
    gl_Position = MVP*vec4(current_position,1); 
}