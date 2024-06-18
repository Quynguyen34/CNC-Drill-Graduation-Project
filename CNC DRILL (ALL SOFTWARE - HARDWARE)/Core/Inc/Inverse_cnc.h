/*
 * Inverse_cnc.h
 *
 *  Created on: Apr 25, 2024
 *      Author: Wis
 */

#ifndef INC_INVERSE_CNC_H_
#define INC_INVERSE_CNC_H_
#include "math.h"
#include "stdint.h"
typedef struct {
  uint8_t Lsw1;
  uint8_t Lsw2;
  uint8_t Lsw3;
  uint8_t Lsw4;
  uint8_t Lsw5;
  uint8_t Lsw6;
  //position x-y-z
  float x ;
  float y ;
  float z ;
  //moverment
  double_t MoveX;
  double_t MoveY;
  double_t MoveZ;
  //speed of STEP
  double_t pos1dot;
  double_t pos2dot;
  double_t pos3dot;
  //accelation
  double_t accel1;
  double_t accel2;
  double_t accel3;
  //decelation
  double_t jerk1;
  double_t jerk2;
  double_t jerk3;
  //value of max_speed | a_max | j_max
  double_t max_speedXY;
  double_t max_speedZ;
  double_t a_maxX;
  double_t j_maxX;
  double_t a_maxY;
  double_t j_maxY;
  double_t a_maxZ;
  double_t j_maxZ;
  //time
  float t;
  float t1;
} CNC_pos_t;

typedef struct{
	double_t set_posX, set_posY, set_posZ;
	double_t pos_x, pos_y, pos_z;
}Inv_CNC_t;

extern CNC_pos_t CNC_pos;
extern Inv_CNC_t CNC;

void HOME(void);
void Stop_STEPPER(void);
void MoveToPosXY(float x, float y);
void MoveToPosZ(float z);
void initializeCNC_pos(CNC_pos_t *cnc);
void trans_to_posXY(float x, float y);
void trans_to_posZ(float z);
signed long long int caculate_pos(double pos, double pwm);

#endif /* INC_INVERSE_CNC_H_ */
