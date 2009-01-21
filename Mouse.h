/*============================================================================*/
/*============================ P R O T O T Y P E =============================*/
/*============================================================================*/

/* Initialise la souris, avec la position spécifiee */
void Init_Mouse(int xLimite, int yLimite);

/* Retourn la position de la souris */
void Mouse_Pos(int *Mouse_X, int *Mouse_Y);

/* Retourne si le bouton gauche de la souris est appuye */
short Mouse_Down();
