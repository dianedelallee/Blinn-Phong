#include <iostream>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string>


#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "preloadPLY.hpp"
#include <iostream>



 using namespace std;
 
class Model_PLY{
 public: 
 	int   npts;
 	int nfac;        /* nb de points et nb de faces */
	POINT_PLY *points;	/* pointeur sur les points */
	FACE_PLY  *faces;	/* pointeur sur les faces */
	GLuint vaoid;
    GLuint vbo[3];
	
	float* vertexTab;
	float* normalTab;
	float* colorTab;
	
 public:
	int Load(char *filename);
	void Draw();
	Model_PLY();
 
};
 

Model_PLY::Model_PLY(){

}



int Model_PLY::Load(char* fichier){ 
	//strcat(fichier,".ply");

	ReadPly(fichier,&npts,&points,&nfac,&faces);
	
cout << "nbpoints: " <<npts<< " nfaces: "<<nfac << endl;
// nfaces = nbtriangles dans la scene. nb de coordonnées = nbtriangles *3*3
 vertexTab = (float*)malloc(nfac*3*3 * sizeof(float));
 normalTab = (float*)malloc(nfac*3*3 * sizeof(float));
 colorTab = (float*)malloc(nfac*3*3* sizeof(float));
  
int indice = 0;
   for(int i=0 ; i<nfac ; i++){
	  for(int j=0 ; j<faces->npts ; j++){
	  
	 		colorTab[indice] =  points[faces[i].ind[j]].r;
	 		colorTab[indice+1] =  points[faces[i].ind[j]].g;
	 		colorTab[indice+2] =  points[faces[i].ind[j]].b;		
	 		
	 		normalTab[indice] =  points[faces[i].ind[j]].nx;
	 		normalTab[indice+1] =  points[faces[i].ind[j]].ny;
	 		normalTab[indice+2] =  points[faces[i].ind[j]].nz;		
	  
	  
	  	 	vertexTab[indice] =  points[faces[i].ind[j]].x;
	 		vertexTab[indice+1] =  points[faces[i].ind[j]].y;
	 		vertexTab[indice+2] =  points[faces[i].ind[j]].z;		
	  		
	  		indice = indice +3;
	    }
	}


cout << "indice: " << indice << endl;

			glGenVertexArrays(1, &vaoid);
		


    glGenBuffers(3, vbo);

			// objet
			glBindVertexArray(vaoid);
			// Bind vertices and upload data
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
			//glBufferData(GL_ARRAY_BUFFER, 3*3*sizeof(Faces_Triangles)*sizeof(GLfloat),Faces_Triangles, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nfac*3*3,vertexTab, GL_STATIC_DRAW);
			// Bind normals and upload data
			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*3, (void*)0);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*nfac*3*3, normalTab, GL_STATIC_DRAW);
			
		
	free(vertexTab);
	free(colorTab);
	free(normalTab);
	return 0;
}

void Model_PLY::Draw(){

	glBindVertexArray(vaoid);
	//glDrawElements(GL_TRIANGLES, 3*TotalConnectedPoints, GL_UNSIGNED_INT, (void*)0);
	glDrawArrays(GL_TRIANGLES,0,nfac*3*3);
	//cout << "draw" << endl;
	glBindVertexArray(0); 
}




