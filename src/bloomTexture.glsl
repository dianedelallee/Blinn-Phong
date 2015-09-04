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

uniform sampler2D Rendered;
uniform sampler2D Material;
out vec4  Color;




void main(){
   vec4 sum = vec4(0);
   float j;
   float i;
 int width = 1024;
 int height=768;
 
   for( i= -3 ;i < 3; i++){
        for (j = -4; j < 3; j++){
            sum += texture2D(Rendered, uv + vec2(j/1024.0, i/768.0)) * 0.45;
        }
   }
   

   if ((texture2D(Rendered, uv).r < 0.3) ){
   	if((sum.r > 0.1)|| (sum.g > 0.1) ||(sum.b > 0.1)){
 		Color = sum*sum*0.012 + texture2D(Rendered, uv);
   	}
   	else Color = texture2D(Rendered, uv);
   }
   else{
        if((texture2D(Rendered, uv).r < 0.5)){
        	if((sum.r > 0.1)|| (sum.g > 0.1) ||(sum.b > 0.1)){
 			Color = sum*sum*0.009 + texture2D(Rendered, uv);
   			}
   			else Color = texture2D(Rendered, uv);
        }
        else{
          if((sum.r > 0.1)|| (sum.g > 0.1) ||(sum.b > 0.1)){
 				Color = sum*sum*0.0075 + texture2D(Rendered, uv);
   			}
   			else Color = texture2D(Rendered, uv);
        }
   }

   

   
   
   
   
   
   
   
   
}

#endif
