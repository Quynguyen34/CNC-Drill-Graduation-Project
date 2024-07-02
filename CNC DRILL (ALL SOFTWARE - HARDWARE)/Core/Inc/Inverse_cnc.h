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
  float MoveX;
  float MoveY;
  float MoveZ;
  //speed of STEP
  uint16_t pos1dot;
  uint16_t pos2dot;
  uint16_t pos3dot;
  //accelation
  uint16_t accel1;
  uint16_t accel2;
  uint16_t accel3;
  //decelation
  uint16_t jerk1;
  uint16_t jerk2;
  uint16_t jerk3;
  //value of max_speed | a_max | j_max
  uint16_t max_speedXY;
  uint16_t max_speedZ;
  uint16_t a_maxX;
  uint16_t j_maxX;
  uint16_t a_maxY;
  double_t j_maxY;
  uint16_t a_maxZ;
  uint16_t j_maxZ;
  //time
  float t;
  float t1;
} CNC_pos_t;

typedef struct{
	float set_posX, set_posY, set_posZ;
	float pos_x, pos_y, pos_z;
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
signed int caculate_pos(float pos, float pwm);

#endif /* INC_INVERSE_CNC_H_ */
