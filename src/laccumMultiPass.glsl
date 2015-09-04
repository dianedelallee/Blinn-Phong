//quit
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
uniform sampler2D RenderedTexture; 
uniform float PremierRendu;

uniform vec3 CameraPosition; 
uniform vec3  LightPosition;
uniform vec3  LightColor;
uniform float LightIntensity;
uniform mat4 InverseViewProjection;

out vec4  Color;






void main(void){
//texture ?!
	vec4  material = texture(Material, uv).rgba;
	vec3  normal = texture(Normal, uv).rgb;
	float depth = texture(Depth, uv).r;

	
	
	
//position camera ?!
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
	
	vec3 color = LightColor * LightIntensity * att * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0));

	if(PremierRendu==1){
		Color = vec4(0.0, 0.0, 0.0, depth);
	}
	else{
		vec3 rendered = texture(RenderedTexture, uv).xyz;
		color += rendered;
		Color = vec4(color, depth);
	}

}

#endif
