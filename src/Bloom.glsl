#if defined(VERTEX)

in vec2 VertexPosition;

out vec2 uv;

void main(void)
{	
	uv = VertexPosition * 0.5 + 0.5;
	gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);
}

#endif

#if defined(FRAGMENT)

in vec2 uv;

uniform sampler2D Material;
uniform sampler2D Normal;
uniform sampler2D Depth; // profondeur
uniform sampler2D Position; // position
uniform sampler2D RandomTexture;  // Normalmap to randomize the sampling kernel

uniform vec3 CameraPosition; 
uniform vec3  LightPosition;
uniform vec3  LightColor;
uniform float LightIntensity;
uniform float TexturesizeX;
uniform float TexturesizeY;
uniform mat4 InverseViewProjection;


out vec4  Color;


float SamplePixels(vec3 srcPosition, vec3 srcNormal, vec2 uv, float OccluderBias){
	vec2 Attenuation = vec2(6.0, 0.5);

  
    vec3 dstPosition = texture(Position, uv).rgb; 

   
    vec3 positionVec = dstPosition - srcPosition;
    float intensity = 22*max(dot(normalize(positionVec), srcNormal) - OccluderBias, 0.0);
 	
    float dist = length(positionVec);
    float attenuation = 1.0 / (Attenuation.x + (Attenuation.y * dist));
    
    return intensity * attenuation;
}






void main(void){
//texture 
	vec4  material = texture(Material, uv).rgba;
	vec3  normal = texture(Normal, uv).rgb;
	float depth = texture(Depth, uv).r;
	vec3  position_sampled = texture(Position, uv).rgb;
	vec2 randVec = normalize(texture(RandomTexture, uv).xy * 2.0 - 1.0);
	
	
	/************** SSAO **************/
	float SamplingRadius = 10.0;
	float OccluderBias = 0.1;
	
	float kernelRadius = SamplingRadius * (1.0 - depth);
		
	vec2 kernel[4] ;		
    kernel[0] = vec2(0.0, 1.0); 
    kernel[1] = vec2(1.0, 0.0); 
    kernel[2] = vec2(0.0, -1.0);    
    kernel[3] = vec2(-1.0, 0.0);    
	
	const float Sin45 = 0.707107;   
	float occlusion = 0.0;
	vec2 k1 = vec2(0.0, 0.0);
	vec2 k2 = vec2(0.0, 0.0);
	for(int i = 0; i<4; ++i){
		 k1 = reflect(kernel[i], randVec);
		 k2 = vec2(k1.x * Sin45 - k1.y * Sin45, k1.x * Sin45 + k1.y * Sin45);
		
		k1 *= vec2(TexturesizeX, TexturesizeY);
        	k2 *= vec2(TexturesizeX, TexturesizeY);
        
        	occlusion += SamplePixels(position_sampled, normal, uv + k1 * kernelRadius, OccluderBias);
        	occlusion += SamplePixels(position_sampled, normal, uv + k2 * kernelRadius * 0.75, OccluderBias);
        	occlusion += SamplePixels(position_sampled, normal, uv + k1 * kernelRadius * 0.5, OccluderBias);
       		occlusion += SamplePixels(position_sampled, normal, uv + k2 * kernelRadius * 0.25, OccluderBias);
   
	}
	
	
    occlusion /= 16.0;
    occlusion = clamp(occlusion, 0.0, 1.0);
	
	
//position camera 
	vec2  xy = uv * 2.0 -1.0;
	vec4  wPosition =  vec4(xy, depth * 2.0 -1.0, 1.0) * InverseViewProjection;
	vec3  position = vec3(wPosition/wPosition.w);

//pour la lumiere
	vec3 diffuse = material.rgb;
	float spec = material.a;

	vec3 n = normalize(normal);
	vec3 l =  LightPosition - position;

	vec3 v = position - CameraPosition;
	vec3 h = normalize(l-v);
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);

	float d = distance(LightPosition, position);
	float att = clamp(  1.0 / ( 1.0 + 1.0 * (d*d)), 0.0, 1.0);

//couleur
	vec3 color = LightColor * LightIntensity * att * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0))*occlusion;
//vec3 color = LightColor * LightIntensity * att * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0));


//  =================================bloom effect========================================================
vec4 sum  = vec4(0.0,0.0,0.0,0.0);
   vec2 texcoord = uv.xy;
   int j;
   int i;
vec4 col;

   for( i= -4 ;i < 4; i++){
        for (j = -4; j < 4; j++){
            sum += texture(Material, texcoord + vec2(j, i)*0.0015) * 0.20;
        }
   }
   if (texture(Material, texcoord).r < 0.3){
       	col = sum*sum*0.012 + texture(Material, texcoord);
    }
    else{
        if (texture(Material, texcoord).r < 0.5){
            col = sum*sum*0.009 + texture(Material, texcoord);
        }
        else {
            col = sum*sum*0.0075 + texture(Material, texcoord);
        }
    }

	Color = col * vec4(color, depth);





}

#endif
