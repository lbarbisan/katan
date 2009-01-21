/*============================================================================
	Objet: Fonctions et type de la bibliothèque graphique
	Version: 1.5
	Auteur: Barbisan Laurent
	Date: 13/01/2001
============================================================================*/

/*============================================================================*/
/*==============================D E F I N I T I O N ==========================*/
/*============================================================================*/
typedef struct
{
	int Width;
	int Height;
	unsigned char *Bitmap;
} Bitmap;


/*============================================================================*/
/*===============================F O N C T I O N S============================*/
/*============================================================================*/

/*Initialise le mode graphique*/
short Init_Graphics();

/* Initialise un Bitmap */
short InitBitmap(Bitmap * SrcBitmap, int Height, int Width);

/*Charge un fichier Pcx dans un Bitmap*/
int loadpcx(char *nomfich, Bitmap *PcxBitmap);

/*Transferts de Bitmap */
void BlitBackBuffer(int SrcX1, int SrcY1,int Longueur, int Hauteur,Bitmap *BmpSource,int DestX, int DestY);
void Blit(int SrcX1, int SrcY1,int Longueur, int Hauteur, Bitmap *BmpSource,int DestX, int DestY, Bitmap *BmpDestination);
void BlitScreen(int SrcX1, int SrcY1,int Longueur, int Hauteur, Bitmap *BmpSource,int DestX, int DestY);
void BlitBckBufToScreen(int SrcX1, int SrcY1,int Longueur, int Hauteur,int DestX, int DestY);
void BlitFromBckBuf(int xBB, int yBB, int Longueur, int Hauteur, Bitmap * BmpDestination, int xDest, int yDest);

/* Attend le rafraîchissement de l'ecran */
void Waitretrace();

/*Configure la palette couleur */
void setpal(unsigned char coul,unsigned char r,unsigned char g,unsigned char b);

/* Efface un Bitmap*/
void clrBitmap(Bitmap *ClrBitmap);

/*Copy un Bitmap à l'ecran*/
void CopyToScreen(Bitmap *SrcBitmap);

/* Copie le BackBuffer à l'ecran */
void CopyBckBufToScreen();

/* Vide les memoires graphiques */
void CloseGraphics();

/* Affiche un texte en mode graphique */
void OutText(int X, int Y, char *Texte, unsigned char Couleur, unsigned char BackColor, Bitmap *BmpDestination);
