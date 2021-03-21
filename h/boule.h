/**
  ******************************************************************************************************************
  * @file    boule.h 
  * @author  Alfred Gaillard     IUT Informatique La Rochelle
  * @version ETAPE6
  * @date    17 mai 2019
  * @modification	rien pour le moment
  * @brief   declarations pour l'affichage des bitmap 
  ******************************************************************************************************************/


#ifndef __BOULE_H
#define __BOULE_H

#define HAUTEUR_BOULE	10
#define LARGEUR_BOULE	10
#define HAUTEUR_FOOD	10
#define LARGEUR_FOOD	10
#define GHOST_WIDTH	10
#define GHOST_HEIGHT	10
#define O GLCD_COLOR_YELLOW 
#define _ GLCD_COLOR_BLACK  
#define R GLCD_COLOR_RED 
#define G GLCD_COLOR_GREEN 
#define W GLCD_COLOR_WHITE
#define B GLCD_COLOR_BLUE
#define P GLCD_COLOR_PURPLE


extern const unsigned short imgBoule[HAUTEUR_BOULE*LARGEUR_BOULE];
extern const unsigned short imgBouleClean[HAUTEUR_BOULE*LARGEUR_BOULE];
extern const unsigned short imgPlayer[4][HAUTEUR_BOULE*LARGEUR_BOULE];
extern const unsigned short imgFruit[HAUTEUR_FOOD*LARGEUR_FOOD];
extern const unsigned short imgFruitClean[HAUTEUR_FOOD*LARGEUR_FOOD];
extern const unsigned short imgGhost[4][GHOST_HEIGHT*GHOST_WIDTH];

#endif
