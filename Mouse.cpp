/*============================================================================
Objet: Gestion de la souris
Fichier: Mouse.cpp
Version: 2.0
Auteur: Barbisan Laurent
Date: 25/02/2002
============================================================================*/

/*============================================================================*/
/*============================== I N C L U D E S =============================*/
/*============================================================================*/
#include "Mouse.h"

/*============================================================================*/
/*=============================== D E F I N E S ==============================*/
/*============================================================================*/
/* Adresse de la fonctions souris du bios */
#define MOUSE_INT                0x33

/*============================================================================*/
/*======================V A R I A B L E S  G L O B A L E S====================*/
/*============================================================================*/
short Front = 0;


/*============================================================================*/
/*============================= F O N C T I O N S ============================*/
/*============================================================================*/

/*----------------------------------------------------------------------------
Init_Mouse()  -  Initialise la souris
----------------------------------------------------------------------------*/
void Init_Mouse(int xLimite, int yLimite)
{

	/* Réinitialise le driver souris */
	asm
	{
		mov ax, 0x00
		int MOUSE_INT
	}

	/*Définit les limites de déplacement en x de la souris*/
	asm
	{
		mov ax,0x07 		// Fonction de limite en X
		mov cx,0
		mov dx, [xLimite]
		int MOUSE_INT
	}

	/*Définit les limites de déplacement en y de la souris*/
	asm
	{
		mov ax,0x08			// Fonction de limite en Y
		mov cx,0
		mov dx,[yLimite]
		int MOUSE_INT
	}
}

/*----------------------------------------------------------------------------
Mouse_Pos()  -  Mise à jour la position de la souris
----------------------------------------------------------------------------*/
void Mouse_Pos(int *Mouse_X, int *Mouse_Y)
{
	int xAsm, yAsm;

	asm
	{
		MOV AX, 0x03
		INT MOUSE_INT
		MOV [xAsm], CX
		MOV [yAsm], DX
	}

	*Mouse_Y = yAsm;
	*Mouse_X = xAsm;
}


/*----------------------------------------------------------------------------*/
/* Mouse_Down()  -  Vérifit si un clique se produit									*/
/*----------------------------------------------------------------------------*/
short Mouse_Down()
{

	int AsmBoutons=0, Boutons = 0;

	/* Prend la valeur du bouton */
	asm
	{
		MOV AX, 0x03
		INT MOUSE_INT
		MOV [AsmBoutons], BX
	}

	/* Evite de cliquer plusieurs fois alors que l'on a appuyé qu'une
	seul fois sur le bouton de la souris */
	Boutons=0;
	if(AsmBoutons!=1) Front = 1;
	if((AsmBoutons==1) && (Front==1)) { Boutons=1; Front=0; }

	return Boutons;
}

