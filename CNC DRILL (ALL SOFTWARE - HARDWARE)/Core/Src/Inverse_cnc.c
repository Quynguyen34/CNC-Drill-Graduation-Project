/*
 * Inverse_cnc.c
 *
 *  Created on: Jan 21, 2024
 *  Author: Nguyen Ngoc Quy (Wis)
 */


#include "Inverse_cnc.h"

extern Inv_CNC_t CNC;
extern CNC_pos_t CNC_pos;

void initializeCNC_pos(CNC_pos_t *cnc) {
	cnc->Lsw1 = 0;
	cnc->Lsw2 = 0;
	cnc->Lsw3 = 0;
	cnc->Lsw4 = 0;
	cnc->Lsw5 = 0;
	cnc->Lsw6 = 0;
    cnc->accel1 = 0;
    cnc->accel2 = 0;
    cnc->accel3 = 0;
    cnc->jerk1 = 0;
    cnc->jerk2 = 0;
    cnc->jerk3 = 0;
    cnc->max_speedXY = 30000;
    cnc->max_speedZ = 7000;
    cnc->a_maxX = 10000;
    cnc->j_maxX = 7000;
    cnc->a_maxY = 10000;
    cnc->j_maxY = 7000;
    cnc->a_maxZ = 1000;
    cnc->j_maxZ = 1000;
    cnc->t = 0.25;
    cnc->t1 = 0.5;
}

void trans_to_posXY(float x,float y)
{
	CNC.set_posX = x - CNC.pos_x;
	CNC.set_posY = y - CNC.pos_y;
}
void trans_to_posZ(float z)
{
	CNC.set_posZ = z - CNC.pos_z;
}

signed int caculate_pos(float pos, float pwm)
{
	return pos*pwm;
}
