/*============================================================================
Objet : Mini-Bibliothèque graphique
Version : 1.5
Remarque : C'était dur...Mais Voilà !!!
Auteurs :  Barbisan Laurent
Date : 13/01/2002
============================================================================*/


/*============================================================================*/
/*===============================I N C L U D E S==============================*/
/*============================================================================*/
#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <conio.h>
#include <mem.h>
#include <dos.h>
#include "Video.h"


/*============================================================================*/
/*================================D E F I N E S===============================*/
/*============================================================================*/
#define			SCREEN_X    	320
#define			SCREEN_Y		200
#define			TRANSPARENT_KEY 201

/*============================================================================*/
/*====================V A R I A B L E S  G L O B A L E S======================*/
/*============================================================================*/
unsigned char *Ecran   = (unsigned char *) 0xA0000000L;     	// Mémoire video
unsigned char *BackBuffer; 										//Ecran virtuel


/*============================================================================*/
/*==============================F O N C T I O N S=============================*/
/*============================================================================*/


/*----------------------------------------------------------------------------
InitGraphics()  -  Initialise le mode graphique
----------------------------------------------------------------------------*/
short Init_Graphics()
{

	/*Passage en mode 13h, soit 320*200 en 256 couleurs*/
	asm
	{
		MOV AH,0
			MOV AL,0x13	// Mode video 13h
			INT 0x10	//Interruption du Bios, Fonction 10
	}

	/* Verifie que l'on peut allouer le backBuffer */
	if((BackBuffer = (unsigned char *)malloc(64000L))==NULL)
		return 0;

	/*Vide le BackBuffer (met 0 partout)*/
	memset(BackBuffer,0,64000L);

	return 1;
}


/*----------------------------------------------------------------------------
CloseGraphics()  -  Vide la mémoire prise par le BackBuffer
----------------------------------------------------------------------------*/
void CloseGraphics()
{

	asm
	{
		MOV AH,0
			MOV AL,0x03		// Mode video 3 Mode Texte en fait
			INT 0x10		//Interruption du Bios, Fonction 10
	}

	free(BackBuffer);
}

/*----------------------------------------------------------------------------
InitBitmap()  -  Initialiset un Bitmap
----------------------------------------------------------------------------*/
short InitBitmap(Bitmap * SrcBitmap, int Height, int Width)
{

	if((SrcBitmap->Bitmap = (unsigned char *)malloc(Height*Width))==NULL)
		return 0;

	SrcBitmap->Height = Height;
	SrcBitmap->Width = Width;

	memset(SrcBitmap->Bitmap, TRANSPARENT_KEY, SrcBitmap->Height*SrcBitmap->Width);

	return 1;

}


/*----------------------------------------------------------------------------
	WaitRetrace()  -  Attend le retraçage de l'écran
----------------------------------------------------------------------------*/
void Waitretrace()
{
	/*là bon alors...En fait on attend que le canon à electron de l'écran
	finisse d'afficher l'image et qu'il revienne en diagonale
	Pourquoi on met un masque de 8 ? simplement parce que sur les bits de 0x3DA
	seulement le 3ème bit nous interresses:
	0: Retrace Vertical non activé
	1: Retrace Vertical activée */
	while (!(inp(0x3DA) & 8));
}


/*----------------------------------------------------------------------------
loadpcx()  -  Charge un fichier au format PCX
----------------------------------------------------------------------------*/
int loadpcx(char *nomfich,Bitmap *PcxBitmap)
{
	unsigned char Byte, Nb_Octets, Palette[768];
	unsigned long Taille;
	unsigned long index = 0;
	unsigned Couleur, index_rle;

	/*Chargement du fichier*/
	FILE *pcxfile;

	/* Impossible de charger le fichier...*/
	if (!(pcxfile = fopen(nomfich, "rb")))
		return 0;

	/*Lit les dimensions du fichier Pcx*/
	fseek(pcxfile,8,SEEK_SET);
	fread(&PcxBitmap->Width,2,1,pcxfile);
	fread(&PcxBitmap->Height,2,1,pcxfile);

	/*Leurs ajoute 1 (je sais pas pourquoi...C'est comme ça dans un fichier
	Pcx les dimensions son stockées avec la dimension - 1*/
	PcxBitmap->Width++;
	PcxBitmap->Height++;

	/*Déduit la taille*/
	Taille = (unsigned long)(PcxBitmap->Width) * (unsigned long)(PcxBitmap->Height);

	/* Charge la palette couleur */
	fseek(pcxfile, -768, SEEK_END);
	fread(&Palette, 768, 1, pcxfile);

	/*Configure la palette couleur*/
	for (Couleur=0;Couleur<=255;Couleur++)
		setpal(Couleur,Palette[Couleur*3]>>2,Palette[Couleur*3+1]>>2,Palette[Couleur*3+2]>>2);

	/*Lit l'entête pour arrivé aux données*/
	fseek(pcxfile, 128, SEEK_SET);

	/*Réserve la place pour le buffer du fichier en mémoire*/
	if((PcxBitmap->Bitmap = (unsigned char *)malloc(PcxBitmap->Width*PcxBitmap->Height))==NULL)
		return -1;

	/*Et là C'est l'algorithme de décompression RLE (Record Encoding Lenght ) du format PCX*/
	do
	{
		/*Lit un octet*/
		fread(&Byte, 1, 1, pcxfile);

		/*Si compris entre 192 et 255 alors il y a une trame compressée*/
		if(Byte>=192)
		{
			/*Reçoit le nombre de point à tracer de la même couleur*/
			Nb_Octets = Byte - 192;
			/*lit la couleur à tracée*/
			fread(&Byte, 1, 1, pcxfile);

			/*Affiche l'octet*/
			for (index_rle=1; index_rle<=Nb_Octets; index_rle++)
			{
				PcxBitmap->Bitmap[index++] = Byte;
			}
		}
		else PcxBitmap->Bitmap[index++] = Byte;
	} while(index < Taille);

	fclose(pcxfile);
	return 1;
}


/*----------------------------------------------------------------------------
	setpal()  -  Modifie une valeur de la palette couleur
----------------------------------------------------------------------------*/
void setpal(unsigned char coul,unsigned char r,unsigned char g,unsigned char b)
{
	outp (0x03C8,coul);		// Couleur à modifier
	outp (0x03C9,r);        // Nouvelle nuance de rouge
	outp (0x03C9,g);			// Nouvelle nuance de vert
	outp (0x03C9,b);			// Nouvelle nuance de bleu
}


/*----------------------------------------------------------------------------
	clrBitmap()  -  Efface un Bitmap
----------------------------------------------------------------------------*/
void clrBitmap(Bitmap *ClrBitmap)
{
	memset(ClrBitmap->Bitmap, TRANSPARENT_KEY , ClrBitmap->Height* ClrBitmap->Width);
}


/*----------------------------------------------------------------------------
	copyToScreen()  -  Copie un Bitmap à l'écran ( plus rapide que BlitScreen )
----------------------------------------------------------------------------*/
void copyToScreen(Bitmap *SrcBitmap)
{
	memcpy(Ecran,SrcBitmap->Bitmap,SrcBitmap->Height * SrcBitmap->Width);
}

/*----------------------------------------------------------------------------
	CopyBckBufToScreen()  -  Copie le BackBuffer à l'écran
----------------------------------------------------------------------------*/
void CopyBckBufToScreen()
{
	memcpy(Ecran,BackBuffer,64000L);
}

/*----------------------------------------------------------------------------
	BlitBackBuffer()  -  Transfert un morceau de Bitmap vers le backBuffer
----------------------------------------------------------------------------*/
void BlitBackBuffer(int SrcX1, int SrcY1,int Longueur, int Hauteur, Bitmap *BmpSource,int DestX, int DestY)
{

	if((DestX+Longueur)>SCREEN_X)
		Longueur = SCREEN_X - DestX;
	if((DestY+Hauteur)>SCREEN_Y)
		Hauteur = SCREEN_Y - DestY;

	if(DestX<0)
	{
		SrcX1 -= DestX;
		Longueur += DestX;
		DestX=0;
	}

	if(DestY<0)
	{
		SrcY1 -= DestY;
		Hauteur += DestY;
		DestY=0;
	}

	for (int y=0; y<Hauteur; y++)
		for (int x=0; x<Longueur; x++)
		{
			if((BmpSource->Bitmap[(SrcY1+y)*BmpSource->Width+x+SrcX1])!=TRANSPARENT_KEY)
				BackBuffer[(DestY+y)*SCREEN_X+x+DestX] = BmpSource->Bitmap[(SrcY1+y)*BmpSource->Width+x+SrcX1];
		}
}


/*----------------------------------------------------------------------------
	BlitScreen()  -  Transfert un morceau de Bitmap vers l'ecran
----------------------------------------------------------------------------*/
void BlitScreen(int SrcX1, int SrcY1,int Longueur, int Hauteur, Bitmap *BmpSource,int DestX, int DestY)
{

	if((DestX+Longueur)>SCREEN_X)
		Longueur = SCREEN_X - DestX;
	if((DestY+Hauteur)>SCREEN_Y)
		Hauteur = SCREEN_Y - DestY;

	if(DestX<0)
	{
		SrcX1 -= DestX;
		Longueur += DestX;
		DestX=0;
	}
	if(DestY<0)
	{
		SrcY1 -= DestY;
		Hauteur += DestY;
		DestY=0;
	}

	for (int y=0; y<BmpSource->Height-SrcY1-(BmpSource->Height-SrcY1-Hauteur); y++)
		for (int x=0; x<BmpSource->Width-SrcX1-(BmpSource->Width-SrcX1-Longueur); x++)
		{
			if((BmpSource->Bitmap[(SrcY1+y)*BmpSource->Width+x+SrcX1])!=TRANSPARENT_KEY)
				Ecran[(DestY+y)*SCREEN_X+x+DestX] = BmpSource->Bitmap[(SrcY1+y)*BmpSource->Width+x+SrcX1];
		}
}


/*----------------------------------------------------------------------------
	Blit()  -  Transfert un morceau de Bitmap vers un autre bitmap
----------------------------------------------------------------------------*/
void Blit(int SrcX1, int SrcY1,int Longueur, int Hauteur, Bitmap *BmpSource,int DestX, int DestY, Bitmap *BmpDestination)
{

	if((DestX+Longueur)>BmpDestination->Width)
		Longueur = BmpDestination->Width - DestX;
	if((DestY+Hauteur)>BmpDestination->Height)
		Hauteur = BmpDestination->Height - DestY;

	if(DestX<0)
	{
		SrcX1 -= DestX;
		Longueur += DestX;
		DestX=0;
	}
	if(DestY<0)
	{
		SrcY1 -= DestY;
		Hauteur += DestY;
		DestY=0;
	}

	for (int y=0; y<Hauteur; y++)
		for (int x=0; x<Longueur; x++)
		{
			if((BmpSource->Bitmap[(SrcY1+y)*BmpSource->Width+x+SrcX1])!=TRANSPARENT_KEY)
				BmpDestination->Bitmap[(DestY+y)*BmpDestination->Width+x+DestX] = BmpSource->Bitmap[(SrcY1+y)*BmpSource->Width+x+SrcX1];
		}
}

/*----------------------------------------------------------------------------
	BlitBckBufToScreen()  -  Transfert un morceau du BackBuffer vers l'écran
----------------------------------------------------------------------------*/
void BlitBckBufToScreen(int SrcX1, int SrcY1,int Longueur, int Hauteur,int DestX, int DestY)
{

	if((DestX+Longueur)>SCREEN_X)
		Longueur = SCREEN_X - DestX;
	if((DestY+Hauteur)>SCREEN_Y)
		Hauteur = SCREEN_Y - DestY;


	if(DestX<0)
	{
		SrcX1 -= DestX;
		Longueur += DestX;
		DestX=0;
	}
	if(DestY<0)
	{
		SrcY1 -= DestY;
		Hauteur += DestY;
		DestY=0;
	}

	for (int y=0; y<SCREEN_Y-SrcY1-(SCREEN_Y-SrcY1-Hauteur); y++)
		for (int x=0; x<SCREEN_X-SrcX1-(SCREEN_X-SrcX1-Longueur); x++)
		{
			Ecran[(DestY+y)*SCREEN_X+x+DestX] = BackBuffer[(SrcY1+y)*SCREEN_X+x+SrcX1];
		}
}

/*----------------------------------------------------------------------------
	BlitFromBckBuf()  -  Transfert une partie du BackBuffer Vers un Bitmap
----------------------------------------------------------------------------*/
void BlitFromBckBuf(int xBB, int yBB, int Longueur, int Hauteur, Bitmap * BmpDestination, int xDest, int yDest)
{

	if((xDest+Longueur)>BmpDestination->Width)
		Longueur = BmpDestination->Width - xDest;
	if((yDest+Hauteur)>BmpDestination->Height)
		Hauteur = BmpDestination->Height - yDest;

	if(xDest<0)
	{
		xBB -= xDest;
		Longueur += xDest;
		xDest=0;
	}
	if(yDest<0)
	{
		yBB -= yDest;
		Hauteur += yDest;
		yDest=0;
	}

	for (int y=0; y<Hauteur; y++)
		for (int x=0; x<Longueur; x++)
		{
			BmpDestination->Bitmap[(yDest+y)*BmpDestination->Width+x+xDest] = BackBuffer[(yBB+y)*320+x+xBB];
		}
}


/*----------------------------------------------------------------------------
	OutText()  -  Ecriture de texte à l'écran
----------------------------------------------------------------------------*/
void OutText(int X, int Y, char *Texte, unsigned char Couleur, unsigned char BackColor, Bitmap *BmpDestination)
{

	int offsetBB, offsetBD, Index, xRom, yRom;
	unsigned char far *work_char;
	unsigned char Bit_Mask;
	unsigned char *rom_char_set = (unsigned char *)0xF000FA6EL;

	for(Index=0;Index<strlen(Texte);Index++)
	{
		work_char = rom_char_set + Texte[Index]* 8;

		if(BmpDestination==NULL)
			offsetBB = Y * SCREEN_X + X + Index*8;
		else offsetBD = Y * (BmpDestination->Width) + X + Index*8;

		for(yRom=0; yRom<8; yRom++)
		{
			Bit_Mask =0x80;
			for(xRom=0;xRom<8;xRom++)
			{
				if((*work_char & Bit_Mask))
				{
					if(BmpDestination==NULL) BackBuffer[offsetBB + xRom] = Couleur;
					else BmpDestination->Bitmap[offsetBD + xRom] = Couleur;
				}
				else if(BackColor!=NULL)
				{
					if(BmpDestination==NULL) BackBuffer[offsetBB + xRom] = BackColor;
					else BmpDestination->Bitmap[offsetBD + xRom] = BackColor;
				}

				//if(BmpDestination!=NULL)
				//	getch();

				Bit_Mask = (Bit_Mask >> 1);
			}

			offsetBB += 320;
			offsetBD += BmpDestination->Width;
			work_char++;
		}
	}
}
