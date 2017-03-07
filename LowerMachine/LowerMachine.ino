#include"common.h"

#include <Wire.h>
#include <MsTimer2.h>
#include "LIDARLite.h"
#include "MotorEncoder.h"
#include "Serial.h"
#define DEBUGPRINT

#define UPPERBOUND 2500
#define LOWERBOUND -200

LIDARLite myLidarLite;
struct controlInfo m_controlInfo;
volatile  int v_oldEncoderPos = 0;
volatile bool flagLED = 0;

void ResetSystem()
{
	bool h_endReset = 1;
	bool v_endReset = 1;
	H_Motor_STOP();
	V_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_LOWSPEED); //远离线位开关  持续4s
	delay(4000);

	H_Motor_Ctrl(CLOSETOSTARTLOC, MOTOR_LOWSPEED);
	V_Motor_Ctrl(CLOSETOSTARTLOC, MOTOR_LOWSPEED);
	while (h_endReset || (v_endReset))
	{
		if ((!digitalRead(h_encoderPinC)) && h_endReset)
		{
			h_endReset = 0;
			H_Motor_STOP();
		}
		
		if ((digitalRead(v_startLocation)) && v_endReset)
		{
			v_endReset = 0;
			V_Motor_STOP();
		}
	}
	H_Motor_STOP();
	V_Motor_STOP();

	serial_name.print("System Reset\r\n");
}



void ResetSystemV()
{

	H_Motor_STOP();
	V_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_LOWSPEED); //远离线位开关  持续2s
	delay(4000);
	V_Motor_STOP();
	
	V_Motor_Ctrl(CLOSETOSTARTLOC, MOTOR_LOWSPEED);//靠近线位开关  

	while(!digitalRead(v_startLocation));
	V_Motor_STOP();

	serial_name.print("System Reset\r\n");
}

void openMachine()
{
	V_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_HIGHSPEED);
	while (HIGH == digitalRead(v_startLocation));
	v_encoderPos = 0;
	h_encoderPos = 0;
	H_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_LOWSPEED);
	serial_name.print("System start\r\n");
	delay(500);

	/*H_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_LOWSPEED);
	while (true)
	{
		Serial.println(h_encoderPos);
		delay(100);
	}*/

}
void initInfo()
{
	m_controlInfo.h_speedPWM = 220;
	m_controlInfo.v_speedPWM = 50;
	m_controlInfo.h_dir = 0; //0：向右旋转  1：向左旋转
	m_controlInfo.v_dir = 0;  //0:逆时针旋转   1:顺时针旋转
	m_controlInfo.verticalDirZcnt = 5;
	m_controlInfo.angleSize = 500;
}

void printInfo()
{

	int r = myLidarLite.distance();
	int h = h_encoderPos;
	int v = v_encoderPos;// [i];
	int orn = h ^ v;
	serial_name.print(11);
	serial_name.print(" ");
	serial_name.print(h);
	serial_name.print(" ");
	serial_name.print(v);
	serial_name.print(" ");
	serial_name.print(r);

	serial_name.print(" ");
	serial_name.print(orn);
	serial_name.print(" ");
	serial_name.println(22);
	delay(30);

	
}
void verticalDeflection(int angle, int num)
{
	int vCnt = 0;

	V_Motor_Ctrl(m_controlInfo.v_dir, 60);
	while (v_encoderPos < 2000);
	analogWrite(v_speedPin, 255);
	delay(1000);
	while (true)
	{
		V_Motor_Ctrl(m_controlInfo.v_dir, m_controlInfo.v_speedPWM);
		//int agoNum = v_encoderPos;
		while (v_encoderPos < (2000 + angle)) //�����ת
		{
			printInfo();
		}
		analogWrite(v_speedPin, 255);
		delay(300);
		m_controlInfo.v_dir = !m_controlInfo.v_dir;
		V_Motor_Ctrl(m_controlInfo.v_dir, m_controlInfo.v_speedPWM);
	//	agoNum = v_encoderPos;
		while (v_encoderPos  >(2000-angle)) //��ǰ��ת
		{
			printInfo();
		}
		analogWrite(v_speedPin, 255);
		delay(300);
		m_controlInfo.v_dir = !m_controlInfo.v_dir;
	//	Serial2.println(v_encoderPos);
		if (++vCnt == num){
			analogWrite(h_speedPin, 255);
			analogWrite(v_speedPin, 255);
			printInfo();
			while (true);
		}
	}


//	m_controlInfo.v_dir = !m_controlInfo.v_dir;
	//if (m_controlInfo.v_dir == 1)
}
void horiDeflection(int angle, int num)
{
	int vCnt = 0;
	int cnt = 0;
	H_Motor_Ctrl(m_controlInfo.h_dir, m_controlInfo.h_speedPWM);
	h_encoderPos = 0;
	while (h_encoderPos <= 5);
	while (true)
	{
		if (h_encoderPos > angle)
		{
			analogWrite(h_speedPin, 255);
			delay(200);
			m_controlInfo.h_dir = !m_controlInfo.h_dir;
			H_Motor_Ctrl(m_controlInfo.h_dir, m_controlInfo.h_speedPWM);
			while (h_encoderPos > angle-5);
			//delay(300);

		}
		if (h_encoderPos <= 0)
		{
			if (++cnt == num)
			{
				if (++vCnt == 2){
					analogWrite(h_speedPin, 255);
					analogWrite(v_speedPin, 255);
					while (true);
				}
				analogWrite(h_speedPin, 255);
				V_Motor_Ctrl(m_controlInfo.v_dir, m_controlInfo.v_speedPWM);
				m_controlInfo.v_dir = !m_controlInfo.v_dir;
				//if (m_controlInfo.v_dir == 1)
				while (v_encoderPos < 4000);
				v_encoderPos = 0;
				//else
				//	while (v_encoderPos > 0);
				V_Motor_Ctrl(0, 255);
				m_controlInfo.h_dir = !m_controlInfo.h_dir;
				H_Motor_Ctrl(m_controlInfo.h_dir, 40);
				while (h_encoderPos < 4000);	
				h_encoderPos = 0;
				analogWrite(h_speedPin, 255);
				while (1);
				delay(1000);
				H_Motor_Ctrl(m_controlInfo.h_dir, m_controlInfo.h_speedPWM);
			
				while (h_encoderPos <= 5);
				cnt = 0;
				continue;
			}
			analogWrite(h_speedPin, 255);
			delay(200);
			m_controlInfo.h_dir = !m_controlInfo.h_dir;
			H_Motor_Ctrl(m_controlInfo.h_dir, m_controlInfo.h_speedPWM);
			while (h_encoderPos < 5);
			
		}
		if (h_encoderPos > angle - 10 || h_encoderPos < 10)
		{

		}
		else
		{
			printInfo();
		}
	}
	
}


void cirDeflection(int num)
{
	int vCnt = 0;

	V_Motor_Ctrl(m_controlInfo.v_dir, 200);
	while (v_encoderPos < 2000);
	delay(5);
	while (v_encoderPos < 2000);
	analogWrite(v_speedPin, 255);
	delay(1000);
	while (true)
	{
		printInfo();
	}


	h_encoderPos = 0;
	H_Motor_Ctrl(m_controlInfo.h_dir,150);

	bool invFlag = 0;
	while (true)
	{
		if (h_encoderPos > 7997)
		{
			if (++vCnt == num)
			{
				analogWrite(h_speedPin, 255);
				while (1);
			}
		//	h_encoderPos = 0;
			analogWrite(h_speedPin, 255);
			delay(100);
			m_controlInfo.h_dir = !m_controlInfo.h_dir;
			H_Motor_Ctrl(m_controlInfo.h_dir, 150);
			while (h_encoderPos > 7990); //7990
			delay(200);
			invFlag = 1;
		}
		if (invFlag && h_encoderPos < 3)
		{
			invFlag = 0;
			//	h_encoderPos = 0;
			analogWrite(h_speedPin, 255);
			delay(100);
			m_controlInfo.h_dir = !m_controlInfo.h_dir;
			H_Motor_Ctrl(m_controlInfo.h_dir, 150);
			while (h_encoderPos > 5);
			delay(200);
		}
		if (h_encoderPos < 7900 && h_encoderPos > 10)
		{
			printInfo();
		}
	}
}

void timeUnusualFun()
{
	flagLED = !flagLED;
	if (flagLED)
	{
		//analogWrite(40, 0);
	}
	else
	{
		
	}
	analogWrite(40, 255);
	/*int absEcoderNum = abs(v_oldEncoderPos - v_encoderPos);
	v_oldEncoderPos = v_encoderPos;
	if (v_encoderPos < -200)
	{

	}
	else if (absEcoderNum < 20)
	{

	}*/
	//Serial.println(absEcoderNum);
}

void initPhase()
{

	pinMode(40, OUTPUT); //LED

	//pinMode(31, OUTPUT); //蜂蜜器



	pinMode(h_speedPin, OUTPUT);
	analogWrite(h_speedPin, 255); //PWM=255��ʾ���ֹͣת��
	pinMode(h_dirPin, OUTPUT);
	digitalWrite(h_dirPin, LOW);

	pinMode(h_encoderPinA, INPUT);
	pinMode(h_encoderPinB, INPUT);
	pinMode(h_encoderPinC, INPUT);
	digitalWrite(h_encoderPinA, HIGH);  // turn on pullup resistor
	digitalWrite(h_encoderPinB, HIGH);  // turn on pullup resistor
	digitalWrite(h_encoderPinC, HIGH);

	pinMode(v_encoderPinA, INPUT);
	pinMode(v_encoderPinB, INPUT);
	digitalWrite(v_encoderPinA, HIGH);  // turn on pullup resistor
	digitalWrite(v_encoderPinB, HIGH);  // turn on pullup resistor

	pinMode(v_speedPin, OUTPUT);
	pinMode(v_dirPin, OUTPUT);
	analogWrite(v_speedPin, 255); //stop
	digitalWrite(v_dirPin, LOW); //full speed
	pinMode(v_startLocation, INPUT);
	//  pinMode(v_clearButton_90, INPUT);
	digitalWrite(v_startLocation, HIGH);
	//  digitalWrite(v_clearButton_90, HIGH);


	attachInterrupt(0, H_doEncoderA, CHANGE);
	attachInterrupt(1, H_doEncoderB, CHANGE);
	attachInterrupt(5, V_doEncoderA, CHANGE);
	attachInterrupt(4, V_doEncoderB, CHANGE);

	myLidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz
	myLidarLite.configure(0); // Change this number to try out alternate configurations
	//  myLidarLite.write(0x02, 0x0d); // Maximum acquisition count of 0x0d. (default is 0x80)
	//  myLidarLite.write(0x04, 0b00000100); // Use non-default reference acquisition count
	//  myLidarLite.write(0x12, 0x03); // Reference acquisition count of 3 (default is 5)
	serial_name.begin(115200);
	//MsTimer2::set(2000, timeUnusualFun);
//	MsTimer2::start();
	initInfo();
	

	
	
}
int distanceFast(bool biasCorrection)
{
	byte isBusy = 1;
	int distance;
	int loopCount;

	// Poll busy bit in status register until device is idle
	while (isBusy)
	{
		// Read status register
		Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
		Wire.write(0x01);
		Wire.endTransmission();
		Wire.requestFrom(LIDARLITE_ADDR_DEFAULT, 1);
		isBusy = Wire.read();
		isBusy = bitRead(isBusy, 0); // Take LSB of status register, busy bit

		loopCount++; // Increment loop counter
		// Stop status register polling if stuck in loop
		if (loopCount > 9999)
		{
			break;
		}
	}

	// Send measurement command
	Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
	Wire.write(0X00); // Prepare write to register 0x00
	if (biasCorrection == true)
	{
		Wire.write(0X04); // Perform measurement with receiver bias correction
	}
	else
	{
		Wire.write(0X03); // Perform measurement without receiver bias correction
	}
	Wire.endTransmission();

	// Immediately read previous distance measurement data. This is valid until the next measurement finishes.
	// The I2C transaction finishes before new distance measurement data is acquired.
	// Prepare 2 byte read from registers 0x0f and 0x10
	Wire.beginTransmission(LIDARLITE_ADDR_DEFAULT);
	Wire.write(0x8f);
	Wire.endTransmission();

	// Perform the read and repack the 2 bytes into 16-bit word
	Wire.requestFrom(LIDARLITE_ADDR_DEFAULT, 2);
	distance = Wire.read();
	distance <<= 8;
	distance |= Wire.read();

	// Return the measured distance
	return distance;
}

void openBeep(unsigned char num)
{
	for (unsigned char i = 0; i < num; i++)
	{
		analogWrite(31, 255);
		delay(1000);
		analogWrite(31, 0);
		delay(1000);
	}
}

void findVEncoder_C()
{
	V_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_LOWSPEED);
	while (true)
	{
		if ((!digitalRead(v_encoderPinC)))
		{
			V_Motor_STOP();
			break;
		}
	}
	serial_name.print("have find C signal\r\n");
}
void setup()
{
	pinMode(0, INPUT);
	analogWrite(0, 255);

	pinMode(1, OUTPUT);
	analogWrite(1, 255);

	initPhase();

	scanBuff_hvl[indx_++] = 11;
	scanBuff_hvl[indx_++] = 22;
	scanBuff_hvl[indx_++] = 55;
	scanBuff_hvl[indx_++] = BUFSIZE * 6;



	//H_Motor_Ctrl(m_controlInfo.h_dir, m_controlInfo.h_speedPWM);
}

volatile bool recyc = 0;
void loop()
{

	if (0 == runStateFlag)
	{
//		MsTimer2::stop();
		analogWrite(40, 255);

		runStateFlag = 2;
		ResetSystemV();
		return;
		//continue;
	}
	if (1 == runStateFlag)
	{
		analogWrite(40, 0);
//		MsTimer2::start();
		v_oldEncoderPos = v_encoderPos;
		openMachine();
		runStateFlag = 3;
		
	}
	if (2 == runStateFlag)
	{
		return;
	}
	if (v_encoderPos < LOWERBOUND || v_encoderPos > UPPERBOUND)
	{//异常处理
		openBeep(50);
		H_Motor_STOP();
		V_Motor_STOP();
		runStateFlag = 2;
		return;
	}
	if (HIGH == digitalRead(v_startLocation))
	{

		V_Motor_STOP();


		H_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_LOWSPEED);
		delay(m_controlInfo.angleSize);
		H_Motor_STOP();

	
		V_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_HIGHSPEED);

		while (HIGH == digitalRead(v_startLocation));
		v_encoderPos = 0;
		delay(300);
		recyc = 0;

	}
#define MAXVNUM 2190
	if ((v_encoderPos) > MAXVNUM)
	{
		V_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_LOWSPEED);
		delay(300);
		V_Motor_STOP();

		H_Motor_Ctrl(FARFROMSTARTLOC, MOTOR_LOWSPEED);
		delay(m_controlInfo.angleSize);
		H_Motor_STOP();

		V_Motor_Ctrl(CLOSETOSTARTLOC, MOTOR_HIGHSPEED);
		while ((v_encoderPos) > MAXVNUM - 10);
		recyc = 1;
	}
	if ((1 == recyc) && (v_encoderPos) < 100) {
		V_Motor_Ctrl(CLOSETOSTARTLOC, MOTOR_LOWSPEED);
		return;
	}
	if (h_encoderPos > HOR_ENCODER_NUM)
	{
		openBeep(5);
		H_Motor_STOP();
		V_Motor_STOP();
		runStateFlag = 2;
		return;
	}
	static int produced = 0;;
	int r = myLidarLite.distance();
	int h = h_encoderPos;
	int v = v_encoderPos;// [i];

	scanBuff_hvl[indx_] = h / 256;
	verifyBit ^= scanBuff_hvl[indx_];
	indx_++;
	scanBuff_hvl[indx_] = h % 256;
	verifyBit ^= scanBuff_hvl[indx_];
	indx_++;
	scanBuff_hvl[indx_] = v / 256;
	verifyBit ^= scanBuff_hvl[indx_];
	indx_++;
	scanBuff_hvl[indx_] = v % 256;
	verifyBit ^= scanBuff_hvl[indx_];
	indx_++;
	scanBuff_hvl[indx_] = r / 256;
	verifyBit ^= scanBuff_hvl[indx_];
	indx_++;
	scanBuff_hvl[indx_] = r % 256;
	verifyBit ^= scanBuff_hvl[indx_];
	indx_++;
	produced++;
	if (produced == BUFSIZE)
	{
		scanBuff_hvl[indx_++] = verifyBit;
		scanBuff_hvl[indx_++] = 178;
		serial_name.write(scanBuff_hvl, indx_);
		delayMicroseconds(200);
		produced = 0;
		indx_ = 4;
		verifyBit = 0;
	}
}

