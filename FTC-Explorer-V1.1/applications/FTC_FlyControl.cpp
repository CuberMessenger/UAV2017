/******************** (C) COPYRIGHT 2015 FTC ***************************
 * ����		 ��FTC
 * �ļ���  ��FTC_FlyControl.cpp
 * ����    �����п���
**********************************************************************************/
#include "FTC_FlyControl.h"

FTC_FlyControl fc;

float getDeltaT(uint32_t currentT) {
	static uint32_t previousT;
	float deltaT = (currentT-previousT) * 1e-6;
	previousT = currentT;

	return deltaT;
}

FTC_FlyControl::FTC_FlyControl() {
	rollPitchRate = 150;
	yawRate = 50;

	altHoldDeadband = 100;

	//����PID����
	PID_Reset();
}

//����PID����
void FTC_FlyControl::PID_Reset(void) {
	pid[PIDROLL].set_pid(0.15, 0.15, 0.02, 200);
	pid[PIDPITCH].set_pid(0.15, 0.15, 0.02, 200);
	pid[PIDYAW].set_pid(0.8, 0.45, 0, 200);
	pid[PIDANGLE].set_pid(5, 0, 0, 0);
	pid[PIDMAG].set_pid(2, 0, 0, 0);
	pid[PIDVELZ].set_pid(1.5, 0.5, 0.002, 150);
	pid[PIDALT].set_pid(1.2, 0, 0, 200);
}

//��������̬�⻷����
void FTC_FlyControl::Attitude_Outter_Loop(void) {
	//����Ƕ����ֵ
	out_ans[ROLL] = constrain_int32((rc.Command[ROLL]*2), -((int) FLYANGLE_MAX), +FLYANGLE_MAX)-imu.angle.x*10;
	out_ans[PITCH] = constrain_int32((rc.Command[PITCH]*2), -((int) FLYANGLE_MAX), +FLYANGLE_MAX)-imu.angle.y*10;
	out_ans[ROLL] = applyDeadband(out_ans[ROLL], 2);
	out_ans[PITCH] = applyDeadband(out_ans[PITCH], 2);

	Vector3f Gyro_ADC = imu.Gyro_lpf/4.0f;

	RateError[ROLL] = pid[PIDANGLE].get_p(out_ans[ROLL])-Gyro_ADC.x;
	RateError[PITCH] = pid[PIDANGLE].get_p(out_ans[PITCH])-Gyro_ADC.y;
	RateError[YAW] = ((int32_t) (yawRate) * rc.Command[YAW])/32-Gyro_ADC.z;
}

//��������̬�ڻ�����
void FTC_FlyControl::Attitude_Inner_Loop(void) {
	static float deltaT = PID_INNER_LOOP_TIME * 1e-6;
	float tiltAngle = constrain_float(max(abs(imu.angle.x), abs(imu.angle.y)), 0, 20);

	//if (true) {//(rc.rawData[THROTTLE]<RC_MINCHECK) {
	//	pid[PIDROLL].reset_I();
	//	pid[PIDPITCH].reset_I();
	//	pid[PIDROLL].reset_I();
	//}

	//inner_ans[PIDROLL] = pid[PIDROLL].get_pid(RateError[PIDROLL], deltaT);
	//inner_ans[PIDPITCH] = pid[PIDPITCH].get_pid(RateError[PIDPITCH], deltaT);
	//inner_ans[PIDYAW] = -constrain_int32(inner_ans[YAW], -300-abs(rc.Command[YAW]), +300+abs(rc.Command[YAW]));

	for (u8 i = 0; i<3; i++) {
		//�����ŵ��ڼ��ֵʱ��������
		if ((rc.rawData[THROTTLE])<RC_MINCHECK)
			pid[i].reset_I();
	
		//�õ��ڻ�PID���
		inner_ans[i] = pid[i].get_pid(RateError[i], PID_INNER_LOOP_TIME*1e-6);
	}
	inner_ans[YAW] = -constrain_int32(inner_ans[YAW], -300-abs(rc.Command[YAW]), +300+abs(rc.Command[YAW]));

	//������б����
	if (!ftc.f.ALTHOLD)
		rc.Command[THROTTLE] = (rc.Command[THROTTLE]-1000)/cosf(radians(tiltAngle))+1000;
	motor.writeMotor(rc.Command[THROTTLE], inner_ans[ROLL], inner_ans[PITCH], inner_ans[YAW]);

	//motor.writeMotor(getThrottleCom(rc.Command[THROTTLE]), inner_ans[ROLL], inner_ans[PITCH], inner_ans[YAW]);
}

//�������߶��⻷����
void FTC_FlyControl::Altitude_Outter_Loop(void) {
	//to do
}

//�������߶��ڻ�����
void FTC_FlyControl::Altitude_Inner_Loop(void) {
	//to do
}

void FTC_FlyControl::AltHoldReset(void) {
	AltHold = nav.position.z;
}

uint16_t FTC_FlyControl::getThrottleCom(int16_t throttle) {
	return ((ftc.f.ALTHOLD) ? throttle :
		((rc.Command[THROTTLE]-1000)/cosf(radians(max(abs(imu.angle.x), abs(imu.angle.y))))+1000));
}

/************************ (C) COPYRIGHT 2015 FTC *****END OF FILE**********************/