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

static unsigned char scanBuff_ackSignal[8] = { 11, 22, 55, 12, 1, 6, 6, 178 };

String comdata = "";
bool mark = 0;
#define REVIBUF 6
int numdata[REVIBUF] = { 0 };
unsigned char runStateFlag = 2; //2:关闭机器  1：启动机器
unsigned char machineStatus = 0;

//起始位置55+数据标识位XX+数据位H+数据位L+校验位P+结束位77
//数据标识位
//12：开
//13：关
//14:横向速度
//15:纵向速度
//16：横向角分辨率
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
				if (j == REVIBUF)
				{
					break;
				}
				numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
			}
		}
		comdata = String("");
		unsigned char pariXor = 0;
		if (numdata[0] == 55 && numdata[REVIBUF-1] == 77)
		{
			if (numdata[1] == 12)//开
			{
				pariXor ^= numdata[2];
				pariXor ^= numdata[3];
				if (pariXor == numdata[4])
				{
					serial_name.write(scanBuff_ackSignal, 8);
					if (1 != machineStatus)
					{
						runStateFlag = 1;
						machineStatus = 1;
					}

				}
			}
			else if (numdata[1] == 13)//关
			{
				pariXor ^= numdata[2];
				pariXor ^= numdata[3];
				if (pariXor == numdata[4])
				{
					serial_name.write(scanBuff_ackSignal, 8);
					if (2 != machineStatus)
					{
						runStateFlag = 2;
						machineStatus = 2;
					}
				}
			}
			else if (numdata[1] == 16)//横向角分辨率
			{
				pariXor ^= numdata[2];
				pariXor ^= numdata[3];
				if (pariXor == numdata[4])
				{
					serial_name.write(scanBuff_ackSignal, 8);
					m_controlInfo.angleSize = numdata[2] * 256 + numdata[3];
					//serial_name.println(m_controlInfo.angleSize);
					//runStateFlag = 0;
				}
			}
			
		}


		//if (numdata[0] == 55 && numdata[2] == 77)
		//{
		//	if (numdata[1] == 1)
		//	{
		//	//	serial_name.println("start machine");
		//		serial_name.write(scanBuff_ackSignal, 8);

		//		runStateFlag = 1;
		//	}
		//	else if (numdata[1] == 2)
		//	{
		//	//	serial_name.println("close machine");
		//		serial_name.write(scanBuff_ackSignal, 8);

		//		runStateFlag = 0;
		//	}
		//	else
		//	{
		//		runStateFlag = 0;
		//	}
		//}
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
		//	serial_name.write(scanBuff_ackSignal, 8);

		//}
		for (int i = 0; i < REVIBUF; i++)
		{
			numdata[i] = 0;
		}
		mark = 0;

	}
}


#endif