#ifndef _PID_H_
#define _PID_H_

/* PID Parameters */
int Kp = 20;
int Kd = 12;
int Ki = 0;
int Ko = 50;


struct SetPointInfo{
	double TargetTicksPerFrame;    // target speed in ticks per frame
	long Encoder;                  // encoder count
	long PrevEnc;                  // last encoder count

	int PrevInput;                // last input
	//int PrevErr;                   // last error


	//int Ierror;
	int ITerm;                    //integrated term

	long output;                    // last motor setting
};

static struct SetPointInfo leftPID, rightPID;


void resetPID(){
	leftPID.TargetTicksPerFrame = 0.0;
	leftPID.Encoder = 0;// readEncoder(LEFT);
	leftPID.PrevEnc = leftPID.Encoder;
	leftPID.output = 0;
	leftPID.PrevInput = 0;
	leftPID.ITerm = 0;

	rightPID.TargetTicksPerFrame = 0.0;
	rightPID.Encoder = 0;//readEncoder(RIGHT);
	rightPID.PrevEnc = rightPID.Encoder;
	rightPID.output = 0;
	rightPID.PrevInput = 0;
	rightPID.ITerm = 0;
}
//
void doPID(struct SetPointInfo *p) {
	long Perror;
	long output;
	int input;

	input = p->Encoder - p->PrevEnc;
	// Serial.print("input ");
	//  Serial.println(input);
	Perror = p->TargetTicksPerFrame - input;
	// Serial.print("TargetTicksPerFrame ");
	// Serial.println(p->TargetTicksPerFrame);
	output = (Kp * Perror - Kd * (input - p->PrevInput) + p->ITerm) / Ko;
	//  Serial.print(output);
	// Serial.print(" ");

	p->PrevEnc = p->Encoder;

	output += p->output;
	if (output >= 255)
		output = 255;
	else if (output <= -255)
		output = -255;
	else

		p->ITerm += Ki * Perror;
	// Serial.print(output);
	//  Serial.println();
	p->output = output;
	p->PrevInput = input;
}

void updatePID() {


}

#endif