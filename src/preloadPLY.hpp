#include <math.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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




/* constantes symboliques et macro internes */
#define PLY_VERSION "2.0 beta"

/* constantes symboliques */
#define NFIELDS    5
#define PLYFLD_COORD  0
#define PLYFLD_CONF   1
#define PLYFLD_COLOR  2
#define PLYFLD_UV     3
#define PLYFLD_NORM   4

#define PLYFMT_OFF     0
#define PLYFMT_UCHAR   1
#define PLYFMT_USHORT  2 
#define PLYFMT_INT     3
#define PLYFMT_FLOAT   4
#define PLYFMT_DOUBLE  5


#define PLY_SKIP 1

#define PLYERR_INVALIDINDICES 0x01
#define PLYERR_UNCONNECTEDPTS 0x02
#define PLYERR_UNSHAREDGES    0x04
#define PLYERR_REVFACETS      0x08 
#define PLYERR_INVNORMALS     0x10
#define PLYERR_BADNORMALS     0x20
#define PLYERR_WARNING        0x40
#define PLYERR_FATAL          0x80

#define PLY_FAILED  0
#define PLY_SUCCESS (! PLY_FAILED)

#define PLY_LITTLE 1
#define PLY_BIG    0
#define PLY_UNDEF  2

#define PLY_BINARY 0
#define PLY_ASCII  1

/* Proportion entre les orientations incorrectes de normales pour décider d'inverser
   256 : n'inverse que les normales des objets totalement cohérents.
   2   : inverse les normales des objets incohérents si le nombre de normale inversée
         est supérieure à 2 fois à le nombre de normales non inversées.
   le calcul actuel utilise 14 points répartis sur une sphère autour de l'objet.
*/
#define PLY_INVLIMIT  3

/* macros */
#define SWAPBYTE(u) ( ((0x000000ff & (u)) << 24) | \
					  ((0x0000ff00 & (u)) << 8)  | \
					  ((0x00ff0000 & (u)) >> 8)  | \
					  ((0xff000000 & (u)) >> 24) )
#define SWAPFLOAT(x) { v=(int *)(&x); *v=SWAPBYTE(*v); } /* declarer [int *v] */
#ifdef PLY_LOG
#define LOG0(str)          printf(str);
#define LOG1(fmt,v1)       printf(fmt,v1);
#define LOG2(fmt,v1,v2)    printf(fmt,v1,v2);
#define LOG3(fmt,v1,v2,v3) printf(fmt,v1,v2,v3);
#else
#define LOG0(str)
#define LOG1(fmt,v1)
#define LOG2(fmt,v1,v2)
#define LOG3(fmt,v1,v2,v3)
#endif
/* attention : dans les read : on passe l'adresse */
#ifdef __sun__
#define READld(x) { fscanf (ptf, "%s", line); *(x) = atoi (line); }
#define READd(x) { fscanf (ptf, "%s", line); *(x) = atoi (line); }
#define READu(x) { fscanf (ptf, "%s", line); *(x) = atoi (line); }
#define READf(x) { fscanf (ptf, "%s", line); *(x) = atof (line); }
#else
#define READld(x) fscanf (ptf, "%ld", x)
#define READd(x) fscanf (ptf, "%d", x)
#define READu(x) { fscanf (ptf, "%u", &v); *x=v; } /* declarer [int v] */
#define READf(x) fscanf (ptf, "%f", x)
#endif

/****************************************************************
 Internal structure for coherence check
*****************************************************************/
typedef struct	 /* for coherence check only */ 
{
  unsigned int    a,b;  // indices
  int             s;    // 1 if unchanged, -1 if inversed.
} EDGE;

/****************************************************************
 Prototypes for internal use
*****************************************************************/
#ifndef __linux__
/*La fonction ceil() arrondit x par excès à l'entier le plus proche,
 * qui est renvoyé sous forme de réel (double).
 */
double ceil (double);

/* La fonction rint() arrondie la valeur x à l'entier le plus proche.
 */
double rint (double);
#endif

/*Test if the local int encoding is big or little endian
 *  return 0 if little, 
 *         1 if big, 
 *  Note:
 *    big endian:    x1234 is stored 12 in memory a+1,and 34 in memory a
 *    little endian: x1234 is stored 34 in memory a+1,and 12 in memory a
 *    endian means: what is the end (little or big) of 
 *                  the 16bits number which is stored at the starting address
 *    big endian: IBM Power PC, Hp PARisc, Sun Sparc
 *    little endian: Intel i386, DEC alpha
 *    network encoding is big endian
 */
int PLY_which_endian(void);

/* fonction de tri des sommets pour test de coherence */
int PLY_EdgeSort(const void*, const void*);
/* Fonction principale de test de coherence */
unsigned char PLY_CoherenceCheck(int, POINT_PLY*, int, FACE_PLY*);
/* affichage sous forme binaire d'un masque */
void PLY_PrintMask(char *, unsigned char);
/* recherche du point le plus loin sur le modele */
float PLY_MaxBound(int, POINT_PLY *);
/* teste si les normales sont bien orientees vers l'exterieur de la surface */
unsigned char PLY_NormalCheck (int, POINT_PLY *, float, float *);
/* affiche un message d'erreur et sort */
void PLY_FatalError (char *, char *);
/* trace affiche lorsque le mode trace est actif */
void PLY_TraceSampleData(int, POINT_PLY *, int, FACE_PLY *, int, int *);
/* lecture de l'entete du fichier ply */
unsigned char PLY_ScanPlyHeader(FILE *, int *, int *, int *, int *, int *, char *, int *);
/* lecture des donnees d'un fichier ply en mode ASCII */
unsigned char PLY_ReadAsciiPlyData(FILE *, int, POINT_PLY *, int, FACE_PLY *, int, int *);
/* lecture des donnees d'un fichier ply en mode binaire */
unsigned char PLY_ReadBinaryPlyData(FILE *, int, POINT_PLY *, int, FACE_PLY *, int, int *, char);
/* inverse les normales aux faces ainsi que le sens de parcours */
void PLY_FullFaceNormalInvert(int, FACE_PLY *);
/* inverse les normales si necessaire */
void PLY_InvertFaceNormalsIfNeeded(int, POINT_PLY *, int, FACE_PLY *, char *);
/* inverse les normales */
void PLY_InvertNormals(int, POINT_PLY *, int, FACE_PLY *, char *);
/* calcul des normales */
void PLY_ProcessNormals(int, POINT_PLY *, int, FACE_PLY *, char *);

/****************************************************************/

#ifndef __linux__
/*La fonction ceil() arrondit x par exces a l'entier le plus proche,
 * qui est renvoye sous forme de reel (double).
 */
double
ceil (double x)
{
  int res = (int) x + 1;
  return (double) res;
}

/* La fonction rint() arrondie la valeur x a l'entier le plus proche.
 */
double
rint (double x)
{
  int res = (int) (x + 0.5);
  return (double) res;
}
#endif

/*Test if the local int encoding is big or little endian
 *  return 0 if little, 
 *         1 if big, 
 *  Note:
 *    big endian:    x1234 is stored 12 in memory a+1,and 34 in memory a
 *    little endian: x1234 is stored 34 in memory a+1,and 12 in memory a
 *    endian means: what is the end (little or big) of 
 *                  the 16bits number which is stored at the starting address
 *    big endian: IBM Power PC, Hp PARisc, Sun Sparc
 *    little endian: Intel i386, DEC alpha
 *    network encoding is big endian
 */
int PLY_which_endian (void)
{
  union
  {
    unsigned short int s;
    char c[2];
  }
  un;

  un.s = 0x0102;
  if ((un.c[0] == 1) && (un.c[1] == 2))
    return PLY_BIG; /* big endian */
  else return PLY_LITTLE; /* little endian */
}

void PLY_PrintMask(char *msg, unsigned char m) {
  char str[9]="00000000";
  int i;
  for(i=0;i<8;i++) {
    if (m & 0x01) str[7-i]='1';
    m = m >> 1;
  }
  printf("%s : %s\n",msg,str);
}


int PLY_EdgeSort (const void *x, const void *y)
{
  if (((EDGE *) x)->a > ((EDGE *) y)->a)
    return 1;
  else if (((EDGE *) x)->a < ((EDGE *) y)->a)
    return -1;
  else
    {
      if (((EDGE *) x)->b > ((EDGE *) y)->b)
	return 1;
      else if (((EDGE *) x)->b < ((EDGE *) y)->b)
	return -1;
      else
	return 0;
    }
}

/* bornes maximales */
float PLY_MaxBound(int npoints, POINT_PLY * points) {
  int     i;
  float   dM = 0.0;
#ifdef PLY_SQRMAXBOUND
  float   d;
  for(i=0;i<npoints;i++) {
    d = points[i].x * points[i].x + points[i].y * points[i].y + points[i].z * points[i].z;
    if (d>dM) dM=d;
  }
  return sqrt(dM);
#else
  for(i=0;i<npoints;i++) {
    if (fabs(points[i].x) > dM) dM=fabs(points[i].x);
    if (fabs(points[i].y) > dM) dM=fabs(points[i].y);
    if (fabs(points[i].z) > dM) dM=fabs(points[i].z);
  }
  return dM;
#endif
}


/* test si l'orientation des normales est correcte 
   amelioration possible de la robustesse : comment avant (le plus loin dans
   une direction donnee) mais en centrant autour de barycentre.
*/
unsigned char PLY_NormalCheck (int npoints, POINT_PLY * points,
			   float dM, float *P) {
  int      j, j0;
  float    d, dm,bx,by,bz;

  /* renormalisation du point externe */
  d = sqrt(P[0]*P[0] + P[1]*P[1] + P[2]*P[2]);
  P[0] *= dM/d;
  P[1] *= dM/d;
  P[2] *= dM/d;

  /* calcul du barycentre */
  bx = by = bz = 0.0;
  for(j=0;j<npoints;j++) {
    bx += points[j].x;
    by += points[j].y;
    bz += points[j].z;
  }
  bx /= npoints;
  by /= npoints;
  bz /= npoints;

  /* recherche du point le plus loin du barycentre */
  j0=0;
  dm = 0.f;
  for(j=0;j<npoints;j++) {
    d = (bx - points[j].x) * (bx - points[j].x)
      + (by - points[j].y) * (by - points[j].y)
      + (bz - points[j].z) * (bz - points[j].z);
    if (d > dm) {
      dm = d;
      j0 = j;
    }
  }
   
  /* on l'a : regarde le produit scalaire */
  d = (points[j0].x - bx) * points[j0].nx
    + (points[j0].y - by) * points[j0].ny
    + (points[j0].z - bz) * points[j0].nz;

  /* retour */
  /* LOG1("Normal orientation check : %s\n",(d>0 ? "Success" : "Failed")); */
  if (d>0) return PLY_SUCCESS;
  else     return PLY_FAILED;
}

/* Main coherence check function */
unsigned char PLY_CoherenceCheck (int npoints, POINT_PLY * points, int nfaces, FACE_PLY * faces) {
  register int i;
  int j, p, q, ngood=0, nbad=0;
  int nedges;
  unsigned char *cor;
  EDGE *edge;
  unsigned char     status=0;
  float  dM;

  /* allocation */
  cor = (unsigned char *) calloc (npoints, sizeof(unsigned char));

  LOG0("Coherence check:\n");

  /* Level 1: vertex indice is out of bounds */
  for(i=0;i<nfaces;i++) {
    for (j=0; j<faces[i].npts; j++)
      if (faces[i].ind[j] >= npoints) {
	status |= PLYERR_INVALIDINDICES | PLYERR_FATAL;
	return status;
      } else cor[ faces[i].ind[j] ]++;
  }

  /* Level 2: if a defined point doesn't belong to a face */
  p = 0;
  for (i = 0; i < npoints; i++)
    if (cor[i] == 0) p++;
  LOG1("\tUnconnected points: %d (vertex belonging to no face)\n", p);
  if (p!=0) status |= PLYERR_UNCONNECTEDPTS | PLYERR_WARNING;

  /* Level 3&4: holes in the surface (brute force method) */
  /* number of edges */
  nedges=0;
  for (i=0;i<npoints;i++) nedges += cor[i];
  edge = (EDGE *) malloc (sizeof (EDGE) * nedges);

  /* filling edge array */
  p = 0;
  for (i = 0; i < nfaces; i++) {
    for (j = 0; j < faces[i].npts - 1; j++) {
      edge[p].a = faces[i].ind[j];
      edge[p].b = faces[i].ind[j + 1];
      edge[p].s = 1;
      p++;
    }
    edge[p].a = faces[i].ind[j];
    edge[p].b = faces[i].ind[0];
    edge[p].s = 1;
    p++;
  }

  /* first indice point always smaller */
  for (i = 0; i < nedges; i++)
    if (edge[i].a > edge[i].b) {
      p = edge[i].a;
      edge[i].a = edge[i].b;
      edge[i].b = p;
      edge[i].s = -1;
    }

  /* sorting */
  qsort (edge, nedges, sizeof (EDGE), PLY_EdgeSort);
  
  /* finding holes */
  p = 0;			// number of unshared edges
  q = 0;			// number of inverted facets
  i = 0;
  while (i + 1 < nedges) {
    if ((edge[i].a != edge[i + 1].a) || (edge[i].b != edge[i + 1].b)) {
      i++;
      p++;
    } else {
      if (edge[i].s == edge[i + 1].s)
	q++;
      i += 2;
    }
  }
  if (i + 1 == nedges) p++;

  LOG1("\tUnshared edges    : %d (imply holes)\n", p);
  if (p!=0) status |= PLYERR_UNSHAREDGES | PLYERR_WARNING;
  LOG1("\tInverted facets   : %d (imply wrong facets orientations)\n", q);
  if (q!=0) status |= PLYERR_REVFACETS | PLYERR_WARNING;

  /* level 5 : orientation des normales */
#define NTEST 1
  float PTEST[14][3]={ 
    {+1,-1,-1},{+1,-1,+1},{+1,+1,+1},{+1,+1,-1},
    {-1,-1,-1},{-1,-1,+1},{-1,+1,+1},{-1,+1,-1},
    {0,0,1},{0,0,-1},{0,1,0},{0,-1,0},{1,0,0},{-1,0,0} };

  /* distance plus loin que le plus loin des points */
  dM = 1.5 * PLY_MaxBound(npoints,points);

  /* renormalisation des points externes */
  for(i=0;i<NTEST;i++) {
    /* renormalisation du point et mise a distance */
    if (PLY_NormalCheck(npoints,points,10*dM,PTEST[i]) == PLY_SUCCESS) ngood++;
    else nbad++;
  }
  
  LOG3("\tNormal tests      : %s (Good=%d, Bad=%d)\n",(nbad==0 ? "Success" : "Failed"),ngood,nbad);
  if (nbad != 0) {
    if (ngood <= nbad / PLY_INVLIMIT) status |= PLYERR_INVNORMALS | PLYERR_WARNING;
    else {
      if (status!=0) status |= PLYERR_BADNORMALS | PLYERR_WARNING;
    }
  }

  /* desallocation */
  free (cor);
  free (edge);

  return status;
}


/* Fatal error managing */
void PLY_FatalError (char *mess, char *what)
{
  fprintf (stderr, "Error: %s : %s\n", mess, what);
  exit (1);
}


void PLY_TraceSampleData(int npoints, POINT_PLY *points, int nfaces, FACE_PLY *faces, int nfields, int *pfield ) {
  int i,j;
  /* quelques points lus avec leurs coordonnees */
  printf("TRACE : 10 first vertex\n");
  for (i = 0; i < MIN(10,npoints); i++) {
    printf ("%5d: ", i);
    for (j = 0; j < nfields; j++) {
      if (pfield[PLYFLD_COORD] == j)
	printf ("\tcoord:<%f,%f,%f>\n", points[i].x, points[i].y, points[i].z);
      else if (pfield[PLYFLD_CONF] == j)
	printf ("\tconf :%e\n", points[i].c);
      else if (pfield[PLYFLD_COLOR] == j)
	printf ("\tcolor:<%f %f %f>\n", points[i].r, points[i].g, points[i].b);
      else if (pfield[PLYFLD_UV] == j)
	printf ("\tcolor:<%f %f>\n", points[i].u, points[i].v);
      else if (pfield[PLYFLD_NORM] == j)
	printf ("\tnorm :<%f,%f,%f>\n", points[i].nx, points[i].ny, points[i].nz);
    }
  }
  /* quelques facettes lues avec leurs coordonnees */
  printf("TRACE : 10 first polygons\n");
  for (i = 0; i < MIN(10,nfaces); i++) {
    printf ("%5d : ", i);
    for (j = 0; j < faces[i].npts; j++)
      printf ("%u ", faces[i].ind[j]);
    printf ("\n");
  }
}

/*********************************
 Scan Ply file header
**********************************/
unsigned char PLY_ScanPlyHeader(FILE *ptf, 
		   int *fileFMT, int *plyFMT, 
		   int *npoints, int *nfaces,
		   int *nfields, char *field, int *pfield) {
  int  i;
  char line[512],fmt;
  
  fscanf (ptf, "%s", line);
  if (strcmp (line, "ply")) {
    LOG0("ScanPlyHeader ERROR : Invalid input file\n");
    return PLY_FAILED;
  }
  
  fscanf (ptf, "%s", line);
  if (strcmp (line, "format")) {
    LOG0("ScanPlyHeader ERROR : Invalid input file (line 2 must contains format)\n");
    return PLY_FAILED;
  }

  /* format du fichier */
  fscanf (ptf, "%s", line);
  if (strcmp (line, "binary_big_endian") == 0) {
    *fileFMT = PLY_BIG;
    *plyFMT  = PLY_BINARY;
    LOG0("File format    : binary & big-endian\n");
  } else {
    if (strcmp (line, "binary_little_endian") == 0) {
      *fileFMT = PLY_LITTLE;
      *plyFMT = PLY_BINARY;
      LOG0("File format    : binary & little-endian\n");
    } else {
      if (strcmp (line, "ascii") == 0) {
	*fileFMT = PLY_UNDEF;
	*plyFMT = PLY_ASCII;
	LOG0("File format    : ASCII\n");
      } else {
	LOG0("ScanPlyHeader ERROR : Unknown file format\n");
	return PLY_FAILED;
      }
    }
  }

  /* nombre de points */
  do
    fscanf (ptf, "%s", line);
  while (strcmp (line, "vertex"));
  READd(npoints);
  LOG1("Vertex count   : %d\n", *npoints);

  /* nombre de champs */
  do
    fscanf (ptf, "%s", line);
  while (strcmp (line, "property"));
  LOG0("Reading fields :\n");
  (*nfields) = 0;
  do {
    fscanf (ptf, "%s", line);	/* skipping type : on ne devrait pas */
    if      (! strcmp(line,"uchar"  )) fmt = PLYFMT_UCHAR;
    else if (! strcmp(line,"uint8"  )) fmt = PLYFMT_UCHAR;
    else if (! strcmp(line,"uint16" )) fmt = PLYFMT_USHORT;
    else if (! strcmp(line,"int"    )) fmt = PLYFMT_INT;
    else if (! strcmp(line,"int32"  )) fmt = PLYFMT_INT;
    else if (! strcmp(line,"float"  )) fmt = PLYFMT_FLOAT;
    else if (! strcmp(line,"float64")) fmt = PLYFMT_DOUBLE;
    else fmt = PLYFMT_OFF;

    fscanf (ptf, "%s", line);
    if (!strcmp (line, "x")) {
      field[PLYFLD_COORD] = fmt;
      pfield[PLYFLD_COORD] = (*nfields)++;
      for (i = 0; i < 7; i++)
	fscanf (ptf, "%s", line);
      LOG0("\t(x,y,z) coordinates\n");
    } else if (!strcmp (line, "confidence")) {
      field[PLYFLD_CONF] = fmt;
      pfield[PLYFLD_CONF] = (*nfields)++;
      fscanf (ptf, "%s", line);
      LOG0("\tconfidence\n");
    } else if (!strcmp (line, "red") || !strcmp (line, "r")) {
      field[PLYFLD_COLOR] = fmt;
      pfield[PLYFLD_COLOR] = (*nfields)++;
      for (i = 0; i < 7; i++)
	fscanf (ptf, "%s", line);
      LOG0("\tcolor\n");
    } else if (!strcmp (line, "u")) {
      field[PLYFLD_UV] = fmt;
      pfield[PLYFLD_UV] = (*nfields)++;
      for (i = 0; i < 4; i++)
	fscanf (ptf, "%s", line);
      LOG0("\t(u,v)\n");
    } else if (!strcmp (line, "nx")) {
      field[PLYFLD_NORM] = fmt;
      pfield[PLYFLD_NORM] = (*nfields)++;
      for (i = 0; i < 7; i++)
	fscanf (ptf, "%s", line);
      LOG0("\t(nx,ny,nz) normals\n");
    } else {
      LOG1("ScanPlyHeader ERROR : Unknown field (%s)\n",line);
      return PLY_FAILED;
    }
  } while (!strcmp (line, "property"));
    
  /* skipping ... */
  while (strcmp (line, "element"))
    fscanf (ptf, "%s", line);
  fscanf (ptf, "%s", line);

  /* nombre de facettes */
  READd(nfaces);
  LOG1("Faces count    : %d\n", *nfaces);
 
  /* skipping to the end ... */
  do
    fscanf (ptf, "%s", line);
  while (strcmp (line, "end_header"));

  return PLY_SUCCESS;
}

/*********************************
read ascii PLY data
**********************************/
unsigned char PLY_ReadAsciiPlyData(FILE *ptf, int npoints, POINT_PLY *points, int nfaces, FACE_PLY *faces, int nfields, int *pfield ){
#ifdef __sun__
  char line[512];
#else
  unsigned int v;
#endif
  int i,j;
  
  /* lecture des points */
  for (i = 0; i <npoints; i++) {
    for (j = 0; j < nfields; j++) {
      if (pfield[PLYFLD_COORD] == j) {
	READf( &(points[i].x) );
	READf( &(points[i].y) );
	READf( &(points[i].z) );
      } else if (pfield[PLYFLD_CONF] == j) {
	READf( &(points[i].c) );
      } else if (pfield[PLYFLD_COLOR] == j) {
	unsigned char c;
	READu(&c);
	points[i].r = (float)c / (float)255;
	READu(&c);
	points[i].g = (float)c / (float)255;
	READu(&c);
	points[i].b = (float)c / (float)255;
      } else if (pfield[PLYFLD_UV] == j) {
	READf( &(points[i].u) );
	READf( &(points[i].v) );
      } else if (pfield[PLYFLD_NORM] == j) {
	READf( &(points[i].nx) );
	READf( &(points[i].ny) );
	READf( &(points[i].nz) );
      }
    }
  }

  /* lecture des faces */
  for (i = 0; i < nfaces; i++) {
    READu( &(faces[i].npts) );
    faces[i].ind = (unsigned int *) malloc (sizeof (unsigned int) * (faces[i].npts));
    for (j = 0; j < faces[i].npts; j++)
      READu( &(faces[i].ind[j]) );
  }

  return PLY_SUCCESS;
}


/*********************************
read ascii PLY data
**********************************/
unsigned char PLY_ReadBinaryPlyData(FILE *ptf, int npoints, POINT_PLY *points, int nfaces, FACE_PLY *faces, int nfields, int *pfield, char needswap) {
  char line[512];
  int i,j;
  int *v;

  /* skip */
  if (PLY_SKIP > 0) fread (line, sizeof (char), PLY_SKIP, ptf);
  LOG1("Swapbyte : swap byte %s needed.\n",( needswap ? "IS" : "NOT") );

  /* lecture des points */
  for (i = 0; i <npoints; i++) {
    for (j = 0; j < nfields; j++) {
      if (pfield[PLYFLD_COORD] == j) {
	fread( &(points[i].x), sizeof(float), 1, ptf);
	fread( &(points[i].y), sizeof(float), 1, ptf);
	fread( &(points[i].z), sizeof(float), 1, ptf);
	if (needswap) {
	  SWAPFLOAT( points[i].x );
	  SWAPFLOAT( points[i].y );
	  SWAPFLOAT( points[i].z );
	}
      } else if (pfield[PLYFLD_CONF] == j) {
	fread( &(points[i].c), sizeof(float), 1, ptf);
	if (needswap)
	  SWAPFLOAT( points[i].c );
      } else if (pfield[PLYFLD_COLOR] == j) {
	unsigned char c;
	fread( &c, sizeof(unsigned char), 1, ptf);
	points[i].r = (float)c / (float)255;
	fread( &c, sizeof(unsigned char), 1, ptf);
	points[i].g = (float)c / (float)255;
	fread( &c, sizeof(unsigned char), 1, ptf);
	points[i].b = (float)c / (float)255;
      } else if (pfield[PLYFLD_UV] == j) {
	fread( &(points[i].u), sizeof(float), 1, ptf);
	fread( &(points[i].v), sizeof(float), 1, ptf);
	if (needswap) {
	  SWAPFLOAT( points[i].u );
	  SWAPFLOAT( points[i].v );
	}
      } else if (pfield[PLYFLD_NORM] == j) {
	fread( &(points[i].nx), sizeof(float), 1, ptf);
	fread( &(points[i].ny), sizeof(float), 1, ptf);
	fread( &(points[i].nz), sizeof(float), 1, ptf);
	if (needswap) {
	  SWAPFLOAT( points[i].nx );
	  SWAPFLOAT( points[i].ny );
	  SWAPFLOAT( points[i].nz );
	}
      }
    }
  }

  /* lecture des faces */
  for (i = 0; i < nfaces; i++) {
    fread( &(faces[i].npts), sizeof(unsigned char), 1, ptf);
    faces[i].ind = (unsigned int *) malloc (sizeof (unsigned int) * (faces[i].npts));
    for (j = 0; j < faces[i].npts; j++) {
      fread( &(faces[i].ind[j]), sizeof(int), 1, ptf);
      if (needswap) faces[i].ind[j] = SWAPBYTE( faces[i].ind[j] );
    }
  }
  return PLY_SUCCESS;
}

/* inverse des normales aux faces et du sens de parcours */
void PLY_FullFaceNormalInvert(int nfaces, FACE_PLY *faces) {
  int            i,j;
  int            n,sw;
  for (i=0; i<nfaces; i++) {
    faces[i].nx = -faces[i].nx;
    faces[i].ny = -faces[i].ny;
    faces[i].nz = -faces[i].nz;
    /* les normales aux faces etant calcules par produit vectoriel, le sens de
       parcours des faces etait donc faux: on inverse aussi le sens de parcours */
    n = faces[i].npts;
    for(j=0;j<n/2;j++) {
      sw = faces[i].ind[j];
      faces[i].ind[j] = faces[i].ind[n-1-j];
      faces[i].ind[n-1-j] = sw;
    }
  }
}

/* test et inverse les normales aux faces si elles sont incorrectes */
void PLY_InvertFaceNormalsIfNeeded(int npoints, POINT_PLY *points, int nfaces, FACE_PLY *faces, char *field) {
  int i,j;  
  float  v;
  unsigned char  s=0;

  /* regarde si pour la premiere face, le nombre de sommets pour lequel la normale
     est inversee par rapport a la normale de la face */
  for(j=0;j<faces[0].npts;j++) {
    i = faces[0].ind[j];
    v = faces[0].nx * points[i].nx + faces[0].ny * points[i].ny +
      faces[0].nz * points[i].nz;
    if (v>0) s++;
  }

  if (s==0) { /* normales point-face incoherentes, on inverse */
    PLY_FullFaceNormalInvert(nfaces,faces);
    LOG0("Normal processing : [vertex normal in PLY] face normals inverted.\n");
  } else {
    LOG0("Normal processing : [vertex normal in PLY] face normals already correct.\n");
  }
}

/*
  Le test d'inversion des normales a ete evalue sur les sommets.
*/
void PLY_InvertNormals(int npoints, POINT_PLY *points, int nfaces, FACE_PLY *faces, char *field) {
  int  i;
  /* les normales aux sommets sont toujours inverses dans cette fonction */
  for(i=0;i<npoints;i++) {
    points[i].nx = -points[i].nx;
    points[i].ny = -points[i].ny;
    points[i].nz = -points[i].nz;   
  }
  LOG0("Normal processing : vertex normals inverted.\n");
  /* inverse les normales aux faces si besoin */
  if (field[PLYFLD_NORM] == PLYFMT_OFF) {
    /* dans ce cas, normales et sommets sont necessairement dans le meme sens
       puisque calculees ensembles */
    PLY_FullFaceNormalInvert(nfaces,faces);
    LOG0("Normal processing : face normals inverted.\n");
  } else { 
    /* cas particulier: les normales n'ont peut-etre pas besoin d'etre inversees */
    PLY_InvertFaceNormalsIfNeeded(npoints,points,nfaces,faces,field);
  }
}

/*********************************
calcul des normales
(faces et sommets)
**********************************/
void PLY_ProcessNormals(int npoints, POINT_PLY *points, int nfaces, FACE_PLY *faces, char *field) {
  int i, j;
  float lg;
  float vec1[3], vec2[3];
  
  /* Normales aux faces */
  for (i=0; i<nfaces; i++) {
    vec1[0] = points[ faces[i].ind[0] ].x - points[ faces[i].ind[1] ].x;
    vec1[1] = points[ faces[i].ind[0] ].y - points[ faces[i].ind[1] ].y;
    vec1[2] = points[ faces[i].ind[0] ].z - points[ faces[i].ind[1] ].z;
    
    vec2[0] = points[ faces[i].ind[2] ].x - points[ faces[i].ind[1] ].x;
    vec2[1] = points[ faces[i].ind[2] ].y - points[ faces[i].ind[1] ].y;
    vec2[2] = points[ faces[i].ind[2] ].z - points[ faces[i].ind[1] ].z;
    
    faces[i].nx = vec2[1] * vec1[2] - vec2[2] * vec1[1];
    faces[i].ny = vec2[2] * vec1[0] - vec2[0] * vec1[2];
    faces[i].nz = vec2[0] * vec1[1] - vec2[1] * vec1[0];
    
    lg = sqrt( faces[i].nx * faces[i].nx + 
	       faces[i].ny * faces[i].ny + 
	       faces[i].nz * faces[i].nz);
    if (lg>0) {
      faces[i].nx /= lg;
      faces[i].ny /= lg;
      faces[i].nz /= lg;
    }
  }
  
  /* Normales aux sommets */
  if (field[PLYFLD_NORM] == PLYFMT_OFF) { /* pas de normales associées aux sommets */
    /* initialisation des normales aux points */
    for (i = 0; i < npoints; i++) {
      points[i].nx = 0.0f;
      points[i].ny = 0.0f;
      points[i].nz = 0.0f;
    }
    
    /* normales aux points */
    for (i=0; i<nfaces; i++)
      for (j=0; j<faces[i].npts; j++) {
	points[ faces[i].ind[j] ].nx += faces[i].nx;
	points[ faces[i].ind[j] ].ny += faces[i].ny;
	points[ faces[i].ind[j] ].nz += faces[i].nz;
      }
    
    /* normalisation des normales aux points */
    for (i=0; i<npoints; i++) {
      lg = sqrt( points[i].nx * points[i].nx +
		 points[i].ny * points[i].ny +
		 points[i].nz * points[i].nz );
      if (lg > 0.0) {
	points[i].nx /= lg;
	points[i].ny /= lg;
	points[i].nz /= lg;
      }
    }
  }
}

/*********************************
 Read a Ply file. 
 Input  :
 name   : filename of the PLY file
 npoints: (pointer) number of points
 points : (pointer) array of points (return allocated points)

 nfaces : (pointer) number of faces
 faces  : (pointer) array of faces (return allocated faces) 
**********************************/
unsigned char ReadPly (char *name, int *npoints, POINT_PLY ** points, int *nfaces, FACE_PLY ** faces) {
  FILE *ptf;
  char field[NFIELDS] = { PLYFMT_OFF, PLYFMT_OFF, PLYFMT_OFF, PLYFMT_OFF, PLYFMT_OFF };
  int nfields, pfield[NFIELDS] = { -1, -1, -1, -1, -1 };
  int machFMT, fileFMT, plyFMT;
  unsigned char s;

  LOG1("ReadPLY %s\n",PLY_VERSION);

  machFMT = PLY_which_endian();
  LOG1("Machine format : %s-endian\n",(machFMT == PLY_BIG ? "BIG" : "little"));
  LOG1("Input PLY file : %s\n", name);

  ptf = fopen (name, "rb");
  if (!ptf) PLY_FatalError ("(ReadPly) Cannot open file", name);
  
  /* scan du l'entete du fichier PLY */
  s = PLY_ScanPlyHeader(ptf,&fileFMT,&plyFMT,npoints,nfaces,&nfields,field,pfield);
  if (s == PLY_FAILED) PLY_FatalError("(ReadPly)","invalid PLY header");
  
  /* allocating memory */
  *points = (POINT_PLY *) malloc (sizeof (POINT_PLY) * (*npoints));
  *faces = (FACE_PLY *) malloc (sizeof (FACE_PLY) * (*nfaces));

  /* lecture des donnees */
  switch(plyFMT) {
  case PLY_ASCII:
    PLY_ReadAsciiPlyData(ptf,*npoints,*points,*nfaces,*faces,nfields,pfield);
    break;
  case PLY_BINARY:
    PLY_ReadBinaryPlyData(ptf,*npoints,*points,*nfaces,*faces,nfields,pfield,machFMT ^ fileFMT);
    break;
  }

  /* fin de lecture du fichier */
  fclose (ptf);

  /* calcul des normales */
  PLY_ProcessNormals(*npoints,*points,*nfaces,*faces,field);

  /* test de coherence si necessaire */
#ifdef PLY_COHERENCE_CHECK
  s = PLY_CoherenceCheck (*npoints,*points,*nfaces,*faces);
  if (s & PLYERR_INVNORMALS)
    PLY_InvertNormals(*npoints,*points,*nfaces,*faces,field);
  else if (field[PLYFLD_NORM] != PLYFMT_OFF)
    PLY_InvertFaceNormalsIfNeeded(*npoints,*points,*nfaces,*faces,field);
  /* flag de sortie */
  if ((s & (0xff ^ PLYERR_INVNORMALS ^ PLYERR_WARNING)) == 0) s=PLY_FULLCOHERENCE;
  else s=0;
#else
  {
    float P[3]={1,1,1},dM;
    dM = 10 * PLY_MaxBound(*npoints,*points);
    s = PLY_NormalCheck(*npoints,*points,dM,P);
    if (s == PLY_FAILED) PLY_InvertNormals(*npoints,*points,*nfaces,*faces,field);
    else if (field[PLYFLD_NORM] == PLYFMT_OFF)
      PLY_InvertFaceNormalsIfNeeded(*npoints,*points,*nfaces,*faces,field);
    /* flag de sortie */
    s = PLY_NOTCHECKED;
  }
#endif

  /* mise à jour des flags */
  if (field[PLYFLD_COORD] != PLYFMT_OFF) s = s | PLY_XYZ;
  if (field[PLYFLD_CONF ] != PLYFMT_OFF) s = s | PLY_CONF;
  if (field[PLYFLD_COLOR] != PLYFMT_OFF) s = s | PLY_COLOR;
  if (field[PLYFLD_UV   ] != PLYFMT_OFF) s = s | PLY_TOLER;
  if (field[PLYFLD_NORM ] != PLYFMT_OFF) s = s | PLY_NORMAL;

#ifdef PLY_TRACE
  /* quelques sommets et polygones pour les traces */
  PLY_TraceSampleData(*npoints,*points,*nfaces,*faces,nfields,pfield);
#endif

  LOG0("ReadPLY completed.\n\n");
  return s;
}  

