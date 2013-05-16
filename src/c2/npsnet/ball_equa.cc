//  Program: ball_equa.cc
//  Programmer: Sam Kirby
//  Date:  5 Jul 1995
//
//
//  Description: This function uses Runge Kutta to calculate the position
//  vector of a projectile.  Using fourth order Runge Kutta I use a function
//  called simple to calculate the effects of drag and gravity.
//
//  Sources: 

#ifndef NOC2MEASURES

#include <iostream.h>
#include <math.h>
#include "globals.h"
#include "ball_equa.h"
#include "terrain.h"

//  Derivative function.  In this case I'm using gravity and drag.
//  I put drag proportional to velocity and in the 
//  opposite direction.
void f(float X_[], float dX[], float drag){

   dX[0] = X_[3];
   dX[1] = X_[4];
   dX[2] = X_[5];

   dX[3] = drag * X_[3];
   dX[4] = drag * X_[4];
   dX[5] = -9.8 + (drag * X_[5]);

   dX[6] = 0.0;
   dX[7] = 0.0;
   dX[8] = 0.0;       
}

//  Simple ballistics model.
pfVec3* simple(float worldX, float worldY, float heading, float elevation,
   BALL_BOX* box){

   pfVec3* returnPts;

   float const degToRad = M_PI / 180.0;

   int HITGROUND = 0;
   int numPoints = 0;

   float barrel = box->coef.BARREL_L;
   float muzzle = box->coef.MUZZLE_V;
   float drag = box->coef.DRAG;

   float X_[9],                     //  x vector.
      dX[9],                      //  Derivative vector.
      Xin[9],                     //  Vector to put in derivative.
      k1[9], k2[9], k3[9], k4[9], //  Values used in Runge-Kutta of position.
      radHead, radElev,           //  Original angles in radians.
      X2d, dX2d,                   //  X and dX in 2 dimensional space.
      points[MAX_POINTS][3],
      scale;

   pfVec3 temp;

   radHead = heading * degToRad;
   radElev = elevation * degToRad;
      
   //  Initial conditions. 
   X2d = barrel * cos(radElev); 
   X_[0] = worldX + (X2d * cos(radHead));
   X_[1] = worldY + (X2d * sin(radHead));
   temp[X] = X_[0];
   temp[Y] = X_[1];
   temp[Z] = X_[2];
   X_[2] = gnd_level2(temp) + (barrel * sin(radElev));

   dX2d = muzzle * cos(radElev);
   X_[3] = dX2d * cos(radHead);
   X_[4] = dX2d * sin(radHead);
   X_[5] = muzzle * sin(radElev);

   //Simple model.  Will change significantly with complex model.
   X_[6] = drag * X_[3];
   X_[7] = drag * X_[4];
   X_[8] = -9.8 + (drag * X_[5]);

   points[numPoints][0] = X_[0];
   points[numPoints][1] = X_[1];
   points[numPoints][2] = X_[2];

   numPoints++;

   //  Loop through incrementing Runge-Kutta.  
   while (!HITGROUND){
      f(X_, dX, drag );

      k1[0] = h * dX[0];
      k1[1] = h * dX[1];
      k1[2] = h * dX[2];
      k1[3] = h * dX[3];
      k1[4] = h * dX[4];
      k1[5] = h * dX[5];
      k1[6] = h * dX[6];
      k1[7] = h * dX[7];
      k1[8] = h * dX[8];

      Xin[0] = X_[0] + k1[0]/2.0;
      Xin[1] = X_[1] + k1[1]/2.0;
      Xin[2] = X_[2] + k1[2]/2.0;
      Xin[3] = X_[3] + k1[3]/2.0;
      Xin[4] = X_[4] + k1[4]/2.0;
      Xin[5] = X_[5] + k1[5]/2.0;
      Xin[6] = X_[6] + k1[6]/2.0;
      Xin[7] = X_[7] + k1[7]/2.0;
      Xin[8] = X_[8] + k1[8]/2.0;

      f(Xin, dX, drag);

      k2[0] = h * dX[0];
      k2[1] = h * dX[1];
      k2[2] = h * dX[2];
      k2[3] = h * dX[3];
      k2[4] = h * dX[4];
      k2[5] = h * dX[5];
      k2[6] = h * dX[6];
      k2[7] = h * dX[7];
      k2[8] = h * dX[8];

      Xin[0] = X_[0] + k2[0]/2.0;
      Xin[1] = X_[1] + k2[1]/2.0;
      Xin[2] = X_[2] + k2[2]/2.0;
      Xin[3] = X_[3] + k2[3]/2.0;
      Xin[4] = X_[4] + k2[4]/2.0;
      Xin[5] = X_[5] + k2[5]/2.0;
      Xin[6] = X_[6] + k2[6]/2.0;
      Xin[7] = X_[7] + k2[7]/2.0;
      Xin[8] = X_[8] + k2[8]/2.0;
   
      f(Xin, dX, drag);

      k3[0] = h * dX[0];
      k3[1] = h * dX[1];
      k3[2] = h * dX[2];
      k3[3] = h * dX[3];
      k3[4] = h * dX[4];
      k3[5] = h * dX[5];
      k3[6] = h * dX[6];
      k3[7] = h * dX[7];
      k3[8] = h * dX[8];

      Xin[0] = X_[0] + k3[0];
      Xin[1] = X_[1] + k3[1];
      Xin[2] = X_[2] + k3[2];
      Xin[3] = X_[3] + k3[3];
      Xin[4] = X_[4] + k3[4];
      Xin[5] = X_[5] + k3[5];
      Xin[6] = X_[6] + k3[6];
      Xin[7] = X_[7] + k3[7];
      Xin[8] = X_[8] + k3[8];

      f(Xin, dX, drag);

      k4[0] = h * dX[0];
      k4[1] = h * dX[1];
      k4[2] = h * dX[2];
      k4[3] = h * dX[3];
      k4[4] = h * dX[4];
      k4[5] = h * dX[5];
      k4[6] = h * dX[6];
      k4[7] = h * dX[7];
      k4[8] = h * dX[8];

      X_[0] += k1[0]/6.0 + k2[0]/3.0 + k3[0]/3.0 + k4[0]/6.0;                              
      X_[1] += k1[1]/6.0 + k2[1]/3.0 + k3[1]/3.0 + k4[1]/6.0;
      X_[2] += k1[2]/6.0 + k2[2]/3.0 + k3[2]/3.0 + k4[2]/6.0;                              
      X_[3] += k1[3]/6.0 + k2[3]/3.0 + k3[3]/3.0 + k4[3]/6.0;
      X_[4] += k1[4]/6.0 + k2[4]/3.0 + k3[4]/3.0 + k4[4]/6.0;                              
      X_[5] += k1[5]/6.0 + k2[5]/3.0 + k3[5]/3.0 + k4[5]/6.0;
      X_[6] += k1[6]/6.0 + k2[6]/3.0 + k3[6]/3.0 + k4[6]/6.0;                              
      X_[7] += k1[7]/6.0 + k2[7]/3.0 + k3[7]/3.0 + k4[7]/6.0;
      X_[8] += k1[8]/6.0 + k2[8]/3.0 + k3[8]/3.0 + k4[8]/6.0;

      points[numPoints][0] = X_[0];
      points[numPoints][1] = X_[1];
      points[numPoints][2] = X_[2];

    

      if ((points[numPoints][2] - gnd_level2(points[numPoints]) ) < 0.0){
         HITGROUND = 1;
      }
    
      numPoints++;

      if ( numPoints >= MAX_POINTS){
         cerr<<"too many points in function simple"<<endl;
         exit(0);
      }
              
   }  //End of while.

   if(NULL==(returnPts=(pfVec3 *)pfMalloc(NUM_TRAJ_POINTS *sizeof(pfVec3),
      pfGetSharedArena() ))){
         cerr<<"error in allocation: function simple"<<endl;
         return(NULL);
   }
  
   scale = (float)(numPoints - 1) / (float) (NUM_TRAJ_POINTS -1);
      
   for (int i = 0; i< NUM_TRAJ_POINTS; i++){
      returnPts[i][X] = points[(int)( (float)i * scale)][0];
      returnPts[i][Y] = points[(int)( (float)i * scale)][1];
      returnPts[i][Z] = points[(int)( (float)i * scale)][2];
   } 

   return (returnPts);  
    
}  //  End of simple.
   
#endif 
