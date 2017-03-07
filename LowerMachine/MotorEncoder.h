#ifndef _MOTOR_H_
#define _MOTOR_H_

#include"common.h"


#ifdef VERSION1
enum PinAssignments {
	h_speedPin = 6,
	h_dirPin = 7,
	h_encoderPinA = 2,
	h_encoderPinB = 3,
	h_encoderPinC = 22,

	v_encoderPinA = 18,
	v_encoderPinB = 19,
	v_speedPin = 4,
	v_dirPin = 5,
	v_startLocation = 28,
	//v_clearButton_90 = 19
};
#endif
#ifdef VERSION2
enum PinAssignments {
	h_speedPin = 5,  //0：全速  255：停止
	h_dirPin = 8, //0：向右旋转  1：向左旋转
	h_encoderPinA = 2,
	h_encoderPinB = 3,
	h_encoderPinC = 24,

	v_speedPin = 4,
	v_dirPin = 9, //0:逆时针旋转   1:顺时针旋转
	v_encoderPinA = 18,
	v_encoderPinB = 19,
	v_encoderPinC = 25,

	v_startLocation = 7,  //线位开关  //1-检测到目标
};
#endif

struct controlInfo
{
	int h_speedPWM; 
	int v_speedPWM;
	boolean h_dir;
	boolean v_dir;
	int verticalDirZcnt;
	int angleSize;
};


volatile int h_encoderPos = 0;
volatile int v_encoderPos = 0;
volatile boolean h_A_set = false;
volatile boolean h_B_set = false;
volatile boolean v_A_set = false;
volatile boolean v_B_set = false;

void H_Motor_Ctrl(bool Ctrl, int m_speed)
{
	digitalWrite(h_dirPin, Ctrl);
	analogWrite(h_speedPin, m_speed);
	if (m_speed == MOTOR_STOP)
	{
		delay(500);
	}
}

void H_Motor_STOP()
{
	analogWrite(h_speedPin, MOTOR_STOP);
//	delay(200);
}

void V_Motor_Ctrl(bool Ctrl, int m_speed)
{
	digitalWrite(v_dirPin, Ctrl);
	analogWrite(v_speedPin, m_speed);
	if (m_speed == MOTOR_STOP)
	{
		delay(500);
	}
}
void V_Motor_STOP()
{
	analogWrite(v_speedPin, MOTOR_STOP);
	//delay(200);
}

void H_doEncoderA()
{
	h_A_set = (digitalRead(h_encoderPinA) == HIGH);
#ifdef VERSION1
	if (h_A_set != h_B_set)
		h_encoderPos = h_encoderPos + 1;
	else
		h_encoderPos = h_encoderPos - 1;
#endif
#ifdef VERSION2
	if (h_A_set != h_B_set)
		h_encoderPos = h_encoderPos + 1;
	else
		h_encoderPos = h_encoderPos - 1;
#endif
}


void H_doEncoderB()
{
#ifdef VERSION1
	h_B_set = (digitalRead(h_encoderPinB) == HIGH);
	if (h_A_set == h_B_set)
		h_encoderPos = h_encoderPos + 1;
	else
		h_encoderPos = h_encoderPos - 1;
#endif
#ifdef VERSION2
	h_B_set = (digitalRead(h_encoderPinB) == HIGH);
	if (h_A_set == h_B_set)
		h_encoderPos = h_encoderPos + 1;
	else
		h_encoderPos = h_encoderPos - 1;
#endif
}

void V_doEncoderA()
{
#ifdef VERSION1
	v_A_set = (digitalRead(v_encoderPinA) == HIGH);
	if (v_A_set != v_B_set)
		v_encoderPos = v_encoderPos - 1;
	else
		v_encoderPos = v_encoderPos + 1;
#endif
#ifdef VERSION2
	v_A_set = (digitalRead(v_encoderPinA) == HIGH);
	if (v_A_set != v_B_set)
		v_encoderPos = v_encoderPos + 1;
	else
		v_encoderPos = v_encoderPos - 1;
#endif
}


void V_doEncoderB()
{
#ifdef VERSION1
	v_B_set = (digitalRead(v_encoderPinB) == HIGH);
	if (v_A_set == v_B_set)
		v_encoderPos = v_encoderPos - 1;
	else
		v_encoderPos = v_encoderPos + 1;
#endif
#ifdef VERSION2
	v_B_set = (digitalRead(v_encoderPinB) == HIGH);
	if (v_A_set == v_B_set)
		v_encoderPos = v_encoderPos - 1;
	else
		v_encoderPos = v_encoderPos + 1;
#endif
}


#endif