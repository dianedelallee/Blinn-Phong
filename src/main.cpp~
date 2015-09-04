#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <cmath>
#include <iostream>
#include "GLCommon.hpp"
#include "GL/glfw.h"
#include "stb_image/stb_image.h"
#include "imgui.h"
#include "imguiRenderGL.h"
#include <GL/glut.h>
#include "loadingPLY.hpp"
#include "FramebufferGL.hpp"
#include "ShaderGLSL.hpp"
#include "Camera.hpp"
#include "Transform.hpp"
#include "LinearAlgebra.hpp"


#ifndef DEBUG_PRINT
#define DEBUG_PRINT 1
#endif

#if DEBUG_PRINT == 0
#define debug_print(FORMAT, ...) ((void)0)
#else
#ifdef _MSC_VER
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__)
#else
#define debug_print(FORMAT, ...) \
    fprintf(stderr, "%s() in %s, line %i: " FORMAT "\n", \
        __func__, __FILE__, __LINE__, __VA_ARGS__)
#endif
#endif

using namespace std;

struct GUIStates
{
    bool panLock;
    bool turnLock;
    bool zoomLock;
    int lockPositionX;
    int lockPositionY;
    int camera;
    double time;
    bool playing;
    static const float MOUSE_PAN_SPEED;
    static const float MOUSE_ZOOM_SPEED;
    static const float MOUSE_TURN_SPEED;
};
const float GUIStates::MOUSE_PAN_SPEED = 0.001f;
const float GUIStates::MOUSE_ZOOM_SPEED = 0.05f;
const float GUIStates::MOUSE_TURN_SPEED = 0.005f;

//////////////////////////////////////// matrices de transformation /////////////////////////////////////////////
void translation(int axe, float dist, float tra[]){
		if(axe ==1){
		tra[12] = dist;
	}
	if(axe ==2){
		tra[13] = dist;
	}
	if(axe ==3){
		tra[14] = dist;
	}
	
}
void clearTranslation(float tra[]){

	tra[12]=0.0;
	tra[13]=0.0;
	tra[14]=0.0;
}

   

void rotation(int axe, float angle, float rotation[]){
	double c = cos(angle);
        double s = sin(angle);
	if(axe ==1){
		rotation[5] = c;
		rotation[6] = -1*s;
		rotation[9] = s;
		rotation[10] = c;		
			
			
	}
	if(axe ==2){
		rotation[0] = c;
		rotation[2] = -1*s;		
		rotation[8] = s;	
		rotation[10] = c;	
	}
	if(axe ==3){
		rotation[0] = c;
		rotation[1] = -1*s;		
		rotation[4] = s;	
		rotation[5] = c;	
	}

}

void clearRotation(float rotation[]){
		rotation[5] = 1.0;
		rotation[9] = 0;		
		rotation[6] = 0;	
		rotation[10] =1.0;	
		rotation[2] = 0;		
		rotation[8] = 0;
		rotation[1] = 0;		
		rotation[4] = 0;
		rotation[0] = 1.0;
	
}

void homothety(float taille, float hom[]){
	hom[0] =taille;
	hom[5] =taille;
	hom[10] =taille;

}
/////////////////////// fin matrice de transformations /////////////////////////////////////////////////


void init_gui_states(GUIStates & guiStates){
    guiStates.panLock = false;
    guiStates.turnLock = false;
    guiStates.zoomLock = false;
    guiStates.lockPositionX = 0;
    guiStates.lockPositionY = 0;
    guiStates.camera = 0;
    guiStates.time = 0.0;
    guiStates.playing = false;
}

int main( int argc, char **argv ){
    int width = 1024, height=768;
    double t;
    int display_mode =1;
    int mode_count =0;
    int help = 0;

//compte nombre fps
static float current_time = 0.0;
static float last_time = 0.0;


    // Initialise GLFW
    if( !glfwInit() ){
        fprintf( stderr, "Failed to initialize GLFW\n" );
        exit( EXIT_FAILURE );
    }
    
#ifdef __APPLE__
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    // ouverture d'une fenetre et contexte de rendu
    if( !glfwOpenWindow( width, height, 0,0,0,0, 24,0, GLFW_WINDOW ) ){
        fprintf( stderr, "Failed to open GLFW window\n" );

        glfwTerminate();
        exit( EXIT_FAILURE );
    }


    glfwSetWindowTitle( "Projet : E. Bonnefoy - D. Delallee" );

#ifdef __APPLE__
    glewExperimental = GL_TRUE;
#endif
    GLenum err = glewInit();
    if (GLEW_OK != err) {
          /* initialisation glew ne fonctionne pas */
          fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
          exit( EXIT_FAILURE );
    }

    // Ensure we can capture the escape key being pressed below
    glfwEnable( GLFW_STICKY_KEYS );

    // permet la synchronisation verticale (si oui baisse le nombre de fps) 
    glfwSwapInterval( 0 );

    // Initialisation UI
    if (!imguiRenderGLInit("DroidSans.ttf")){
        fprintf(stderr, "Could not init GUI renderer.\n");
        exit(EXIT_FAILURE);
    }

    // initialisation des structures de vues 
    Camera camera;
    GUIStates guiStates;
    init_gui_states(guiStates);

    float numLights = 10.f;
    
    // matrices pour les transformations

 float tra[16] = {1.0,  0.0, 0.0, 0.0,0.0,  1.0, 0.0, 0.0,0.0,  0.0, 1.0, 0.0,0.0,  0.0, 0.0, 1.0, };
 float rot[16] = {1.0,  0.0, 0.0, 0.0,0.0,  1.0, 0.0, 0.0,0.0,  0.0, 1.0, 0.0,0.0,  0.0, 0.0, 1.0, };
 float hom[16] = {1.0,  0.0, 0.0, 0.0,0.0,  1.0, 0.0, 0.0,0.0,  0.0, 1.0, 0.0,0.0,  0.0, 0.0, 1.0, };	
       

    // chargement et mise a jour des textures 
    GLuint textures[5];
    glGenTextures(5, textures);
    int x;
    int y;
    int comp; 
    unsigned char * diffuse = stbi_load("textures/box.tga", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned char * spec = stbi_load("textures/box.tga", &x, &y, &comp, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, spec);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);
    glBindTexture(GL_TEXTURE_2D, 0);


    unsigned char * diffuse_sol = stbi_load("textures/masonry-wall-texture.tga", &x, &y, &comp, 3);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, diffuse_sol);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Diffuse %dx%d:%d\n", x, y, comp);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned char * spec_sol = stbi_load("textures/masonry-wall-texture.tga", &x, &y, &comp, 1);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, spec_sol);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "Spec %dx%d:%d\n", x, y, comp);
    glBindTexture(GL_TEXTURE_2D, 0);

    // normales aleatoire -> pour l'SSAO 
    unsigned char * normalmap = stbi_load("textures/normalmap.png", &x, &y, &comp, 1);
   // unsigned char * normalmap = stbi_load("textures/noise.tga", &x, &y, &comp, 1); // autre texture pour la normalmap
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, textures[4]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RED, GL_UNSIGNED_BYTE, normalmap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    fprintf(stderr, "normalmap: %dx%d:%d\n", x, y, comp);
    glBindTexture(GL_TEXTURE_2D, 0);

	int texturesize[2] = {x, y};

	//definition et chargement des objets
Model_PLY cow;
int cowID = cow.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/cow.ply");

Model_PLY streetLamp;
int streetLampID = streetLamp.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/street_lamp.ply");

Model_PLY tree;
int treeID = tree.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/fracttree.ply");

Model_PLY egret;
int egretID = egret.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/egret.ply");

Model_PLY bird;
int birdID = bird.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/egret.ply");

Model_PLY helico;
int helicoID = helico.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/chopper.ply");

Model_PLY pickup;
int pickupID = pickup.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/pickup_big.ply");

Model_PLY rabbit;
int rabbitID = rabbit.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/bunny1850.ply");

Model_PLY porsche;
int porscheID = porsche.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/big_porsche.ply");

Model_PLY moto;
int motoID = moto.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/big_atc.ply");

Model_PLY snail;
int snailID = snail.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/snail.ply");

Model_PLY dragon;
int dragonID = dragon.Load("/home/2sis2a/ddelalle/Bureau/SyntheseAvanceeProjet/src/mesh/dragon.ply");


    // chargement et compilation des shader
    int status;
	   // gbuffer
    ShaderGLSL gbuffer_shader;
    status = load_shader_from_file(gbuffer_shader, "src/gbuffer.glsl", ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  src/gbuffer.glsl\n");
        exit( EXIT_FAILURE );
    }
  
    GLuint gbuffer_projectionLocation = glGetUniformLocation(gbuffer_shader.program, "Projection");
    GLuint gbuffer_viewLocation = glGetUniformLocation(gbuffer_shader.program, "View");
    GLuint gbuffer_objectLocation = glGetUniformLocation(gbuffer_shader.program, "Object");
    GLuint gbuffer_timeLocation = glGetUniformLocation(gbuffer_shader.program, "Time");
    GLuint gbuffer_diffuseLocation = glGetUniformLocation(gbuffer_shader.program, "Diffuse");
    GLuint gbuffer_specLocation = glGetUniformLocation(gbuffer_shader.program, "Spec");


    // blit
    ShaderGLSL blit_shader;
    status = load_shader_from_file(blit_shader, "src/blockImageTransfert.glsl", ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  src/blockImageTransfert.glsl\n");
        exit( EXIT_FAILURE );
    }
    GLuint blit_tex1Location = glGetUniformLocation(blit_shader.program, "Texture1");


    // laccum
    ShaderGLSL laccum_shader;
    status = load_shader_from_file(laccum_shader, "src/laccum.glsl", ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  src/laccum.glsl\n");
        exit( EXIT_FAILURE );
    }
    GLuint laccum_materialLocation = glGetUniformLocation(laccum_shader.program, "Material");
    GLuint laccum_normalLocation = glGetUniformLocation(laccum_shader.program, "Normal");
    GLuint laccum_depthLocation = glGetUniformLocation(laccum_shader.program, "Depth");
    GLuint laccum_inverseViewProjectionLocation = glGetUniformLocation(laccum_shader.program, "InverseViewProjection");
    GLuint laccum_cameraPositionLocation = glGetUniformLocation(laccum_shader.program, "CameraPosition");
    GLuint laccum_lightPositionLocation = glGetUniformLocation(laccum_shader.program, "LightPosition");
    GLuint laccum_lightColorLocation = glGetUniformLocation(laccum_shader.program, "LightColor");
    GLuint laccum_lightIntensityLocation = glGetUniformLocation(laccum_shader.program, "LightIntensity");

    //laccumMultiPass
    ShaderGLSL laccumMultiPass_shader;
    status = load_shader_from_file(laccumMultiPass_shader, "src/laccumMultiPass.glsl", ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading  src/laccumMultiPass.glsl\n");
        exit( EXIT_FAILURE );
    }
    GLuint laccumMultiPass_materialLocation = glGetUniformLocation(laccumMultiPass_shader.program, "Material");
    GLuint laccumMultiPass_normalLocation = glGetUniformLocation(laccumMultiPass_shader.program, "Normal");
    GLuint laccumMultiPass_depthLocation = glGetUniformLocation(laccumMultiPass_shader.program, "Depth");
    GLuint laccumMultiPass_inverseViewProjectionLocation = glGetUniformLocation(laccumMultiPass_shader.program, "InverseViewProjection");
    GLuint laccumMultiPass_cameraPositionLocation = glGetUniformLocation(laccumMultiPass_shader.program, "CameraPosition");
    GLuint laccumMultiPass_lightPositionLocation = glGetUniformLocation(laccumMultiPass_shader.program, "LightPosition");
    GLuint laccumMultiPass_lightColorLocation = glGetUniformLocation(laccumMultiPass_shader.program, "LightColor");
    GLuint laccumMultiPass_lightIntensityLocation = glGetUniformLocation(laccumMultiPass_shader.program, "LightIntensity");
    GLuint laccumMultiPass_premierrenduLocation = glGetUniformLocation(laccumMultiPass_shader.program, "PremierRendu");
    GLuint laccumMultiPass_renderedTexturepLocation = glGetUniformLocation(laccumMultiPass_shader.program, "RenderedTexture");
	
    //  SSAO
    ShaderGLSL  SSAO_shader;
    status = load_shader_from_file( SSAO_shader, "src/SSAO.glsl", ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading   SSAO.glsl\n");
        exit( EXIT_FAILURE );
    }
    GLuint  SSAO_materialLocation = glGetUniformLocation(SSAO_shader.program, "Material");
    GLuint  SSAO_normalLocation = glGetUniformLocation( SSAO_shader.program, "Normal");
    GLuint  SSAO_positionLocation = glGetUniformLocation( SSAO_shader.program, "Position");
    GLuint  SSAO_depthLocation = glGetUniformLocation( SSAO_shader.program, "Depth");
    GLuint  SSAO_normalmapLocation = glGetUniformLocation( SSAO_shader.program, "Randomtexture");
    GLuint  SSAO_inverseViewProjectionLocation = glGetUniformLocation( SSAO_shader.program, "InverseViewProjection");
    GLuint  SSAO_cameraPositionLocation = glGetUniformLocation( SSAO_shader.program, "CameraPosition");
    GLuint  SSAO_lightPositionLocation = glGetUniformLocation( SSAO_shader.program, "LightPosition");
    GLuint  SSAO_lightColorLocation = glGetUniformLocation( SSAO_shader.program, "LightColor");
    GLuint  SSAO_lightIntensityLocation = glGetUniformLocation( SSAO_shader.program, "LightIntensity");
    GLuint  SSAO_texturesizeXLocation = glGetUniformLocation( SSAO_shader.program, "TexturesizeX");
    GLuint  SSAO_texturesizeYLocation = glGetUniformLocation( SSAO_shader.program, "TexturesizeY");

// bloom 
    ShaderGLSL  bloom_shader;
    status = load_shader_from_file( bloom_shader, "src/Bloom.glsl", ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading   bloom.glsl\n");
        exit( EXIT_FAILURE );
    }
    GLuint  bloom_materialLocation = glGetUniformLocation(bloom_shader.program, "Material");
    GLuint  bloom_normalLocation = glGetUniformLocation( bloom_shader.program, "Normal");
    GLuint  bloom_positionLocation = glGetUniformLocation( bloom_shader.program, "Position");
    GLuint  bloom_depthLocation = glGetUniformLocation( bloom_shader.program, "Depth");
    GLuint  bloom_normalmapLocation = glGetUniformLocation( bloom_shader.program, "Randomtexture");
    GLuint  bloom_inverseViewProjectionLocation = glGetUniformLocation( bloom_shader.program, "InverseViewProjection");
    GLuint  bloom_cameraPositionLocation = glGetUniformLocation( bloom_shader.program, "CameraPosition");
    GLuint  bloom_lightPositionLocation = glGetUniformLocation( bloom_shader.program, "LightPosition");
    GLuint  bloom_lightColorLocation = glGetUniformLocation( bloom_shader.program, "LightColor");
    GLuint  bloom_lightIntensityLocation = glGetUniformLocation( bloom_shader.program, "LightIntensity");
    GLuint  bloom_texturesizeXLocation = glGetUniformLocation( bloom_shader.program, "TexturesizeX");
    GLuint  bloom_texturesizeYLocation = glGetUniformLocation( bloom_shader.program, "TexturesizeY");

//bloomTexture
ShaderGLSL  bloomTexture_shader;
    status = load_shader_from_file( bloomTexture_shader, "src/bloomTexture.glsl", ShaderGLSL::VERTEX_SHADER | ShaderGLSL::FRAGMENT_SHADER);
    if ( status == -1 )
    {
        fprintf(stderr, "Error on loading   bloomTexture.glsl\n");
        exit( EXIT_FAILURE );
    }
    GLuint  bloomTexture_renderedLocation = glGetUniformLocation(bloomTexture_shader.program, "Material");



    
    //pour dessiner un plan (sol)
    int   plane_triangleCount = 2;
    int   plane_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float plane_uvs[] = {0.f, 0.f, 0.f, 10.f, 10.f, 0.f, 10.f, 10.f};
    float plane_vertices[] = {-50.0, -1.0, 50.0, 50.0, -1.0, 50.0, -50.0, -1.0, -50.0, 50.0, -1.0, -50.0};
    float plane_normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
    
    
    int   quad_triangleCount = 2;
    int   quad_triangleList[] = {0, 1, 2, 2, 1, 3}; 
    float quad_vertices[] =  {-1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0};

    // Vertex Array Object
    GLuint vao[3];
    glGenVertexArrays(3, vao);

    // Vertex Buffer Objects
    GLuint vbo[12];
    glGenBuffers(12, vbo);




    // Plan
    glBindVertexArray(vao[1]);
    // attache les indices et met a jour les donnees
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_triangleList), plane_triangleList, GL_STATIC_DRAW);
    //attache les vertices et met a jour les donnees
    glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
    // attache les normales et met a jour les donnees
    glBindBuffer(GL_ARRAY_BUFFER, vbo[6]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_normals), plane_normals, GL_STATIC_DRAW);
    // attache les coordonnes de texture et met a jour les donnees
    glBindBuffer(GL_ARRAY_BUFFER, vbo[7]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_uvs), plane_uvs, GL_STATIC_DRAW);


    // Quad
    glBindVertexArray(vao[2]);
    // attache les indices et met a jour les donnees
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[8]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_triangleList), quad_triangleList, GL_STATIC_DRAW);
    // attache les vertices et met a jour les donnees
    glBindBuffer(GL_ARRAY_BUFFER, vbo[9]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*2, (void*)0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);


    // on detache tout
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


/********************** Initialisation des GBuffers *************************/

    // Initialisation gbuffers
    FramebufferGL gbuffer; //laccum
    status = build_framebuffer(gbuffer, width, height, 3);  // color, normals, position
    if (status == -1)
    {
        fprintf(stderr, "Error on building gbuffer\n");
        exit( EXIT_FAILURE );
    }

 // Initialisation gbuffers
    FramebufferGL laccumgbuffer; //laccum
    status = build_framebuffer(laccumgbuffer, width, height, 3);  // color, normals, position
    if (status == -1)
    {
        fprintf(stderr, "Error on building laccumgbuffer\n");
        exit( EXIT_FAILURE );
    }

 // Initialisation gbuffers
    FramebufferGL transfergbuffer; //laccum
    status = build_framebuffer(transfergbuffer, width, height, 3);  // color, normals, position
    if (status == -1)
    {
        fprintf(stderr, "Error on building transfergbuffer\n");
        exit( EXIT_FAILURE );
    }



    do
    {
	//calcul nombre de fps
        t = glfwGetTime();
	last_time = current_time;
	current_time = glfwGetTime();
	float fps = 1.0/(current_time - last_time);
	float fps2 = 	(current_time - last_time);
	//std::cout<< "time : " << fps << std::endl;


        // etat de la souris
        int leftButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT );
        int rightButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT );
        int middleButton = glfwGetMouseButton( GLFW_MOUSE_BUTTON_MIDDLE );

        if( leftButton == GLFW_PRESS )
            guiStates.turnLock = true;
        else
            guiStates.turnLock = false;

        if( rightButton == GLFW_PRESS )
            guiStates.zoomLock = true;
        else
            guiStates.zoomLock = false;

        if( middleButton == GLFW_PRESS )
            guiStates.panLock = true;
        else
            guiStates.panLock = false;

        // mouvements de la camera
        int altPressed = glfwGetKey(GLFW_KEY_LSHIFT);
	 if (! (leftButton == GLFW_PRESS || rightButton == GLFW_PRESS || middleButton == GLFW_PRESS))        
	{
            int x; int y;
            glfwGetMousePos(&x, &y);
            guiStates.lockPositionX = x;
            guiStates.lockPositionY = y;
        }
	 if ((leftButton == GLFW_PRESS || rightButton == GLFW_PRESS || middleButton == GLFW_PRESS))  
        {
            int mousex; int mousey;
            glfwGetMousePos(&mousex, &mousey);
            int diffLockPositionX = mousex - guiStates.lockPositionX;
            int diffLockPositionY = mousey - guiStates.lockPositionY;
            if (guiStates.zoomLock)
            {
                float zoomDir = 0.0;
                if (diffLockPositionX > 0)
                    zoomDir = -1.f;
                else if (diffLockPositionX < 0 )
                    zoomDir = 1.f;
                camera.zoom(zoomDir * GUIStates::MOUSE_ZOOM_SPEED);
            }
            else if (guiStates.turnLock)
            {
                camera.turn(diffLockPositionY * GUIStates::MOUSE_TURN_SPEED,
                            diffLockPositionX * GUIStates::MOUSE_TURN_SPEED);

            }
            else if (guiStates.panLock)
            {
                camera.pan(diffLockPositionX * GUIStates::MOUSE_PAN_SPEED,
                           diffLockPositionY * GUIStates::MOUSE_PAN_SPEED);
            }
            guiStates.lockPositionX = mousex;
            guiStates.lockPositionY = mousey;
        }
  
  	// mode d'affichage - attribution touche mode d'affichage
		if( glfwGetKey(GLFW_KEY_F1 ) == GLFW_PRESS ){
	   		display_mode = 1;
		}
		else if( glfwGetKey(GLFW_KEY_F2 ) == GLFW_PRESS){
			display_mode = 2;    
		}
		else if( glfwGetKey(GLFW_KEY_F3 ) == GLFW_PRESS){
			display_mode = 3;    
		}
		else if( glfwGetKey(GLFW_KEY_F4 ) == GLFW_PRESS){
			display_mode = 4;
		}
		else if( glfwGetKey(GLFW_KEY_F5 ) == GLFW_PRESS){
			display_mode = 5;
		}
		else if( glfwGetKey(GLFW_KEY_F6 ) == GLFW_PRESS){
			display_mode = 6;
		}
		else if( glfwGetKey(GLFW_KEY_F7 ) == GLFW_PRESS){
			display_mode = 7;
		}
		else if( glfwGetKey(GLFW_KEY_F8 ) == GLFW_PRESS){
			display_mode = 8;
		}
		else if( glfwGetKey(GLFW_KEY_F9 ) == GLFW_PRESS){
			display_mode = 9;
		}
		
		else if( glfwGetKey(GLFW_KEY_DEL ) == GLFW_PRESS){
			display_mode = 10;
		}
		else if( glfwGetKey(GLFW_KEY_INSERT ) == GLFW_PRESS){
			display_mode = 11;
		}
		else if( glfwGetKey(GLFW_KEY_CAPS_LOCK ) == GLFW_PRESS){
				help = 1;
		}
		else if( glfwGetKey(GLFW_KEY_LSHIFT ) == GLFW_PRESS){
				help = 0;
		}
  
        // obtention des matrices de la camera
        float projection[16];
        float worldToView[16];
        float objectToWorld[16];
        float cameraPosition[4];
        float orthoProj[16];
        float objectToWorldTmp[16];
        
        mat4fCopy(projection, camera.perspectiveProjection());
        mat4fCopy(worldToView, camera.worldToView());
        mat4fToIdentity(objectToWorld);
        vec4fCopy(cameraPosition, camera.position());
        mat4fToIdentity(objectToWorldTmp);

        float viewProjection[16];     
        float iviewProjection[16];       

        mat4fMul( worldToView, projection, viewProjection);
        mat4fInverse(viewProjection, iviewProjection);

        glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.fbo);
        glDrawBuffers(gbuffer.outCount, gbuffer.drawBuffers);
 


  
        glEnable(GL_DEPTH_TEST);

        // nettoyage du buffer de profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // attache le gbuffer
        glUseProgram(gbuffer_shader.program);
        // envoie des donnees
        glUniformMatrix4fv(gbuffer_projectionLocation, 1, 0, projection);
        glUniformMatrix4fv(gbuffer_viewLocation, 1, 0, worldToView);
        glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
        glUniform1f(gbuffer_timeLocation, t);
        
    	glUniform1i(gbuffer_diffuseLocation, 0);
        glUniform1i(gbuffer_specLocation, 1);

        // attache les textures
     	glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[2]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[3]);
   

        glBindVertexArray(vao[1]);
        glDrawElements(GL_TRIANGLES, plane_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);

		glBindTexture(GL_TEXTURE_2D, 0);
		
if(display_mode == 10 || display_mode == 11){					//dragon

		homothety(35,hom);
		translation(2,-2.5,tra);
		translation(3,-1.5,tra);
		translation(1,3,tra);
		mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
		mat4fCopy(objectToWorld, objectToWorldTmp);
		mat4fMul( objectToWorldTmp, rot, objectToWorldTmp);
		mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
		glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
		dragon.Draw();
		clearTranslation(tra);
		clearRotation(rot);

		glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
		  mat4fToIdentity(objectToWorldTmp);
	
	
	
	}
	else{		//scene normale		

	// placement de la vache
	translation(3,2,tra);
	translation(1,-3,tra);
	translation(2,-1,tra);
	rotation(2,90.0,rot);
	homothety(1.0,hom);
	mat4fCopy(objectToWorld, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, rot, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	cow.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	  mat4fToIdentity(objectToWorldTmp);

	// placement du lampadaire
	//rotation(3,90.0,rot);
	translation(3,-10,tra);
	translation(2,2.3,tra);
	homothety(1,hom);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	mat4fCopy(objectToWorld, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, rot, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	streetLamp.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement de l'arbre 1
	translation(2,4,tra);
	translation(1,7,tra);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	tree.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement de l'aigrette
	homothety(0.001,hom);
	rotation(1,89.50, rot);
	translation(1,10,tra);
	translation(3,-2.5,tra);
	translation(2, 2.0, tra);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, rot, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	egret.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement de l'arbre 2
	translation(2,4,tra);
	translation(1,10,tra);
	translation(3,-7,tra);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	tree.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement de l'arbre 3
	translation(2,4,tra);
	translation(1,15,tra);
	translation(3,-4,tra);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	tree.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement du pick-up
	rotation(1,89.55, rot);
	translation(2,-0.4,tra);
	translation(1, -1.6,tra);
	translation(3,-20,tra);
	mat4fMul( objectToWorldTmp, rot, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	pickup.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement de l'helicoptere
	homothety(0.05,hom);
	rotation(1,89.35, rot);
	translation(2,8,tra);
	translation(1,6,tra);
	translation(3,-12,tra);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, rot, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	helico.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement du lapin
	homothety(0.01, hom);
	//rotation(1, 89.55, rot);
	translation(2,0.7,tra);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	//mat4fMul( objectToWorldTmp, rot, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	rabbit.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement de l'escargot
	homothety(2.0, hom);
	translation(2,-0.6,tra);
	translation(3,-5,tra);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	snail.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

// placement de la moto
	homothety(1.0, hom);
	translation(2,0.4,tra);
	translation(3,-21,tra);
	translation(1, 9, tra);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	moto.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement de la porsche
	homothety(0.7, hom);
	translation(2,0.55,tra);
	translation(3,-21,tra);
	translation(1, -25, tra);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	porsche.Draw();
	clearTranslation(tra);
	clearRotation(rot);

	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorld);
	mat4fToIdentity(objectToWorldTmp);

	// placement de l'arbre 4
	homothety(1.0, hom);
	translation(2,4.0,tra);
	translation(3,-9,tra);
	translation(1, -10, tra);
	mat4fMul( objectToWorldTmp, hom, objectToWorldTmp);
	mat4fMul( objectToWorldTmp, tra, objectToWorldTmp);
	glUniformMatrix4fv(gbuffer_objectLocation, 1, 0, objectToWorldTmp);
	tree.Draw();
	clearTranslation(tra);
	clearRotation(rot);

}
	
        // detache framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Viewport
        glViewport( 0, 0, width, height );
        camera.setViewport(0, 0, width, height);


        // nettoie le  front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
     

        // attache la couleur a l'identifiant 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[0]);        
        // attache les normales a l'identifiant 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[1]);    
        //attache la profondeur a l'identifiant 2
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gbuffer.depthTexId);      
        // attache les positions a l'identifiant 3
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[2]); 


/**********************************************************************************************************************
*						SSAO Shader							       *
 **********************************************************************************************************************/
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, textures[4]); 

 		
/**********************************************************************************************************************
*						laccum Shader							       *
 **********************************************************************************************************************/
	 if( display_mode == 1 ){		//affichage normal (scene + eclairage)
    
		   glUseProgram(laccum_shader.program);		
		    glUniform1i(laccum_materialLocation, 0);
		    glUniform1i(laccum_normalLocation, 1);
		    glUniform1i(laccum_depthLocation, 2);
		    glUniform3fv(laccum_cameraPositionLocation, 1, cameraPosition);
		    glUniformMatrix4fv(laccum_inverseViewProjectionLocation, 1, 0, iviewProjection);


		    glDisable(GL_DEPTH_TEST);
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_ONE, GL_ONE);

			numLights = 6;
       for (unsigned int i = 0; i < (int) numLights; ++i){
           float tl = 25 + (i*20);
		//float tl =t*i;
            float lightPosition[3] = { sin(tl) * 10.0, -0.5, cos(tl) * 10.0};
            float lightColor[3] = {sin(tl) *  1.0, 1.0 - cos(tl), -sin(tl)};
            float lightIntensity = 50.0 + rand() % 50 -30 ;

            glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
            glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            glUniform1f(SSAO_lightIntensityLocation, lightIntensity);
            glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0); 
        }

	for(unsigned int i = 0; i <5; i ++){
		float lightColor[3] = {1.0,1.0,1.0};
 		float lightIntensity = 30.0;
		float t = (float)i * 15;
		float lightPosition[3] = {-50+t , 0.0, -20};
  		glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
        	glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            	glUniform1f(SSAO_lightIntensityLocation, lightIntensity);

		 glBindVertexArray(vao[2]);
           	 glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0); 
	}

	float lightColor[3] = {1.0,0.0,0.0};
 		float lightIntensity = 50.0;
		float lightPosition[3] = {6.0 ,8.0, -12.0};
  		glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
        	glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            	glUniform1f(SSAO_lightIntensityLocation, lightIntensity);

		 glBindVertexArray(vao[2]);
           	 glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
glBindVertexArray(0); 


		    glDisable(GL_BLEND);
    glUseProgram(0);

    }
   
/**********************************************************************************************************************
*						BLIT Shader							       *
 **********************************************************************************************************************/

    else if( display_mode == 2){	//affichage couleurs
        glUseProgram(blit_shader.program);
        glUniform1i(blit_tex1Location, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[0]);        
     
        glBindVertexArray(vao[2]);
    	glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
     	glUseProgram(0);
    }
    else if( display_mode == 3){	//affichage normales
        glUseProgram(blit_shader.program);
        glUniform1i(blit_tex1Location, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[1]);        
      
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        glUseProgram(0);
    }
    else if( display_mode == 4){	//affichage profondeur
        glUseProgram(blit_shader.program);
        glUniform1i(blit_tex1Location, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer.depthTexId);        
        
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        glUseProgram(0);
    }
    else if( display_mode == 5){	//affichage position
        glUseProgram(blit_shader.program);
        glUniform1i(blit_tex1Location, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[2]);          
       
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
        glUseProgram(0);
    }        
/**********************************************************************************************************************
*						laccumMultiPass Shader							       *
 **********************************************************************************************************************/
else if( display_mode == 6 ){		//affichage normal (scene + eclairage) avec plusieurs passes
       /////////////////////////  Lumiere 1: obligatoire
//////////////////////////////////

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, laccumgbuffer.fbo);
        glDrawBuffers(laccumgbuffer.outCount, laccumgbuffer.drawBuffers);
            // Default states

        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
            glEnable(GL_DEPTH_TEST);

                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE);
                   glUseProgram(laccumMultiPass_shader.program);                         // Bind laccum shader
                    // Upload uniforms
                    glUniform1i(laccumMultiPass_materialLocation, 0);
                    glUniform1i(laccumMultiPass_normalLocation, 1);
                    glUniform1i(laccumMultiPass_depthLocation, 2);
                     glUniform1f(laccumMultiPass_premierrenduLocation, 1);            // premier rendu!
                    glUniform3fv(laccumMultiPass_cameraPositionLocation, 1, cameraPosition);
                    glUniformMatrix4fv(laccumMultiPass_inverseViewProjectionLocation, 1, 0,iviewProjection);

            float lightPosition[3] = { -2.0, 3.0, 0.0};
            float lightColor[3] = {1.0, 1.0, 1.0};
            float lightIntensity = 0.0 ;

            glUniform3fv(laccumMultiPass_lightPositionLocation, 1, lightPosition);
            glUniform3fv(laccumMultiPass_lightColorLocation, 1, lightColor);
            glUniform1f(laccumMultiPass_lightIntensityLocation, lightIntensity);
            glDisable(GL_BLEND);
            // Draw quad
                         glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3,GL_UNSIGNED_INT, (void*)0);
                        glBindVertexArray(0);



        glDisable(GL_BLEND);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);               // debind du framebuffer laccum
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

        /////////////////////////   Multilumiere
//////////////////////////////////

        int NumLights = 12;
        for(int i = 0; i< NumLights; i++){

                        // transfer FBO laccum -> FBO blit transfer
                glDisable(GL_DEPTH_TEST);
                glBindFramebuffer(GL_FRAMEBUFFER, transfergbuffer.fbo);
        glDrawBuffers(transfergbuffer.outCount, transfergbuffer.drawBuffers);

        glUseProgram(blit_shader.program);
        glUniform1i(blit_tex1Location, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, laccumgbuffer.colorTexId[0]);  //sortie du laccum bindée

        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT,(void*)0);
        glUseProgram(0);
                glBindFramebuffer(GL_FRAMEBUFFER, 0);           // debind du framebuffer detransfer
                glDisable(GL_DEPTH_TEST);

                        // transfer FBO blit transfer -> FBO laccum
                glUseProgram(laccumMultiPass_shader.program);                            // Bind laccum shader
                glBindFramebuffer(GL_FRAMEBUFFER, laccumgbuffer.fbo);
        glDrawBuffers(laccumgbuffer.outCount, laccumgbuffer.drawBuffers);
            // Bind color to unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[0]);
        // Bind normal to unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[1]);
        // Bind depth to unit 2
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gbuffer.depthTexId);
                    // Upload uniforms
                glUniform1i(laccumMultiPass_materialLocation, 0);
                glUniform1i(laccumMultiPass_normalLocation, 1);
                glUniform1i(laccumMultiPass_depthLocation, 2);

                 //   glEnable(GL_TEXTURE_2D);
                 glActiveTexture(GL_TEXTURE3);
         glBindTexture(GL_TEXTURE_2D, transfergbuffer.colorTexId[0]);
        //       glBindTexture(GL_TEXTURE_2D, textures[0]);
                 glUniform1i(laccumMultiPass_renderedTexturepLocation, 3);
                 glUniform3fv(laccumMultiPass_cameraPositionLocation, 1, cameraPosition);
                 glUniformMatrix4fv(laccumMultiPass_inverseViewProjectionLocation, 1, 0,iviewProjection);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

        glEnable(GL_BLEND);
                glBlendFunc(GL_ONE, GL_ONE);

                float tl = 25 + (i*20);
                        //      float tl =t*i;
        	lightPosition = { sin(tl) * 10.0, -0.5, cos(tl) * 10.0};
        	lightColor = {sin(tl) *  1.0, 1.0 - cos(tl), -sin(tl)};
                //float lightColor[3] = {1.0,1.0,1.0};
      		 lightIntensity = 50.0 ;//+ rand() % 50 -30 ;
                glUniform3fv(laccumMultiPass_lightPositionLocation, 1, lightPosition);
        glUniform3fv(laccumMultiPass_lightColorLocation, 1, lightColor);
        glUniform1f(laccumMultiPass_lightIntensityLocation, lightIntensity);
        glUniform1f(laccumMultiPass_premierrenduLocation, 0);

        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3,GL_UNSIGNED_INT, (void*)0);
                glBindVertexArray(0);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
                glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);           // debind du framebufferlaccum
        glDisable(GL_DEPTH_TEST);

        }



        glUseProgram(blit_shader.program);
        glUniform1i(blit_tex1Location, 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, laccumgbuffer.colorTexId[0]);
        // Draw quad
        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT,(void*)0);
        glUseProgram(0);


    }
/**********************************************************************************************************************
*						BloomTexture Shader							       *
 **********************************************************************************************************************/

	else if (display_mode == 7){
		       /////////////////////////  Lumiere 1: obligatoire
//////////////////////////////////

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, laccumgbuffer.fbo);
        glDrawBuffers(laccumgbuffer.outCount, laccumgbuffer.drawBuffers);
            // Default states

        // Clear the front buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
            glEnable(GL_DEPTH_TEST);

                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE);

                   glUseProgram(SSAO_shader.program);                           // Bind laccum shader
                    // Upload uniforms
                 glUniform1i( SSAO_materialLocation, 0);
                    glUniform1i( SSAO_normalLocation, 1);
                    glUniform1i( SSAO_depthLocation, 2);
                    glUniform1i( SSAO_positionLocation, 3);
                    glUniform1i( SSAO_normalmapLocation, 4);
                    glUniform3fv( SSAO_cameraPositionLocation, 1, cameraPosition);
                    glUniformMatrix4fv( SSAO_inverseViewProjectionLocation, 1, 0,
iviewProjection);
                    glUniform1f(SSAO_texturesizeXLocation, texturesize[0]);
                    glUniform1f(SSAO_texturesizeYLocation, texturesize[1]);

                numLights =1;
       for (unsigned int i = 0; i < (int) numLights; ++i){
           float tl = 25 + (i*20);
                        //      float tl =t*i;

            //Update light uniforms
            float lightPosition[3] = {0.0,0.0,6.0};
          //  float lightPosition[3] = { sin(tl) * 10.0, -0.5, cos(tl) *10.0};
           // float lightColor[3] = {sin(tl) *  1.0, 1.0 - cos(tl),-sin(tl)};
                          float lightColor[3] = {1.0,1.0,1.0};
            float lightIntensity = 50.0 ;//+ rand() % 50 -30 ;

            glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
            glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            glUniform1f(SSAO_lightIntensityLocation, lightIntensity);
                glDisable(GL_BLEND);
            // Draw quad
            glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3,GL_UNSIGNED_INT, (void*)0);
                        glBindVertexArray(0);
        }


glDisable(GL_BLEND);
    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);               // debind du framebuffer laccum
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

    glDisable(GL_DEPTH_TEST);
        glUseProgram(bloomTexture_shader.program);

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(bloomTexture_renderedLocation, 0);
        glBindTexture(GL_TEXTURE_2D, laccumgbuffer.colorTexId[0]);  //sortie du laccum bindée
                glActiveTexture(GL_TEXTURE1);
       
        glBindTexture(GL_TEXTURE_2D, gbuffer.colorTexId[0]);  // sortie dulaccum bindée
        //      glBindTexture(GL_TEXTURE_2D, textures[0]);  // sortie du laccum bindée

        glBindVertexArray(vao[2]);
        glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT,(void*)0);


	 glUseProgram(0);
}
/**********************************************************************************************************************
*						Bloom Shader							       *
 **********************************************************************************************************************/
 
  else if( display_mode == 8){ 		//affichage bloom + SSAO
    
		  glUseProgram( bloom_shader.program);				
		    glUniform1i( bloom_materialLocation, 0);
		    glUniform1i( bloom_normalLocation, 1);
		    glUniform1i( bloom_depthLocation, 2);
		    glUniform1i( bloom_positionLocation, 3);
		    glUniform1i( bloom_normalmapLocation, 4);
		    glUniform3fv( bloom_cameraPositionLocation, 1, cameraPosition);
		    glUniformMatrix4fv( bloom_inverseViewProjectionLocation, 1, 0, iviewProjection);
  		    glUniform1f(bloom_texturesizeXLocation, texturesize[0]);
		    glUniform1f(bloom_texturesizeYLocation, texturesize[1]);

		    glDisable(GL_DEPTH_TEST);
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_ONE, GL_ONE);

					numLights = 6;
       for (unsigned int i = 0; i < (int) numLights; ++i){
           float tl = 25 + (i*20);
		//float tl =t*i;
            float lightPosition[3] = { sin(tl) * 10.0, -0.5, cos(tl) * 10.0};
            float lightColor[3] = {sin(tl) *  1.0, 1.0 - cos(tl), -sin(tl)};
            float lightIntensity = 50.0;// + rand() % 50 -30 ;

            glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
            glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            glUniform1f(SSAO_lightIntensityLocation, lightIntensity);
            glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0); 
        }

	for(unsigned int i = 0; i <5; i ++){
		float lightColor[3] = {1.0,1.0,1.0};
 		float lightIntensity = 30.0;
		float t = (float)i * 15;
		float lightPosition[3] = {-50+t , 0.0, -20};
  		glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
        	glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            	glUniform1f(SSAO_lightIntensityLocation, lightIntensity);

		 glBindVertexArray(vao[2]);
           	 glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0); 
	}

	float lightColor[3] = {1.0,0.0,0.0};
 		float lightIntensity = 50.0;
		float lightPosition[3] = {6.0 ,8.0, -12.0};
  		glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
        	glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            	glUniform1f(SSAO_lightIntensityLocation, lightIntensity);

		 glBindVertexArray(vao[2]);
           	 glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0); 


		    glDisable(GL_BLEND);
		    glUseProgram(0);
		   }


 else if( display_mode == 9 ){		//affichage SSAO
    
		   glUseProgram( SSAO_shader.program);	
		    glUniform1i( SSAO_materialLocation, 0);
		    glUniform1i( SSAO_normalLocation, 1);
		    glUniform1i( SSAO_depthLocation, 2);
		    glUniform1i( SSAO_positionLocation, 3);
		    glUniform1i( SSAO_normalmapLocation, 4);
		    glUniform3fv( SSAO_cameraPositionLocation, 1, cameraPosition);
		    glUniformMatrix4fv( SSAO_inverseViewProjectionLocation, 1, 0, iviewProjection);
  		    glUniform1f(SSAO_texturesizeXLocation, texturesize[0]);
		    glUniform1f(SSAO_texturesizeYLocation, texturesize[1]);

		    glDisable(GL_DEPTH_TEST);
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_ONE, GL_ONE);

			numLights = 6;
       for (unsigned int i = 0; i < (int) numLights; ++i){
           float tl = 25 + (i*20);
		//float tl =t*i;
            float lightPosition[3] = { sin(tl) * 10.0, -0.5, cos(tl) * 10.0};
            float lightColor[3] = {sin(tl) *  1.0, 1.0 - cos(tl), -sin(tl)};
            float lightIntensity = 50.0 ;//+ rand() % 50 -30 ;

            glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
            glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            glUniform1f(SSAO_lightIntensityLocation, lightIntensity);
            glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0); 
        }

	for(unsigned int i = 0; i <5; i ++){
		float lightColor[3] = {1.0,1.0,1.0};
 		float lightIntensity = 30.0;
		float t = (float)i * 15;
		float lightPosition[3] = {-50+t , 0.0, -20};
  		glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
        	glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            	glUniform1f(SSAO_lightIntensityLocation, lightIntensity);

		 glBindVertexArray(vao[2]);
           	 glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
			glBindVertexArray(0); 
	}

	float lightColor[3] = {1.0,0.0,0.0};
 		float lightIntensity = 50.0;
		float lightPosition[3] = {6.0 ,8.0, -12.0};
  		glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
        	glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            	glUniform1f(SSAO_lightIntensityLocation, lightIntensity);

		 glBindVertexArray(vao[2]);
           	 glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0); 


		    glDisable(GL_BLEND);
		    glUseProgram(0);
		    }

/**********************************************************************************************************************
*						SSAO Shader							       *
 **********************************************************************************************************************/
 else if( display_mode == 10){	//affichage SSAO pour le dragon
 		glUseProgram( SSAO_shader.program);				
		    glUniform1i( SSAO_materialLocation, 0);
		    glUniform1i( SSAO_normalLocation, 1);
		    glUniform1i( SSAO_depthLocation, 2);
		    glUniform1i( SSAO_positionLocation, 3);
		    glUniform1i( SSAO_normalmapLocation, 4);
		    glUniform3fv( SSAO_cameraPositionLocation, 1, cameraPosition);
		    glUniformMatrix4fv( SSAO_inverseViewProjectionLocation, 1, 0, iviewProjection);
  		    glUniform1f(SSAO_texturesizeXLocation, texturesize[0]);
		    glUniform1f(SSAO_texturesizeYLocation, texturesize[1]);

		    glDisable(GL_DEPTH_TEST);
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_ONE, GL_ONE);

			numLights = 3;
       for (unsigned int i = 0; i < (int) numLights; ++i){
           float tl = 25 + (i*20);
	//	float tl =t*i;
	
            float lightPosition[3] = { sin(tl) * 5.0, 1.5, cos(tl) * 5.0};
            float lightColor[3] = {sin(tl) *  1.0, 1.0 - cos(tl), 0.};
            float lightIntensity = 30.0 ;

            glUniform3fv(SSAO_lightPositionLocation, 1, lightPosition);
            glUniform3fv(SSAO_lightColorLocation, 1, lightColor);
            glUniform1f(SSAO_lightIntensityLocation, lightIntensity);
     
            glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0); 
        }
		    glDisable(GL_BLEND);
		    glUseProgram(0);
    }

/**********************************************************************************************************************
*						laccum Shader							       *
 **********************************************************************************************************************/
 else if( display_mode == 11){	//affichage laccum pour le dragon
 		glUseProgram( laccum_shader.program);			
		    // Upload uniforms
		    glUniform1i( laccum_materialLocation, 0);
		    glUniform1i( laccum_normalLocation, 1);
		    glUniform1i( laccum_depthLocation, 2);
		    glUniform3fv( laccum_cameraPositionLocation, 1, cameraPosition);
		    glUniformMatrix4fv( laccum_inverseViewProjectionLocation, 1, 0, iviewProjection);
		    glDisable(GL_DEPTH_TEST);
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_ONE, GL_ONE);

			numLights = 3;
       for (unsigned int i = 0; i < (int) numLights; ++i){
           float tl = 25 + (i*20);
	//	float tl =t*i;
            float lightPosition[3] = { sin(tl) * 5.0, 1.5, cos(tl) * 5.0};
            float lightColor[3] = {sin(tl) *  1.0, 1.0 - cos(tl), 0.};
            float lightIntensity = 20.0 ;

            glUniform3fv(laccum_lightPositionLocation, 1, lightPosition);
            glUniform3fv(laccum_lightColorLocation, 1, lightColor);
            glUniform1f(laccum_lightIntensityLocation, lightIntensity);

            glBindVertexArray(vao[2]);
            glDrawElements(GL_TRIANGLES, quad_triangleCount * 3, GL_UNSIGNED_INT, (void*)0);
	glBindVertexArray(0); 
        }
		    glDisable(GL_BLEND);
		    glUseProgram(0);
    }



   glActiveTexture(GL_TEXTURE0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);
        glDisable(GL_DEPTH_TEST);


// affiche le nombre de FPS dans une petite fenetre en haut a droite de la fenetre openGL
	char nbfps[10];
	sprintf(nbfps, "%f", fps);
	char nbfps2[10];
	sprintf(nbfps2, "%f", fps2);
        unsigned char mbut = 0;
        int mscroll = 0;
        int mousex; int mousey;
        glfwGetMousePos(&mousex, &mousey);
        mousey = height - mousey;
        if( leftButton == GLFW_PRESS )
            mbut |= IMGUI_MBUT_LEFT;
        imguiBeginFrame(mousex, mousey, mbut, mscroll);
        int logScroll = 0;
        imguiBeginScrollArea("NB FPS", width - 110, height - 110, 100, 100, &logScroll);
	imguiValue(nbfps);
	//imguiValue(nbfps2);
        imguiEndScrollArea();
        imguiEndFrame();
        imguiRenderGLDraw(width, height); 
   	glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

	
	//affichage de l'aide
	if(help ==1){
		glActiveTexture(GL_TEXTURE0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glViewport(0, 0, width, height);
        glDisable(GL_DEPTH_TEST);
		char nbfps[10];
		sprintf(nbfps, "%f", fps);
       		unsigned char mbut = 0;
        	int mscroll = 0;
        	int mousex; int mousey;
        	glfwGetMousePos(&mousex, &mousey);
        	mousey = height - mousey;
        	if( leftButton == GLFW_PRESS )
            	mbut |= IMGUI_MBUT_LEFT;
        	imguiBeginFrame(mousex, mousey, mbut, mscroll);
	
        	int logScroll = 0;
        	imguiBeginScrollArea("Menu d'aide", width - 3*width/4, height - 3*height/4, width/2, height/2+30, &logScroll);
		imguiLabel("clic gauche : rotation de la scene");
		imguiLabel("clic milieu : deplacer la camera");
		imguiLabel("clic droit : zoom avant/arriere sur la scene");
		imguiLabel("");
		imguiLabel("F1 : scene sans SSAO");
		imguiLabel("F2 : couleur de la scene");
		imguiLabel("F3 : normales de la scene");
		imguiLabel("F4 : profondeur de la scene");
		imguiLabel("F5 : positions de la scene");		
		imguiLabel("F6 : scene avec illumination multipass");
		imguiLabel("F7 : scene avec une lumière et effet bloom");
		imguiLabel("F8 : scene avec SSAO + rehausseur de contraste");
		imguiLabel("F9 : scene avec SSAO");
		imguiLabel("Inser : Dragon sans SSAO");
		imguiLabel("Suppr : Dragon avec SSAO");
		imguiLabel("ShiftLeft : fermer le menu d'aide");
		imguiLabel("");
		imguiLabel("Bonne promenade dans notre scene !!!");
		imguiValue("E. Bonnefoy & D. Delallee - mars 2013");
		
        	imguiEndScrollArea();
        	imguiEndFrame();
        	imguiRenderGLDraw(width, height); 
   		glDisable(GL_BLEND);
        	glEnable(GL_DEPTH_TEST);

	}


        // on verifie s'il y a des erreurs openGL
        GLenum err = glGetError();
        if(err != GL_NO_ERROR)
        {
            fprintf(stderr, "OpenGL Error : %s\n", gluErrorString(err));
            
        }

        // nettoie tous les buffers
        glfwSwapBuffers();

    } // on affiche tant que la touche escape n'est pas pressee
    while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
           glfwGetWindowParam( GLFW_OPENED ) );

    // nettoie la fenetre de rendu 
    imguiRenderGLDestroy();

    // ferme la fenetre opengl
    glfwTerminate();

    exit( EXIT_SUCCESS );
}

