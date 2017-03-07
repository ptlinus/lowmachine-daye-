#ifndef _SERIAL_H_
#define _SERIAL_H_
#include"common.h"

struct serialCommBuf
{
	int h_encoderNumber;
	int v_encoderNumber;
	int distance;
};



////////////////////////////////////////////////////////////////////////////////////////////////////
#define BUFSIZE 8
#define SUMBUF (4+BUFSIZE*6+2)
static unsigned char scanBuff_hvl[SUMBUF];
static unsigned char verifyBit = 0;
static unsigned char indx_ = 0;


String comdata = "";
bool mark = 0;
#define REVIBUF 5
int numdata[REVIBUF] = { 0 };
unsigned char runStateFlag = 0; //0:关闭机器  1：启动机器
void serialEvent()
{

	while (serial_name.available() > 0)
	{
		comdata += char(serial_name.read());
		delay(2);
		mark = 1;
	}
	if (mark)
	{
		int j = 0;
		for (int i = 0; i < comdata.length(); i++)
		{
			if (comdata[i] == ' ')
			{
				j++;
			}
			else
			{
				numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
			}
		}
		comdata = String("");
		if (numdata[0] == 55 && numdata[2] == 77)
		{
			if (numdata[1] == 1)
			{
				serial_name.println("start machine");
				runStateFlag = 1;
			}
			else if (numdata[1] == 2)
			{
				serial_name.println("close machine");
				runStateFlag = 0;
			}
			else
			{
				runStateFlag = 0;
			}
		}
		//if (numdata[0] == 55 && numdata[3] == 77)
		//{
		//	if (numdata[1] >= 1 && numdata[1] <= 50)
		//	{
		//		m_controlInfo.angleSize = numdata[1] * 10;

		//	}
		//	//Serial2.println(m_controlInfo.h_speedPWM);

		//	if (numdata[2] >= 4 && numdata[2] <= 20)
		//	{
		//		m_controlInfo.v_speedPWM = numdata[2] * 10;
		//	}
		//}
		for (int i = 0; i < REVIBUF; i++)
		{
			numdata[i] = 0;
		}
		mark = 0;

	}
}


#endif