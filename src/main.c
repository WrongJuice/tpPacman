/**
  ******************************************************************************************************************
  * @file    main.c 
  * @author  Alfred Gaillard     IUT Informatique La Rochelle
  * @version ETAPE6
  * @date    17 mai 2019
  * @modification	rien pour le moment
  * @brief   ProjetPacMan fonctionnel sur des cartes MCBSTM32EXL
  ******************************************************************************************************************/

#include "stm32f10x.h"                  /* STM32F10x.h definitions            */
#include "GLCD_Config.h"
#include "boule.h"
#include "ext_globales.h"
#include "globales.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

int difficultyGhostsTimer[4] = {8 , 4 , 2 , 1}; // diviseur du timer pour augmenter la vitesse des fantômes en fonction de la difficulté
char strPlayerScore[8];	 //score du joueur en string pour l'affichage avec DrawString
unsigned int playerScore; //score du joueur en int
char strGhostScore[8]; //score des fantômes en string pour l'affichage avec DrawString
unsigned int ghostScore; //score des fantômes en int
unsigned int firstGhost = 0;		// permet de faire apparaitre le premier fantome une seule fois
unsigned int temp; //Masque etc
unsigned int direction;		// direction du PacMan
bool move = false;			// PacMan se deplace ou non
unsigned int pacMouth;		// animation bouche ouverte/fermee du pacman
bool collision = false;		// collision ou pas
int indice = 0; //indice pour les boucles for

//coordonnées des côtés (utilisés pour l'apparition des fantômes aléatoire)
int xCorner[4] = {0 , 0 , (GLCD_WIDTH - GHOST_WIDTH) , (GLCD_WIDTH - GHOST_WIDTH)};
int yCorner[4] = {14 , (GLCD_HEIGHT - GHOST_HEIGHT) , 14 , (GLCD_HEIGHT - GHOST_HEIGHT)};

// position aleatoire
int randomX = 0;	
int randomY = 0;

//Coordonnées du fruit
int XFood;	// abscisse du fruit
int YFood;	// ordonnee du fruit

//Coordonnées du PacMan
int X;	// abscisse du PacMan
int Y;	// ordonnee du Pacman
int LastX; // Ancien abscisse du PacMan
int LastY; // Ancien ordonnee du Pacman

//Coordonnées de tous les fantomes
int XGhost[4];		
int YGhost[4];
int LastXGhost[4];
int LastYGhost[4];
unsigned int nGhosts = 0;		// nombre de fantomes max
unsigned int corner;		// lieu aleatoire d'apparition des fantomes
unsigned int randomCorner = 1;
unsigned int difficultyGhosts = 1;	// niveau des fantomes
unsigned int cptTourTimer = 0;

// Declarations des JOYSTICKS
unsigned long JOYSTICK_LEFT;	
unsigned long JOYSTICK_RIGHT;
unsigned long JOYSTICK_UP;
unsigned long JOYSTICK_DOWN;

/*----------------------------------------------------------------------------
--------------------------------FONCTIONS FANTOMES----------------------------
*----------------------------------------------------------------------------*/
 
 // deplacement fantomes ETAPE 6
 void MoveGhost(int ghostNumber){
		if(XGhost[ghostNumber] == XFood){
			if(YGhost[ghostNumber] < YFood) YGhost[ghostNumber]++;
			else YGhost[ghostNumber]--;
		}else{
			if(XGhost[ghostNumber] < XFood) XGhost[ghostNumber]++;
			else XGhost[ghostNumber]--;
		}
}

//ETAPE 6
void ApparitionGhost(void){
	nGhosts++;
	corner = randomCorner;
	XGhost[playerScore] = xCorner[corner];
	YGhost[playerScore] = yCorner[corner];
	GLCD_DrawBitmap(XGhost[playerScore] , YGhost[playerScore] , GHOST_WIDTH, GHOST_HEIGHT , (const unsigned char *)imgGhost[0]);
}

//ETAPE 6
void eatGhost(void)	{	// augmente le score des fantomes et redessine le fruit
	if((XGhost[0] == XFood && YGhost[0] == YFood) || (XGhost[1] == XFood && YGhost[1] == YFood) || (XGhost[2] == XFood && YGhost[2] == YFood) || (XGhost[3] == XFood && YGhost[3] == YFood)){
		ghostScore++; //augmentation du score des fantômes
		sprintf(strGhostScore, "%d", ghostScore); //Conversion du int du score du joueur et chargement de celui-ci dans le tableau de char pour retourner un string
		GLCD_DrawBitmap(XFood , YFood , LARGEUR_FOOD , HAUTEUR_FOOD , (const unsigned char *)imgFruitClean);
		XFood = randomX;
		YFood = randomY;
		GLCD_DrawBitmap(XFood , YFood , LARGEUR_FOOD, HAUTEUR_FOOD, (const unsigned char *)imgFruit);
	}
}

/*----------------------------------------------------------------------------
--------------------------------FONCTIONS PACMAN----------------------------
*----------------------------------------------------------------------------*/

//ETAPE 1 2 ET 4
 void DeplacerPacMan(void){   // fonction pour deplacer le Pacman
	if(move == true && direction == 1 && X > 0) X--;
	if(move == true && direction == 2 && X < GLCD_WIDTH - LARGEUR_BOULE) X++;
	if(move == true && direction == 3 && Y > 16)Y --;
	if(move == true && direction == 4 && Y < GLCD_HEIGHT - HAUTEUR_BOULE) Y++;
	if(X < 0 || X > GLCD_WIDTH - LARGEUR_BOULE  || Y < 0 || Y > GLCD_HEIGHT - HAUTEUR_BOULE) move = false;
}

//ETAPE 3
void DessinerPacMan(void){	// redessine le PacMan en fonction de sa direction et l'anime (bouche ouverte/fermee)
	if(pacMouth==1) GLCD_DrawBitmap(X , Y , LARGEUR_BOULE, HAUTEUR_BOULE , (const unsigned char *)imgPlayer[direction - 1]);
	else GLCD_DrawBitmap(X , Y , LARGEUR_BOULE , HAUTEUR_BOULE , (const unsigned char *)imgBoule);
}

//ETAPE 5
void eat(void){	// augmente le score du joueur, gere le niveau et l'apparition des fantomes et redessine le fruit
	if(X + LARGEUR_BOULE > XFood && X < XFood + LARGEUR_FOOD && Y + LARGEUR_BOULE > YFood && Y < YFood + LARGEUR_FOOD){
		playerScore++;
		difficultyGhosts++;
		cptTourTimer = 0;
		ApparitionGhost();
		sprintf(strPlayerScore, "%d", playerScore); //Conversion du int du score du joueur et chargement de celui-ci dans le tableau de char pour retourner un string
		TIM4->CR1 |= 0x0001;
		TIM4->CNT |= 0;
		TIM5->CR1 |= 0x0001;
	  	TIM5->CNT |= 0;
		GLCD_DrawBitmap(XFood , YFood, LARGEUR_FOOD , HAUTEUR_FOOD , (const unsigned char *)imgFruitClean);
		XFood = randomX;
		YFood = randomY;
		GLCD_DrawBitmap(XFood , YFood , LARGEUR_FOOD , HAUTEUR_FOOD , (const unsigned char *)imgFruit);
	}
}

//ETAPE 6
void Colision(void){	// determine si il y a collision entre le Pacman et un fantome
	if(X + LARGEUR_BOULE > XGhost[0] && X < XGhost[0] + GHOST_WIDTH && Y + LARGEUR_BOULE > YGhost[0] && Y < YGhost[0] + GHOST_WIDTH)
		collision = true;
	if(X + LARGEUR_BOULE > XGhost[1] && X < XGhost[1] + GHOST_WIDTH && Y + LARGEUR_BOULE > YGhost[1] && Y < YGhost[1] + GHOST_WIDTH)
		collision = true;
	if(X + LARGEUR_BOULE > XGhost[2] && X < XGhost[2] + GHOST_WIDTH && Y + LARGEUR_BOULE > YGhost[2] && Y < YGhost[2] + GHOST_WIDTH)
		collision = true;
	if(X + LARGEUR_BOULE > XGhost[3] && X < XGhost[3] + GHOST_WIDTH && Y + LARGEUR_BOULE > YGhost[3] && Y < YGhost[3] + GHOST_WIDTH)
		collision = true;
}

//ETAPE 5
void RamdomPopFood(void){	// position aleatoire du fruit
	if(randomX > GLCD_WIDTH - LARGEUR_BOULE) randomX = 0;
	else randomX ++;
	if(randomY > GLCD_HEIGHT - HAUTEUR_BOULE) randomY = 16;
	else randomY ++;
}

//ETAPE 5 ET 6
void RamdomCoin(void){ //Alternance du corner de spawn des fantomes
	if(randomCorner == 4) randomCorner = 1;
	else randomCorner ++;
}

/*----------------------------------------------------------------------------
-------------------------FONCTIONS PARAMETRAGE PORTS/TIMER--------------------
*----------------------------------------------------------------------------*/

 void Enable_GPIO(void){
	RCC->APB2ENR |= (1<<8);  // portG
	RCC->APB2ENR |= (1<<5);  // portD
	RCC->APB2ENR |= (1<<2);  // portA
	RCC->APB2ENR |= (1<<11); // Timer1
	RCC->APB1ENR |= (1<<1); // Timer3
  	RCC->APB1ENR |= (1 << 2); // Timer4
	RCC->APB1ENR |= (1 << 3); // Timer5
	RCC->APB2ENR |= (1<<0);  // AFIO
}

void Init_GPIO(void){
	GPIOG->CRH |= 0x44400000;		// input mode sur tout les JOYSTICKS
	GPIOD->CRL |= 0x00004000;   // input mode sur JOYSTICK_DOWN
	temp = GPIOA->CRL & 0xFFF0FFFF;
	GPIOA->CRL = temp | 0x00030000; //Mode=0b00 et CNF=0b01
  	JOYSTICK_LEFT = GPIOG->IDR & 0x4000;					
  	JOYSTICK_RIGHT = GPIOG->IDR & 0x2000;	
	JOYSTICK_UP = GPIOG->IDR & 0x8000;
	JOYSTICK_DOWN = GPIOD->IDR & 0x0008;
}

//ETAPE 2 4
void cfgGPIO(void){
	// JOYSTICK-LEFT
	SETENA1 |= (1<<8);
	temp = AFIO_EXTICR4 & 0xF0FF;
	AFIO_EXTICR4 = temp | 0x0600;
	EXTI->IMR |= (1<<14);
	EXTI->RTSR |= (1<<14);
	// JOYSTICK RIGHT
	temp = AFIO_EXTICR4 & 0xFF0F;
	AFIO_EXTICR4 = temp | 0x0060;
	EXTI->IMR |= (1<<13);
	EXTI->RTSR |= (1<<13);
	// JOYSTICK DOWN
	SETENA0 |= (1<<9);
	temp = AFIO_EXTICR1 & 0x0FFF;
	AFIO_EXTICR1 = temp | 0x3000;
	EXTI->IMR |= (1<<3);
	EXTI->RTSR |= (1<<3);
	// JOYSTICK UP
	temp = AFIO_EXTICR4 & 0x0FFF;
	AFIO_EXTICR4 = temp | 0x6000;
	EXTI->IMR |= (1<<15);
	EXTI->RTSR |= (1<<15);
}

//ETAPE 1 3 5
void config_Timer(void){
	// Timer pour la vitesse de deplacement
	TIM1->ARR = 16000;  	
	TIM1->PSC = 45; // 100hz
	TIM1->DIER |= 0x0001;
	TIM1->CR1 |= 0x0001;
	SETENA0 |= (1<<25);  // autorise les interruptions sur le Timer1
	// Timer pour la frequence d'affichage des sprites bouche ouverte/bouche fermée
	TIM3->ARR =4000;   
	TIM3->PSC =2250;    // 5hz
	TIM3->DIER |= 0x0001;
	TIM3->CR1 |= 0x0001;
	SETENA0 |= (1<<29);	//activation de l'interruption pour Timer3
	// Son après avoir croqué le fruit
	TIM4->ARR =9000; // 3000khz
	TIM4->PSC =14;
	TIM4->CR1 |= 0x0001;
	TIM4->DIER|=0x0001;
	SETENA0 |= (1<<30);//activation de l'interruption pour Timer4
	TIM5->ARR =64000;
	TIM5->PSC =850;  //5hz
	TIM5->CR1 |= 0x0001;
	TIM5->DIER |= 0x0001;
	SETENA1 |= (1<<18);//activation de l'interruption pour Timer5
}

/*----------------------------------------------------------------------------
------------------------------ROUTINES D'INTERRUPTION-------------------------
*----------------------------------------------------------------------------*/

//ETAPE 1 6
void TIM1_UP_TIM10_IRQHandler(void){  // interruption sur le Timer1
	if(TIM1->SR & UIF){
		DeplacerPacMan();
		if(cptTourTimer == difficultyGhostsTimer[difficultyGhosts]){
			MoveGhost(0);
			if(playerScore >= 1)
				MoveGhost(1);
			if(playerScore >= 2)
				MoveGhost(2);
			if(playerScore >= 3)
				MoveGhost(3);
			cptTourTimer = 0;
		}
		cptTourTimer++;
	}
	RamdomPopFood();
	RamdomCoin();
	TIM1->SR &= ~UIF;
}

//ETAPE 3
void TIM3_IRQHandler(void){  // interruption sur Timer3
	if (TIM3->SR & UIF){
		TIM3->SR &= ~UIF;
		pacMouth ^= 1;
	}
}


void TIM4_IRQHandler(void){  // interruption sur Timer4
	if (TIM4->SR & 0x0001){
		TIM4->SR &= ~0x0001;
		GPIOA->ODR ^= (1<<4);
	}
}

void TIM5_IRQHandler(void){  // interruption sur Timer5
	if (TIM5->SR & 0x0001){
		TIM5->SR &= ~0x0001;
		TIM5->CR1 &= ~0x0001; 
		TIM4->CR1 &= ~0x0001;
	}
}

//ETAPE 2 4
void EXTI15_10_IRQHandler(void){
	// interruption JOYSTICK_LEFT (PG14)
	if(EXTI->PR & (1<<14)){
		EXTI->PR |= (1<<14);
		direction = 1;
		move = true;
	}
	// interruption JOYSTICK_UP (PG15)
	if(EXTI->PR & (1<<15)){
		EXTI->PR |= (1<<15);
		direction = 3;
		move = true;
	}
	// interruption JOYSTICK_RIGHT (PG13)
	if(EXTI->PR & (1<<13)){
		EXTI->PR |= (1<<13);
		direction = 2;
		move = true;
	}	
}

//ETAPE 2 4
void EXTI3_IRQHandler(void){
	// interruption JOYSTICK_DOWN (PD3)
	if(EXTI->PR & (1<<3)){
		EXTI->PR |= (1<<3);
		direction = 4;
		move = true;
	}
}

int main(void){

	sprintf(strPlayerScore, "%d", playerScore);  //Conversion du int du score du joueur et chargement de celui-ci dans le tableau de char pour retourner un string
	sprintf(strGhostScore, "%d", ghostScore);  //Conversion du int du score du fantôme et chargement de celui-ci dans le tableau de char pour retourner un string
	
 /* Initialize graphical LCD display  */
  	GLCD_Initialize();                          
  	GLCD_SetBackgroundColor(GLCD_COLOR_BLACK); /* couleur du fond */
	GLCD_SetForegroundColor(GLCD_COLOR_WHITE);  /* couleur du texte en arriere plan */

/* Clear graphical LCD display */
	GLCD_ClearScreen();               
	
	X = 160;
	LastX = X;		
	Y = 115;
	LastY = Y;
	XFood = 200;
	YFood = 50;
	GLCD_DrawBitmap(X , Y , LARGEUR_BOULE, HAUTEUR_BOULE , (const unsigned char *)imgBoule); // afficher le pacman
	GLCD_DrawBitmap(XFood , YFood , LARGEUR_FOOD, HAUTEUR_FOOD , (const unsigned char *)imgFruit); // afficher food
	GLCD_FrameBufferAccess(true); 	// supprimer limitations d'acces a la SRAM (anciennes cartes) sinon les fonctions GLCD_DrawPixel et GLCD_ReadPixel ne fonctionnent pas

/*GPIO / Timer */												
	Enable_GPIO();																
 	Init_GPIO();
	config_Timer();
	cfgGPIO();
	
	while(1){		
		GLCD_SetFont(&GLCD_Font_6x8);				// affichage du score et de la scene
		GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
		GLCD_DrawString(5,0,"Player score :");
		GLCD_DrawString(120,0,"Ghost score :");
		GLCD_DrawString(230,0,"PuckMan : AG AD");
		GLCD_DrawHLine(0,13,GLCD_WIDTH);
		GLCD_DrawString(90,0,strPlayerScore);
		GLCD_DrawString(200,0,strGhostScore);
		
		if(firstGhost == 0)	{ // apparition du premier fantome une seule fois
			ApparitionGhost();
			firstGhost = 1;
		}
		
		for (indice = 0; indice != 4; indice++)
		{
			if(nGhosts >= indice){
				if(LastXGhost[indice] != XGhost[indice] || LastYGhost[indice] != YGhost[indice]){ // nettoie derrière le fantome :)
					GLCD_DrawBitmap(LastXGhost[indice] , LastYGhost[indice], GHOST_WIDTH, GHOST_HEIGHT, (const unsigned char *)imgBouleClean);
					GLCD_DrawBitmap(XGhost[indice] , YGhost[indice], GHOST_WIDTH, GHOST_HEIGHT, (const unsigned char *)imgGhost[indice]);
					LastXGhost[indice] = XGhost[indice];
					LastYGhost[indice] = YGhost[indice];
				}
			}
		}
		
		if(LastX != X || LastY != Y){ // nettoie derrière le PacMan :)
			GLCD_DrawBitmap(LastX,LastY, LARGEUR_BOULE, HAUTEUR_BOULE, (const unsigned char *)imgBouleClean);
			DessinerPacMan();
			LastX = X;
			LastY = Y;
		}

		Colision();

		eat();

		eatGhost();									
		  
/*----------------------------------------------------------------------------
---------------------------------ECRAN DE FIN---------------------------------
*----------------------------------------------------------------------------*/

//ETAPE 6

		if(collision == true){	// Defaite si collision avec un fantome
			GLCD_ClearScreen();
			while(collision == true){
				GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
				GLCD_DrawString(74,80,"Game Over, you hit a ghost !");
				GLCD_DrawString(85,150,"Press reset to try again");
				GLCD_DrawHLine(70,75,172);
				GLCD_DrawHLine(70,162,172);
				GLCD_DrawVLine(70,75,87);
				GLCD_DrawVLine(242,75,87);
			}
		}
		
		if(playerScore == 4){ // Victoire si le joueur atteint un score de 4
			GLCD_ClearScreen();
			while(playerScore == 4){
				GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
				GLCD_DrawString(80,80,"Congratulations, you win !");
				GLCD_DrawString(85,150,"Press RESET to try again");
				GLCD_DrawHLine(75,75,162);
				GLCD_DrawHLine(75,162,162);
				GLCD_DrawVLine(75,75,87);
				GLCD_DrawVLine(237,75,87);
			}
		}
		
		if(ghostScore == 4){  // Defaite si les fantomes atteignent un score de 4
			GLCD_ClearScreen();
			while(ghostScore == 4){
				GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
				GLCD_DrawString(90,80,"Ghosts win, you loose !");
				GLCD_DrawString(85,150,"Press RESET to try again");
				GLCD_DrawHLine(80,75,152);
				GLCD_DrawHLine(80,163,152);
				GLCD_DrawVLine(80,75,88);
				GLCD_DrawVLine(232,75,88);
			}
		}
	}
}
