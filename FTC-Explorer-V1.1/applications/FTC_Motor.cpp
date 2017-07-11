/******************** (C) COPYRIGHT 2015 FTC ***************************
 * ����		 ��FTC
 * �ļ���  ��FTC_Motor.cpp
 * ����    �����������غ���
**********************************************************************************/
#include "FTC_Motor.h"

// //#define stupid
// #define not_stupid

FTC_Motor motor;

void FTC_Motor::writeMotor(uint16_t throttle, int32_t pidTermRoll, int32_t pidTermPitch, int32_t pidTermYaw) {
	//to do
	float T_matrix[6][4] = {{1,0.5,0.866,-1},
							{1,-0.5,0.866,1},
							{1,-1,0,-1},
							{1,-0.5,-0.866,1},
							{1,0.5,-0.866,-1},
							{1,1,0,1}};

	for (u8 i = 0; i<MAXMOTORS; i++)
		motorPWM[i] = T_matrix[i][0]*throttle+T_matrix[i][1]*pidTermRoll+T_matrix[i][2]*pidTermPitch+T_matrix[i][3]*pidTermYaw;

	int16_t maxMotor = motorPWM[0];
	for (u8 i = 1; i<MAXMOTORS; i++) {
		if (motorPWM[i]>maxMotor)
			maxMotor = motorPWM[i];
	}

	for (u8 i = 0; i<MAXMOTORS; i++) {
		if (maxMotor>MAXTHROTTLE)
			motorPWM[i] -= maxMotor-MAXTHROTTLE;
		//���Ƶ��PWM����С�����ֵ
		motorPWM[i] = constrain_uint16(motorPWM[i], MINTHROTTLE, MAXTHROTTLE);
	}

	//���δ�������򽫵���������Ϊ���
	if (!ftc.f.ARMED)
		ResetPWM();

	if (!ftc.f.ALTHOLD && rc.rawData[THROTTLE]<RC_MINCHECK)
		ResetPWM();

	//д����PWM
	pwm.SetPwm(motorPWM);

}

void FTC_Motor::getPWM(int16_t* pwm) {
	*(pwm) = motorPWM[0];
	*(pwm+1) = motorPWM[1];
	*(pwm+2) = motorPWM[2];
	*(pwm+3) = motorPWM[3];
	*(pwm+4) = motorPWM[4];
	*(pwm+5) = motorPWM[5];
}

void FTC_Motor::ResetPWM(void) {
	for (u8 i = 0; i<MAXMOTORS; i++)
		motorPWM[i] = 1000;
}

/******************* (C) COPYRIGHT 2015 FTC *****END OF FILE************/
