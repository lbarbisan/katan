/*============================================================================
Objet: Le Jeux => Les Colans de Katan !!!							    
Fichier: Katan.cpp
Version: 1.0
Auteur: Barbisan Laurent
Date: 05/02/2002
============================================================================*/

/*============================================================================*/
/*============================== I N C L U D E S ================================*/
/*============================================================================*/
#include <stdio.h>
#include <conio.h>
#include <mem.h>
#include <alloc.h>
#include "Video.h"
#include "Mouse.h"
#include <stdlib.h>
#include <time.h>

/*============================================================================*/
/*=============================== D E F I N E S ==============================*/
/*============================================================================*/
#define	  	PLATEAU_X		20  	// Dimension en X du plateau de jeu
#define	  	PLATEAU_Y		20  	// Dimension en y du plateau de jeu

#define 	SPRITE_X		32  	// Dimensoin en x des Hexagones
#define 	SPRITE_Y		32  	// Dimensoin en y des Hexagones

#define		SCRHEXA_X		7   	// Nbre d'Hexagone affihhé à l'écran
#define		SCRHEXA_Y		10  	// Nbre d'Hexagone affiché à l'écran

#define		MAXJOUEURS      2		// Nombre de joueurs maximum

#define 	BLE				0		// Valeur du Ble
#define     ARGILE			1     	// Valeur de l'Argile
#define		FORET			2     	// Valeur de la Forêt


/*============================================================================*/
/*============================= D E F I N I T I O N S ========================*/
/*============================================================================*/


/* Enumération des actions possibles*/
enum actions { Aucune = 0, PoseMaison = 1, PoseRoute = 2};

/* Structure des ressources */
typedef int ressources[3];

/* Hexagones du jeu */
typedef struct
{
	int Ressource;
	int Numero;
} hexagone;

/* Structure d'un joueur */
typedef struct
{
	ressources Ressource;
	char *Nom;
	int NumJoueur;
	int Score;
} Joueurs;


/*============================================================================*/
/*============================== G L O B A L E S =============================*/
/*============================================================================*/

/*----Variables pour le jeu----*/

/* Hexagones du plateau  de jeu */
hexagone HexaPlateau[PLATEAU_X][PLATEAU_Y];
/* Maison sur le plateau de jeu */
int Maison[PLATEAU_X*2 + 1][PLATEAU_Y + 1];
/* Route sur le plateau de jeu */
int Route[PLATEAU_X*3 + 2][PLATEAU_Y + 1];

/*Ressources pour chaque joueur*/
Joueurs	Joueur[MAXJOUEURS];

/*----Variables pour les graphismes----*/
/*Buffer pour tout les Sprites*/
Bitmap BmpHexa;
/*Buffer pour la carte*/
Bitmap BmpCarte;
/*Buffer pour le menu*/
Bitmap BmpMenu;
/* Buffer pour les Sprite */
Bitmap BmpSprite;
/* Buffer pour les boutons */
Bitmap BmpBouttons;

/*Boutons et position de la souris*/
int Mouse_X=0,Mouse_Y=0;

/* Numéro du joueur qui est en train de jouer */
int NumJoueur = 1;

/* Fin du tour du joueur */
short TourJoueur = 1;

/* Coordonnées des maisons */
int xHouse = 0, yHouse = 0;
int xLastHouse = -1, yLastHouse = -1;
int xScrHouse, yScrHouse;

/* Coordonnées des route */
int xStreet, yStreet;
int xScrStreet, yScrStreet;

/* On peut poser une maison */
short MaisonPosable = 1;

short LastCheckOnly = 1;

/* Curseur Courant */
int Curseur = 4;

/* Action Courante */
actions Action;

/* Fin du jeu */
short EndGame = 0;


/*============================================================================*/
/*============================= P R O T O T Y P E S ==========================*/
/*============================================================================*/

/*Initialise le jeu*/
void Init_Game();

/* Condition de victoire */
int Victoire();

/*Permet de tirer 2D6*/
int Tirage2D6();

/*Vérification des ressources gagnées suivant le tirage*/
void CheckRessources(int Tirage);

/* Peut t'on poser une maison */
void DrawHouse(short Posable);
short PutHouse(short CheckOnly, short Init);

/* Peut t'on poser une route */
void DrawStreet();
short PutStreet(short CheckOnly);

/* Permet d'afficher des informations sur un joueurs */
void ShowJoueur(Joueurs InfoJoueur);

/* Vérifit que l'utilisateur n'a pas cliquer sur un boutons */
void CheckAction();


/*============================================================================*/
/*============================== F O N C T I O N S ===========================*/
/*============================================================================*/


/*----------------------------------------------------------------------------
Main()  -  Boucle Pricipale du programme
----------------------------------------------------------------------------*/
void main(void)
{
	int Tirage;				// Valeur du nombre tiré aux dés ( 2D6 )
	int Nbre_Tours = 0;		// Numéro du tour courant
	short Init = 1;			// Tour d'initialisation
	char *Valeur;			// ?

	/*Initialisation du jeu*/
	Init_Game();

	do
	{

		/*1. Selection du joueur*/
		NumJoueur = 1 - NumJoueur;

		/*2. Tirage des dés*/
		Tirage = Tirage2D6();
		OutText(220, 115, "TIRAGE:", 10,NULL,NULL);
		gcvt((double)Tirage, 3, Valeur);
		OutText(290, 115, "   ", 10, 254,NULL);
		OutText(290, 115, Valeur, 10,254,NULL);

		/* 3. Vérification des ressources gagnées suivant le tirage et ajout de ressources au joueur */
		if(Init!=1) CheckRessources(Tirage);

		/* Affiche les informations sur le joueurs */
		ShowJoueur(Joueur[NumJoueur]);

		/* Vérifit les conditions de victoires et affichage de la victoire */
		/*if(Victoire()!=0)
		{*/
			/*Bitmap BmpVictoire;
			loadpcx("Victoire.pcx", &BmpVictoire);
			BlitBackBuffer(0,0,BmpVictoire,10,10)
			do loop*/
			/*EndGame=1;
		}*/

		/*Boucle d'attente ( Et maintenant que veux faire le joueurs ) */
		TourJoueur = 1;

		do
		{
			/* Copy le backBuffer à l'écran et affiche la souris */
			Mouse_Pos(&Mouse_X, &Mouse_Y);
			gotoxy(1,1);
			Waitretrace();
			CopyBckBufToScreen();
			BlitScreen(Curseur * 32,0,32,32,&BmpHexa,Mouse_X,Mouse_Y);

			/* La souris est sur le plateau de jeu */
			if(Mouse_X<208)
			{
				switch(Action)
				{
				case PoseMaison:
					xHouse = 0; yHouse = 0;
					PutHouse((Mouse_Down()==0), Init);
					break;
				case PoseRoute:
					PutStreet(Mouse_Down()==0);
					break;
				}
			}
			/* La souris est sur le panneau de commande et le bouttons appuyé */
			else
			{
				if(Mouse_Down()==1) CheckAction();

				switch(Action)
				{
				case PoseMaison:
					if(Maison[xHouse][yHouse]==0)
						BlitBackBuffer(0,0, SPRITE_X, SPRITE_Y, &BmpSprite, xScrHouse, yScrHouse);
					clrBitmap(&BmpSprite);
					break;
				case PoseRoute:
					if(Route[xStreet + 1][yStreet]==0)
						BlitBackBuffer(0,0, SPRITE_X, SPRITE_Y, &BmpSprite, xScrStreet, yScrStreet);
					clrBitmap(&BmpSprite);
					break;
				}
			}
		} while(TourJoueur==1);

		Nbre_Tours++;
		if(Nbre_Tours==2)
			Init=0;

	} 	while(EndGame==0);

	/* libère l'espace mémoire */
	free(BmpCarte.Bitmap);
	free(BmpHexa.Bitmap);
	free(BmpSprite.Bitmap);
	free(BmpMenu.Bitmap);
	free(BmpBouttons.Bitmap);

	/* Ferme le graphique */
	CloseGraphics();

} // Fin Main


/*----------------------------------------------------------------------------*/
/* DrawHouse()  -  Déssine les maisons seulement si possible						*/
/*----------------------------------------------------------------------------*/
void DrawHouse(short Posable)
{

	unsigned char Decalage=0;

	/*Efface l'ancienne maison si il n'y en à pas */
	if(Maison[xLastHouse][yLastHouse]==0)
		BlitBackBuffer(0,0, SPRITE_X, SPRITE_Y, &BmpSprite, xScrHouse, yScrHouse);
	else
		BlitBackBuffer(128 * NumJoueur,32, SPRITE_X, SPRITE_Y, &BmpHexa, xScrHouse, yScrHouse);


	/* Calcul le décalage pour l'affichage à l'écran */
	Decalage = (1 - (xHouse & 1) ^ (yHouse & 1)) * 7;

	/* Coordonnées de la maison à l'écran */
	xScrHouse = xHouse * (SPRITE_X/2) - SPRITE_X/2;
	yScrHouse = yHouse * (SPRITE_Y - 7) + Decalage - SPRITE_Y/2;

	/* Sauvegarde l'image avant de poser la maison */
	BlitFromBckBuf(xScrHouse, yScrHouse, SPRITE_X, SPRITE_Y, &BmpSprite, 0,0);
	if(Posable==1)
		BlitBackBuffer(128 * NumJoueur,32, SPRITE_X, SPRITE_Y, &BmpHexa, xScrHouse, yScrHouse);
	else if(Maison[xLastHouse][yLastHouse]==0)
		BlitBackBuffer(128 * NumJoueur + 32, 32,SPRITE_X, SPRITE_Y, &BmpHexa, xScrHouse, yScrHouse);

}

/*----------------------------------------------------------------------------*/
/*	PutHouse()  -  Vérifit quez l'on peut poser une maison							*/
/*----------------------------------------------------------------------------*/
short PutHouse(short CheckOnly, short Init)
{

	short valid = 0;

	xLastHouse = xHouse;
	yLastHouse = yHouse;

	/* Coordonnées de la maison dans le tableau */
	xHouse =  Mouse_X/(SPRITE_X/2);
	yHouse =  Mouse_Y/(SPRITE_Y - 7);

	if((xHouse==xLastHouse) && (yHouse==yLastHouse) && (CheckOnly!=LastCheckOnly))
	{
		LastCheckOnly = CheckOnly;
		DrawHouse(MaisonPosable);
		return MaisonPosable;
	}

	LastCheckOnly = CheckOnly;

	/* Vérifit que les ressources sont suffisantes */
	if((Joueur[NumJoueur].Ressource[BLE]<1)
		|| (Joueur[NumJoueur].Ressource[ARGILE]<1)
		|| (Joueur[NumJoueur].Ressource[FORET]<1))
	{
		DrawHouse(0);
		MaisonPosable = 0;
		return 0;
	}

	/* Vérifit qu'il n'y a pas déjà une maison */
	if(Maison[xHouse][yHouse]!=0)
	{
		DrawHouse(0);
		MaisonPosable = 0;
		return 0;
	}

	/* Vérifit qu'il y a une route à coté de la maison */
	if(Init==0)
	{
		// Impaire et Impaire
		if((xHouse & 1) & (yHouse & 1))
		{
			if(Route[(xHouse -1)/2 * 3 - 1 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
			if(Route[(xHouse -1)/2 * 3 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
			if(Route[(xHouse -1)/2 * 3 + 1 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
		}
		// Paire et Paire
		else if ((1 - (xHouse & 1)) & (1 - (yHouse & 1)))
		{
			if(Route[(xHouse/2) * 3 - 1 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
			if(Route[(xHouse/2) * 3 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
			if(Route[(xHouse/2) * 3 + 1 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
		}
		// Impaire et Paire
		else if((xHouse & 1) & (1 - (yHouse & 1)))
		{
			if(Route[((xHouse - 1)/2) * 3 + 1 ][yHouse - 1]==Joueur[NumJoueur].NumJoueur) valid = 1;
			if(Route[((xHouse - 1)/2) * 3 + 1 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
			if(Route[((xHouse - 1)/2) * 3 + 2 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
		}
		// Paire et Impaire
		else
		{
			if(Route[xHouse/2 * 3 - 2 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
			if(Route[xHouse/2 * 3 - 1 + 1][yHouse]==Joueur[NumJoueur].NumJoueur) valid = 1;
			if(Route[xHouse/2 * 3 + 1][yHouse - 1]==Joueur[NumJoueur].NumJoueur) valid = 1;
		}
	}
	else valid=1;

	if(valid==0)
	{
		DrawHouse(0);
		MaisonPosable = 0;
		return 0;
	}

	/* Enlève les ressources et affiche la maison */
	if(CheckOnly==0)
	{
		Joueur[NumJoueur].Ressource[BLE] -= 1;
		Joueur[NumJoueur].Ressource[ARGILE] -= 1;
		Joueur[NumJoueur].Ressource[FORET] -= 1;
		Maison[xHouse][yHouse] = Joueur[NumJoueur].NumJoueur;
		Joueur[NumJoueur].Score += 2;
		ShowJoueur(Joueur[NumJoueur]);
	}

	/*Affiche la maison */
	MaisonPosable = 1;
	DrawHouse(1);

	return 1;

}


/*----------------------------------------------------------------------------*/
/* DrawStreet()  -  Déssine une route seulemtn si possiBLE							*/
/*----------------------------------------------------------------------------*/
void DrawStreet()
{

	unsigned char DecalageY=3;
	unsigned char DecalageX=0;
	int StreetSprite;

	/*Efface l'ancienne route si il n'y en à pas */
	if(Route[xStreet + 1][yStreet]==0)
		BlitBackBuffer(0,0, SPRITE_X, SPRITE_Y, &BmpSprite, xScrStreet, yScrStreet);

	/* Décalage pour l'affichage des routes */
	if(xStreet%3==0)
		DecalageY = SPRITE_X/2;

	DecalageX = (yStreet & 1) * 16;

	/* Change l'orientation des routes suivant la position de la route */
	/* la y un bug normalment en toute logie on devrait avoir + 1 et +2 */
	if(xStreet%3==0)
		StreetSprite = 32;
	else if((xStreet-1)%3==0)
		StreetSprite = 64;
	else if((xStreet-2)%3==0)
		StreetSprite = 92;


	/* Coordonnées de la maison à l'écran */
	xScrStreet = xStreet * SPRITE_X / 3 + DecalageX - SPRITE_X/2;
	yScrStreet = yStreet * (SPRITE_Y - 7) + DecalageY - SPRITE_Y/2;

	/* Sauvegarde l'image avnt de poser la route */
	BlitFromBckBuf(xScrStreet, yScrStreet, SPRITE_X, SPRITE_Y, &BmpSprite, 0,0);
	BlitBackBuffer(StreetSprite + 128 * NumJoueur, 32, SPRITE_X, SPRITE_Y, &BmpHexa, xScrStreet, yScrStreet);

}


/*----------------------------------------------------------------------------*/
/*	PutStreet  -  Vérifit que l'on peut poser une route								*/
/*----------------------------------------------------------------------------*/
short PutStreet(short CheckOnly)
{

	short Valide = 0;

	/* Coordonées des routes dans le taBLEau */
	yStreet =  Mouse_Y/(SPRITE_Y - 7);
	xStreet =  Mouse_X/(SPRITE_X/3) - (yStreet & 1)  ;

	/* Vérifit qu'il n'y a pas déjà une route*/
	if(Route[xStreet + 1][yStreet]!=0)
		return 0;

	/* Vérifit que les ressources sont suffisantes */
	if((Joueur[NumJoueur].Ressource[ARGILE]<2)
		|| (Joueur[NumJoueur].Ressource[FORET]<1))
		return 0;

	if(((xStreet+2)%3)==0)
	{
		if(Maison[(xStreet - 1 + 1)/3 * 2 + (yStreet & 1)][yStreet]==Joueur[NumJoueur].NumJoueur) Valide = 1;
		if(Maison[(xStreet - 1 + 1)/3 * 2 + 1 + (yStreet & 1)][yStreet]==Joueur[NumJoueur].NumJoueur) Valide = 1;
	}
	else if(((xStreet+1)%3)==0)
	{
		if(Maison[(xStreet + 1 + 1)/3 * 2 - (1 - (yStreet & 1))][yStreet]==Joueur[NumJoueur].NumJoueur) Valide = 1;
		if(Maison[(xStreet + 1 + 1)/3 * 2 + 1 - (1 - (yStreet & 1))][yStreet]==Joueur[NumJoueur].NumJoueur) Valide = 1;
	}
	else if((xStreet%3)==0)
	{
		if(Maison[(xStreet + 1)/3 * 2 + (yStreet & 1)][yStreet]==Joueur[NumJoueur].NumJoueur) Valide = 1;
		if(Maison[(xStreet + 1)/3 * 2 + (yStreet & 1)][yStreet + 1 ]==Joueur[NumJoueur].NumJoueur) Valide = 1;
	}

	if(Valide==0) return 0;

	if(CheckOnly==0)
	{
		Joueur[NumJoueur].Ressource[ARGILE] -= 2;
		Joueur[NumJoueur].Ressource[FORET] -= 1;
		Route[xStreet + 1][yStreet] = Joueur[NumJoueur].NumJoueur;
		Joueur[NumJoueur].Score += 1;
		ShowJoueur(Joueur[NumJoueur]);
	}

	/* Affiche la route */
	DrawStreet();

	return 1;

}

/*----------------------------------------------------------------------------*/
/*	Tirage2D6()  -  Tire deux dés à 6 faces												*/
/*----------------------------------------------------------------------------*/
int Tirage2D6()
{

	/* Choisie au hasard deux nombres compris entre 0 et 5 */
	return (random(6) + random(6) + 2);

}


/*----------------------------------------------------------------------------*/
/* CheckRessources()  -  Attribut au joueur des ressources suivant le tirage 	*/
/*----------------------------------------------------------------------------*/
void CheckRessources(int Tirage)
{

	int Index_X, Index_Y;
	int Index_J;


	for(Index_J=0;Index_J<MAXJOUEURS;Index_J++)
	{
		/* Fait le tour de tout les Hexagones et y regarde le nombre tiré */
		for(Index_Y=0;Index_Y<PLATEAU_Y;Index_Y++)
		{
			for(Index_X=0;Index_X<PLATEAU_X;Index_X++)
			{
				if(HexaPlateau[Index_X][Index_Y].Numero==Tirage)
				{
					if(Maison[Index_X*2 + (Index_Y & 1)][Index_Y]==Joueur[Index_J].NumJoueur)
						Joueur[Index_J].Ressource[HexaPlateau[Index_X][Index_Y].Ressource]++;
					if(Maison[Index_X*2 + 1 + (Index_Y & 1)][Index_Y]==Joueur[Index_J].NumJoueur)
						Joueur[Index_J].Ressource[HexaPlateau[Index_X][Index_Y].Ressource]++;
					if(Maison[Index_X*2 + 2 + (Index_Y & 1)][Index_Y]==Joueur[Index_J].NumJoueur)
						Joueur[Index_J].Ressource[HexaPlateau[Index_X][Index_Y].Ressource]++;
					if(Maison[Index_X*2 + (Index_Y & 1)][Index_Y + 1]==Joueur[Index_J].NumJoueur)
						Joueur[Index_J].Ressource[HexaPlateau[Index_X][Index_Y].Ressource]++;
					if(Maison[Index_X*2 + 1 + (Index_Y & 1)][Index_Y + 1]==Joueur[Index_J].NumJoueur)
						Joueur[Index_J].Ressource[HexaPlateau[Index_X][Index_Y].Ressource]++;
					if(Maison[Index_X*2 + 2 + (Index_Y & 1)][Index_Y + 1]==Joueur[Index_J].NumJoueur)
						Joueur[Index_J].Ressource[HexaPlateau[Index_X][Index_Y].Ressource]++;

				}
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
/* ShowJoueur()  -  Affiche les informations sur un joueur							*/
/*----------------------------------------------------------------------------*/
void ShowJoueur(Joueurs InfoJoueur)
{

	char *Valeur;

	OutText(220, 35, "SCORE:", 10,NULL,NULL);
	gcvt((double)InfoJoueur.Score, 3, Valeur);
	OutText(290, 35, "   ", 10, 254,NULL);
	OutText(290, 35, Valeur, 10,254,NULL);

	OutText(230, 45, "Joueur ", 10,NULL,NULL);
	gcvt((double)InfoJoueur.NumJoueur, 3, Valeur);
	OutText(290, 45, "   ", 10,254,NULL);
	OutText(290, 45, Valeur, 10,254,NULL);

	OutText(220, 65, "RESSOURCES", 10,NULL, NULL);

	OutText(230, 75, "Argile:", 10,NULL,NULL);
	gcvt((double)InfoJoueur.Ressource[ARGILE], 3, Valeur);
	OutText(290, 75, "   ", 10,254,NULL);
	OutText(290, 75, Valeur, 10,254,NULL);

	OutText(230, 85, "ble:", 10,NULL,NULL);
	gcvt((double)InfoJoueur.Ressource[BLE], 3, Valeur);
	OutText(290, 85, "   ", 10,254,NULL);
	OutText(290, 85, Valeur, 10,254,NULL);

	OutText(230, 95, "Foret:", 10,NULL,NULL);
	gcvt((double)InfoJoueur.Ressource[FORET], 3, Valeur);
	OutText(290, 95, "   ", 10,254,NULL);
	OutText(290, 95, Valeur, 10,254,NULL);

	

}

/*----------------------------------------------------------------------------*/
/* CheckAction()  -  Vérifit si la souris est sur les bouttons						*/
/*----------------------------------------------------------------------------*/
void CheckAction()
{
	/* Bouton pour poser une maison */
	if(Mouse_X>225 && Mouse_X<306 && Mouse_Y>136 && Mouse_Y<152)
	{
		if(Curseur!=5)
		{ 
			Curseur = 5; Action = PoseMaison;
			clrBitmap(&BmpSprite);
			BlitBackBuffer(81,16, 81,16, &BmpBouttons, 225, 136);
			BlitBackBuffer(0,0, 81,16, &BmpBouttons, 225, 152);
		}
		else
		{
			Curseur = 4;
			BlitBackBuffer(0,16, 81,16, &BmpBouttons, 225, 136);
		}

	}
	/* Bouton pour poser une route */
	else if(Mouse_X>225 && Mouse_X<306 && Mouse_Y>152 && Mouse_Y<168)
	{
		if(Curseur!=6)
		{ Curseur = 6; Action = PoseRoute;
		clrBitmap(&BmpSprite);
		BlitBackBuffer(81,0, 81,16, &BmpBouttons, 225, 152);
		BlitBackBuffer(0,16, 81,16, &BmpBouttons, 225, 136);}
		else {Curseur = 4;
		BlitBackBuffer(0,0, 81,16, &BmpBouttons, 225, 152);}
	}
	/* Bouton pour changer de joueur */
	else if(Mouse_X>225 && Mouse_X<306 && Mouse_Y>168 && Mouse_Y<184)
	{
		TourJoueur = 0;
	}
	/* Bouton pour quitter le jeu */
	else if(Mouse_X>225 && Mouse_X<306 && Mouse_Y>184 && Mouse_Y<200)
	{
		TourJoueur = 0;
		EndGame = 1;
	}
}





/*----------------------------------------------------------------------------*/
/*	Init_Game()  -  I N I T I A L I S A T I O N											*/
/*----------------------------------------------------------------------------*/
void Init_Game()
{
	int Index_X, Index_Y, Decalage=0;
	char *Valeur;

	/* Efface l'ecran */
	clrscr();

	/*Initialisation du mode video*/
	if(Init_Graphics()==0)
		printf("Erreur lors de l'initialisation graphique\n");

	/* Initialise l'algorithme de random */
	randomize();

	/*Création du plateau au hasard*/
	for(Index_Y=0;Index_Y<PLATEAU_Y;Index_Y++)
	{
		for(Index_X=0;Index_X<PLATEAU_X;Index_X++)
		{
			// Type du terrain sur la case courante
			HexaPlateau[Index_X][Index_Y].Ressource = random(3);
			// Nombre de resources sur la case courante
			HexaPlateau[Index_X][Index_Y].Numero = Tirage2D6();
			// Reset des elements sur la case courante
			Maison[Index_X*2][Index_Y] = 0;
			Maison[Index_X*2 + 1][Index_Y] = 0;
			Route[Index_X*3][Index_Y]  = 0;
			Route[int(Index_X*3 + 1) ][Index_Y]  = 0;
			Route[Index_X*3 + 2 ][Index_Y]  = 0;
		}
		Maison[PLATEAU_X*2][Index_Y] = 0;
	}

	/*Création du buffer pour le menu*/
	if(loadpcx("Menu.pcx", &BmpMenu)<1)
		printf("Impossible de charger Menu.Pcx\n");

	/*Création du buffer pour tout les type d'hexagone, maison, etc*/
	if(loadpcx("Sprites.pcx", &BmpHexa)<1)
		printf("Impossible de charger Sprites.Pcx\n");

	/*Création du buffer pour les boutons */
	if(loadpcx("Bouttons.pcx", &BmpBouttons)<1)
		printf("Impossible de charger Bouttons.Pcx\n");

	/*Affichage du fond */
	BlitBackBuffer(0,0,320,200,&BmpMenu,0,0);

	/* Affichage des bouttons */
	BlitBackBuffer(0,16, 81,16, &BmpBouttons, 225, 136);
	BlitBackBuffer(0,0, 81,16, &BmpBouttons, 225, 152);
	BlitBackBuffer(0,48, 81,16, &BmpBouttons, 225, 168);
	BlitBackBuffer(0,32, 81,16, &BmpBouttons, 225, 184);

	/*Création du Buffer pour la carte*/
	if(InitBitmap(&BmpCarte, 25*SCRHEXA_Y, 32*SCRHEXA_X)==0)
		printf("Impossible d'allouer la mémoire pour la carte. Pas assez de mémoire");

	/* Création du Buffer pour les Sprites */
	if(InitBitmap(&BmpSprite, SPRITE_Y, SPRITE_X)==0)
		printf("Impossible d'allouer la mémoire pour les sprites. Pas assez de mémoire");

	/*Affichage du plataeu*/
	for(Index_Y=0;Index_Y<SCRHEXA_Y;Index_Y++)
	{
		for(Index_X=0;Index_X<SCRHEXA_X;Index_X++)
		{
			Blit(HexaPlateau[Index_X][Index_Y].Ressource*32,0,32,32,&BmpHexa,Index_X*32+Decalage, Index_Y*25, &BmpCarte);
			gcvt((double)HexaPlateau[Index_X][Index_Y].Numero, 3, Valeur);
			OutText(Index_X*32+Decalage + 12, Index_Y*25 + 12, Valeur, 50,NULL, &BmpCarte);
		}
		/*Premet d'obtenir un décalage (comme les damiers) pour les hexagones*/
		Decalage = ((Decalage/16) ^ 1)*16;
	}

	BlitBackBuffer(0, 0, SPRITE_X*6+16, 200,&BmpCarte,0,0);

	Init_Mouse(320, 200);

	/* Initialisation des données joueurs */
	for(Index_X=0;Index_X<MAXJOUEURS;Index_X++)
	{
		Joueur[Index_X].Ressource[BLE] = 2;
		Joueur[Index_X].Ressource[ARGILE] = 6;
		Joueur[Index_X].Ressource[FORET] = 4;
		Joueur[Index_X].NumJoueur = Index_X + 1;
		Joueur[Index_X].Score = 0;
	}

}


/*----------------------------------------------------------------------------*/
/* Victoire()  -  Condition de victoire													*/
/*----------------------------------------------------------------------------*/
int Victoire()
{

	int Index;

	for(Index=0;Index<MAXJOUEURS;Index++)
	{
		if(Joueur[Index].Score==20)
			return Joueur[Index].NumJoueur;
	}

	return 0;

}
