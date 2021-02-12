#ifdef vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;

uniform mat3 normalMatrix;
uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraPosition;
uniform vec2 normalMapTiling;

uniform float time;
float speed = 0.25;
uniform float wave_length;
float steepness = 0.5;

uniform vec3 direction_1;
vec3 direction_2 = vec3(0.5, 0.5, 0.5);
vec3 direction_3 = vec3(0.5, 0.5, 0.5);

out float relative_position;

out VS_OUT {
	vec4 clipSpace;
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} vs_out;

vec3 lightPos = vec3(0.0, 10.0, 0.0);

vec3 generateWave(float amp, vec3 direction, float num_waves, float steepness, float phase_constant, float w);

void main()
{
 vec3 fPosition = position;
 float pi = 3.1415f;
 float phase_constant = speed * 2.0 * pi / wave_length;
 float w = sqrt(9.81 * (2 * pi / wave_length));
 float num_waves = 3.0;
 
 float amp1 = 0.5;
 vec3 wave1 = generateWave(amp1, direction_1, num_waves, steepness, phase_constant, w);
 
 float amp2 = 0.035;
 vec3 wave2 = generateWave(amp2, direction_2, num_waves, steepness, phase_constant, w);
 
 float amp3 = 0.035;
 vec3 wave3 = generateWave(amp3, direction_3, num_waves, steepness, phase_constant, w);

 fPosition += wave1 + wave2 + wave3;
 relative_position = fPosition.z / ((amp1 * 2.0 + amp2 * 2.0 + amp3 * 2.0) / num_waves);
 relative_position = (relative_position + 0.5) * 0.5;
 relative_position = max(relative_position, 0.0);
 
 vs_out.FragPos = vec3(model_matrix * vec4(position, 1.0));

 vs_out.TexCoords = texCoord * normalMapTiling;
 vs_out.TexCoords.x += cos(time) * speed * 0.25;
 vs_out.TexCoords.y += sin(time) * speed * 0.25;
 
 vec3 T = normalize(normalMatrix * tangent);
 vec3 N = normalize(normalMatrix * normal);
 T = normalize(T - dot(T, N) * N);
 vec3 B = cross(N,T);
 
 mat3 TBN = transpose(mat3(T,B,N));
 vs_out.TangentLightPos = TBN * lightPos;
 vs_out.TangentViewPos  = TBN * cameraPosition;
 vs_out.TangentFragPos  = TBN * vs_out.FragPos;
 
 vs_out.clipSpace = projection * view * model_matrix * vec4(fPosition, 1.0);
 gl_Position = projection * view * model_matrix * vec4(fPosition, 1.0);

}

vec3 generateWave(float amp, vec3 direction, float num_waves, float steepness,
 float phase_constant, float w ) {
 vec3 dir = normalize(direction);
 float q = steepness / (w * amp * num_waves);
 
 vec3 wave;
 wave.x = q * amp * dir.x * cos(dot(w * dir, position) + phase_constant * time);
 wave.y = q * amp * dir.y * cos(dot(w * dir, position) + phase_constant * time);
 wave.z = amp * sin(dot(w * dir, position) + phase_constant * time);
 
 return wave;
}
#endif

#ifdef fragment
#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform vec3 cameraPosition;
uniform float time;

uniform float opacity;
uniform vec3 color;
uniform float contrast;
uniform float speed;
uniform float steepness;
uniform vec2 normalMapTiling;
in float relative_position;
in vec3 Normal;

in VS_OUT {
	vec4 clipSpace;
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

vec2 rand2( vec2 coord );
float cellular_noise(vec2 coord);
float fbm(vec2 coord);
float LinearizeDepth(float depth);

void main()
{
    vec2 coord = fs_in.FragPos.xz * (2 /normalMapTiling.x);
    float value = fbm(coord);   
    
    vec3 normal = texture(normalMap, fs_in.TexCoords * 1.5).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    
    //ambient 
    float ambientStrength = 0.75;
    vec3 ambient = ambientStrength * color;
  	
    // diffuse 
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    
    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    
    vec3 specular = vec3(0.5) * spec;  
    
    vec3 foam = vec3(value);

    vec4 waterColor = vec4(ambient + diffuse + specular, opacity);
	vec4 foamColor = vec4(vec3(value * relative_position), value * relative_position);
	
	FragColor = vec4(waterColor.rgb, opacity) + foamColor;
} 



vec2 rand2( vec2 coord ) {
  coord = mod(coord, 10000.0);
  return fract(sin(vec2(dot(coord,vec2(127.1,311.7)),dot(coord,vec2(269.5,183.3))))*43758.5453);
}

float cellular_noise(vec2 coord){
  vec2 i = floor(coord);
  vec2 f = fract(coord);
  
  float min_dist = 99999.0;
  
  for(float x = -1.0; x <= 1.0; x++)
  {
    for(float y = -1.0; y <= 1.0; y++)
    {
      vec2 node = rand2(i + vec2(x,y)) + vec2(x,y);
      float dist = sqrt((f-node).x * (f-node).x + (f - node).y * (f - node).y);
      
      min_dist = min(min_dist, dist);
    }  
  }
  
  return min_dist;
}

float fbm(vec2 coord) {
 int octaves = 4;
 
 float normalize_factor = 0.0;
 float value = 0.0;
 float scale = 0.5;
 
 for(int i = 0; i < octaves; i++){
  value += cellular_noise(coord) * scale;
  normalize_factor += scale;
  coord *= 2.0;
  scale *= 0.5;
 }
 
 return value / normalize_factor;
}

float LinearizeDepth(float depth)
{
 float near = 0.1;
 float far = 1000.0;
 return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}
#endif



