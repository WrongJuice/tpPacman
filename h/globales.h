/**
  ******************************************************************************************************************
  * @file    globales.h 
  * @author  PCo     IUT Informatique La Rochelle
  * @version v1.1
  * @date    06 mai 2019
  * @brief   déclarations des variables globales
  ******************************************************************************************************************/


#ifndef __GLOBALES_H
#define __GLOBALES_H
#define APPUYE 0x0000
#define UIF (1<<0)
#define UIE (1<<0)
#define SETENA0 *(volatile unsigned long *)0xE000E100
#define SETENA1 *(volatile unsigned long *)0xE000E104
#define AFIO_EXTICR1 *(volatile unsigned long *)0x40010008
#define AFIO_EXTICR4 *(volatile unsigned long *)0x40010014


#endif
