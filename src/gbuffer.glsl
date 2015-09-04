#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec2 uv;
out vec3 normal;
out vec3 position2;

void main(void)
{	
//coordonnees des vertex
	uv = VertexTexCoord;
// normal
	//normal = vec3(Object * vec4(VertexNormal, 1.0));
	normal = normalize(vec3(transpose(inverse(Object)) * vec4(VertexNormal, 1.0)));
//position de la camera
	vec3 position = vec3(Object * vec4(VertexPosition, 1.0));
	
	position.x += (gl_InstanceID % 2) ; 
	position.y += (int(gl_InstanceID / 3) );
	position.z +=  (gl_InstanceID % 3) ; 
	gl_Position = Projection * View * vec4(position, 1.0);
	position2 = vec3(View * vec4(position, 1.0)).xyz;
	
}

#endif

#if defined(FRAGMENT)
uniform vec3 CameraPosition;
uniform float Time;

in vec2 uv;
in vec3 position2;
in vec3 normal;

uniform sampler2D Diffuse;
uniform sampler2D Spec;
//uniform sampler2D Bois;

out vec4  Color;
out vec4  Normal;
out vec4  Position;

void main(void){
// lumiere
	vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = texture(Spec, uv).r;
//couleur
	Color = vec4(diffuse, spec);
//normal
	Normal = vec4(normal, spec);
//position	
	 Position = vec4(position2, spec);
}

#endif
