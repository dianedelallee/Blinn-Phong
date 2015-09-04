#ifndef __READPLY__
#define __READPLY__

/********************************
 Structures en retour
*********************************/

/* pour les sommets */
typedef struct
{
  float         x,y,z;
  float		nx,ny,nz;
  float         u,v;
  float         c;
  float         r,g,b;
} POINT_PLY;

/* pour les faces */
typedef struct
{
  unsigned char   npts;
  unsigned int    *ind;
  float			  nx,ny,nz;
} FACE_PLY;

/*********************************
 Lecture d'un fichier ply
 [in]  nom du fichier ply.
 [out] nombre de sommets (pointeur)
 [out] table des sommets (pointeur alloue en retour)
 [out] nombre de faces (pointeur)
 [out] table des faces (pointeur alloue en retour)
 *********************************/
unsigned char ReadPly(char *,int *, POINT_PLY **, int *, FACE_PLY **);

/**********************************************
 constantes symboliques a activer ou desactiver 
***********************************************/

/* affichage des operations effectuees lors du readPLY */
#define PLY_LOG
/* affichage de donnees lors du readPLY */
#define PLY_TRACE
/* activation du test de coherence */
#define PLY_COHERENCE_CHECK 

/* codes definis pour analyse du retour */
#define PLY_XYZ               0x01  /* coordonnées des sommets */
#define PLY_NORMAL            0x02  /* normale réelle */
#define PLY_TOLER             0x04  /* tolérance */
#define PLY_CONF              0x08  /* confiance */
#define PLY_COLOR             0x10  /* couleur */
#define PLY_NOTCHECKED        0x20  /* le modele a subit le test de coherence */
#define PLY_FULLCOHERENCE     0x40  /* le modele est completement coherent */

/* macros et constantes de base */
#define MIN(u,v) ((u)<(v)?(u):(v))
#define MAX(u,v) ((u)>(v)?(u):(v))
#define OFF 0
#define ON (! OFF)

#endif
