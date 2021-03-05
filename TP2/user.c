/*****************************************************************************/
/* File:        user.c (Khepera Simulator)                                   */
/* Author:      Olivier MICHEL <om@alto.unice.fr>                            */
/* Date:        Thu Sep 21 14:39:05 1995                                     */
/* Description: example of user.c file                                       */
/*                                                                           */
/* Copyright (c) 1995                                                        */
/* Olivier MICHEL                                                            */
/* MAGE team, i3S laboratory,                                                */
/* CNRS, University of Nice - Sophia Antipolis, FRANCE                       */
/*                                                                           */
/* Permission is hereby granted to copy this package for free distribution.  */
/* The author's name and this copyright notice must be included in any copy. */
/* Commercial use is forbidden.                                              */
/*****************************************************************************/

#include "../SRC/include.h"
#include "user_info.h"
#include "user.h"

#define FORWARD_SPEED   4                    /* normal (slow) forward speed*/
#define TURN_SPEED      4                    /* normal (slow) turn speed */
#define COLLISION_TH    900                  /* value of IR sensors to be 
                                                  considered as collision */
int pas=0;

/* courbe des 3 cas */
float tresproche[1024];
float proche[1024];
float pasproche[1024];

/*vitesse angulaire tres proche */
float vitesseturnright[100];
float vitesseturnleft[100];

/* courbe vitesse angulaire proche */
float vitesseturnrightp[100];
float vitesseturnleftp[100];

/* courbe vitesse angulairepour pas proche */
float vitesseturnrightpp[100];
float vitesseturnleftpp[100];

/*courbe vitesse angulaire demi-tour*/
float vitesseturnarround[100];

void DrawStep()
{
  char text[256];

  sprintf(text,"step = %d",pas);
  Color(GREY);
  UndrawText(200,100,"step = 500");
  Color(BLUE);
  DrawText(200,100,text);
}

void UserInit(struct Robot *robot){
  for (int i = 0; i < 1024; ++i)
  {
    if (i<300)
    {
      pasproche[i]=(float)-1/3*i+100;
      proche[i]=0;
      tresproche[i]=0;
    }
    else if (i>=300 && i<600){
      pasproche[i]=0;
      proche[i]=(float)1/3*i-100;
      tresproche[i]=0;
    }
    else if (i>=600 && i<900)
    {
      pasproche[i]=0;
      proche[i]=(float)-1/3*i+300;
      tresproche[i]=0;
    }
    else{
      pasproche[i]=0;
      proche[i]=0;
      tresproche[i]=(float)100/123*i-731.707;
    }
  }
  for (int i = -50; i < 50; ++i)
  {
    if (i<0)
    {
      vitesseturnright[i+50]=-2*i;
      vitesseturnleft[i+50]=0;
    }
    else{
      vitesseturnright[i+50]=0;
      vitesseturnleft[i+50]=2*i;
    }
    vitesseturnrightp[i+50] = vitesseturnright[i+50]/2;
    vitesseturnleftp[i+50] = vitesseturnleft[i+50]/2;

    vitesseturnrightpp[i+50] = vitesseturnright[i+50]/4;
    vitesseturnleftpp[i+50] = vitesseturnleft[i+50]/4;
  }
  for (int i = -50; i < 50; ++i)
  {
    if (i>45)
      vitesseturnarround[i+50] = 100;
    else
      vitesseturnarround[i+50] =0;
  }
}

void UserClose(struct Robot *robot){
}

void NewRobot(struct Robot *robot){
  pas = 0;
}

void LoadRobot(struct Robot *robot,FILE *file){
}

void SaveRobot(struct Robot *robot,FILE *file){
}

void RunRobotStart(struct Robot *robot){
  ShowUserInfo(2,1);
}

void RunRobotStop(struct Robot *robot){
  ShowUserInfo(1,1);
}


boolean StepRobot(struct Robot *robot){
  pas++;
  DrawStep();
    
  float baricentre=0, trapeze, poids=0;
  
/* Baricentre pour tresproche */
  float capteur = fmax(tresproche[robot->IRSensor[1].DistanceValue],tresproche[robot->IRSensor[0].DistanceValue]);
  for (int i = -50; i < 50; ++i)
   {
     if (vitesseturnright[i+50] > capteur)
     {
       trapeze = capteur;
     }
     else{
      trapeze = vitesseturnright[i+50];
     }
     poids += trapeze;
     baricentre += trapeze*i;
   } 

  capteur = fmax(tresproche[robot->IRSensor[5].DistanceValue],tresproche[robot->IRSensor[4].DistanceValue]);
   for (int i = -50; i < 50; ++i)
   {
     if (vitesseturnleft[i+50] > capteur)
     {
       trapeze = capteur;
     }
     else{
      trapeze = vitesseturnleft[i+50];
     }
     poids += trapeze;
     baricentre += trapeze*i;
   }

  /* Baricentre pour proche */
  capteur = fmax(tresproche[robot->IRSensor[1].DistanceValue],tresproche[robot->IRSensor[0].DistanceValue]);
  for (int i = -50; i < 50; ++i)
   {
     if (vitesseturnrightp[i+50] > capteur)
     {
       trapeze = capteur;
     }
     else{
      trapeze = vitesseturnrightp[i+50];
     }
     poids += trapeze;
     baricentre += trapeze*i;
   } 

  capteur = fmax(tresproche[robot->IRSensor[5].DistanceValue],tresproche[robot->IRSensor[4].DistanceValue]);
   for (int i = -50; i < 50; ++i)
   {
     if (vitesseturnleftp[i+50] > capteur)
     {
       trapeze = capteur;
     }
     else{
      trapeze = vitesseturnleftp[i+50];
     }
     poids += trapeze;
     baricentre += trapeze*i;
   }

   /* Baricentre pour demi-tour */
  capteur = fmin(proche[robot->IRSensor[3].DistanceValue],proche[robot->IRSensor[2].DistanceValue]);
  for (int i = -50; i < 50; ++i)
   {
     if (vitesseturnarround[i+50] > capteur)
     {
       trapeze = capteur;
     }
     else{
      trapeze = vitesseturnarround[i+50];
     }
     poids += trapeze;
     baricentre += trapeze*i;
   } 

   /* Calcul du baricentre final*/
   if (poids == 0)
     baricentre = 0;
   else
    baricentre = baricentre/poids;

  /* Déplacement Robot */
  robot->Motor[LEFT].Value = -baricentre*0.1 + FORWARD_SPEED;
  robot->Motor[RIGHT].Value = baricentre*0.1 + FORWARD_SPEED;
  return TRUE;
}

void FastStepRobot(struct Robot *robot){
}

void ResetRobot(struct Robot *robot){
  pas = 0;
}

void UserCommand(struct Robot *robot,char *text)
{
  WriteComment("unknown command"); /* no commands */
}

void DrawUserInfo(struct Robot *robot,u_char info,u_char page)
{

  switch(info)
  {
    case 1:
      switch(page)
      {
        case 1: Color(MAGENTA);
                FillRectangle(0,0,40,40);
                Color(BLUE);
                DrawLine(100,100,160,180);
                Color(WHITE);
                DrawPoint(200,200);
                Color(YELLOW);
                DrawRectangle(240,100,80,40);
                Color(GREEN);
                DrawText(240,230,"hello world");
                break;
        case 2: Color(RED);
                DrawArc(200,50,100,100,0,360*64);
                Color(YELLOW);
                FillArc(225,75,50,50,0,360*64);
                Color(BLACK);
                DrawText(140,170,"This is the brain of the robot");
      }
      break;
    case 2:     DrawStep();
  }
}



