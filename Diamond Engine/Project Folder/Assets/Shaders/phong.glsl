#ifdef vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec3 tangents;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;
uniform float time;
uniform vec3 cameraPosition;

uniform vec3 lightPos;

out VS_OUT {
    vec4 clipSpace;
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

void main()
{
 vs_out.FragPos = vec3(model_matrix * vec4(position, 1.0));
 vs_out.TexCoords = texCoord;
 
 vec3 T = normalize(vec3(model_matrix * vec4(tangents, 0.0)));
 vec3 N = normalize(vec3(model_matrix * vec4(normals, 0.0)));
 T = normalize(T - dot(T, N) * N);
 vec3 B = cross(N,T);
 
 mat3 TBN = mat3(T,B,N);
 vs_out.TangentLightPos = TBN * lightPos;
 vs_out.TangentViewPos  = TBN * cameraPosition;
 vs_out.TangentFragPos  = TBN * vs_out.FragPos;
 
 gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
}
#endif

#ifdef fragment
#version 330 core

out vec4 FragColor;

float opacity = 1.0f;

uniform sampler2D diffuseTexture;
uniform vec2 diffuseTextureTiling;
uniform sampler2D normalMap;
uniform vec2 normalMapTiling;
uniform vec3 altColor;

uniform float time;
uniform vec3 lightPos;
uniform bool activeTextures[];

in VS_OUT {
	vec4 clipSpace;
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

void main()
{
    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
   
   //diffuseColor
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    //ambient 
    vec3 ambient = 0.1 * color;
    // diffuse 
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    vec3 specular = vec3(0.2) * spec; 
    
     FragColor =  vec4(ambient + diffuse + specular, opacity) * vec4(altColor, 1.0);
}
#endif


