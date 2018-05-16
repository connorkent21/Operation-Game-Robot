/*
Sensor Port 1 = Tetrix to EV3 converter
Sensor Port 2 = Cross Beam Touch Sensor
Sensor Port 3 = Long Beam Touch Sensor
Sensor Port 4 = Color Sensor

ARRAY SETUP
	1: White Card
	2: Blue Card
	3: Green Card
	4: Yellow Card
	5: Red Card

TETRIX TO EV3
	SV1 = Cross Beam Servo
	SV2 = Pully Servo
	SV3 = Claw Servo
	SV8 = Battery
*/

#include "EV3Servo-lib-UW.c"
#define size1 2
#define size2 5
#define posRight 1220
#define posCenter 720
#define posLeft 220

////////////////////////////
//STRUCT ARRAYS FOR PASSING

typedef struct
{
	int array[size1];
}Points;

typedef struct
{
	int array[size2];
}Bones;

void initPoints(Points score)
{
	for(int index = 0; index < size1; index++)
	{
		score.array[index] = 0;
	}
}

void initBones(Bones boneID)
{
	for(int index = 0; index < size2; index++)
		boneID.array[index] = 1;
}

/////////////////
//CLAW FUNCTIONS

void armUp()
{
	setServoPosition(S1, 2, -20);
}

void armDown()
{
	setServoPosition(S1, 2, 90);
}

void gripperOpen(bool open)
{
	if(open == true)
		setGripperPosition(S1, 3, 20);
	else
		setGripperPosition(S1, 3, 0);
}

void makeGrab()
{
	gripperOpen(true);
	wait1Msec(500);
	armDown();
	wait1Msec(1000);
	gripperOpen(false);
	wait1Msec(1000);
	armUp();
}

/////////////////////
//MOVEMENT FUNCTIONS 

void goHome()
{
	motor[motorA] = 40;
	motor[motorC] = 30;
	while(SensorValue[S3] == 0)
	{}
	motor[motorA] = motor[motorC] = 0;

	setServoSpeed(S1, 1, -10, -7, 19);
	while(SensorValue[S2] == 0)
	{}
	setServoSpeed(S1, 1, 0, -7, 19);
}

void longDist(int cm)
{
	nMotorEncoder[motorA] = 0;

	motor[motorA] = motor[motorC] = -40;

	while(nMotorEncoder[motorA] > -((cm*360.0)/(2*PI*2.75)))
	{}
	motor[motorA] = motor[motorC] = 0;
}

void crossDist(short time)
{
	setServoSpeed(S1, 1, 20, -7, 19);
	clearTimer(T1);
	while(time1[T1] <= time)
	{}
	setServoSpeed(S1, 1, 0, -7, 19);

}

/////////////////////
//GAMEPLAY FUNCTIONS

void successCheck(int boneArrayIndex, Bones bone, Points score, int playerRobo)
{
	bool cardScan = false;
	int bonePick = -1;
		do
		{
			while(SensorValue[S4] != colorGreen && SensorValue[S4] != colorRed)
			{
				eraseDisplay();
				displayString(6, "PLEASE SAY WHETHER ATTEMPT WAS SUCCESSFUL.");
				displayString(8, "GREEN CARD FOR 'YES'. RED CARD FOR 'NO'.");
				if(SensorValue[S4] != colorGreen && SensorValue[S4] != colorRed && SensorValue[S4] != colorBlack)
				{
					nVolume = 4;
					playSound(soundDownwardTones);
					eraseDisplay();
					displayString(6, "Invalid Color.");
					while (SensorValue[S4] != colorBlack)
					{}
					wait1Msec(1000);
					eraseDisplay();
				}
			}
			nVolume = 4;
			eraseDisplay();
			bonePick = SensorValue[S4];
			displayString(6, "Color num: %i", bonePick);
			wait1Msec(1000);
			eraseDisplay();
			if(bonePick == colorGreen)
			{
				playSound(soundUpwardTones);
				bone.array[boneArrayIndex] = 0;
				score.array[playerRobo] += 1;
				displayString(6, "POINTS: Harman: %i  Player: %i  ",score.array[0], score.array[1]);
				wait1Msec(2000);
				eraseDisplay();
				cardScan = true;
			}
			else if(bonePick == colorRed)
			{
				playSound(soundDownwardTones);
				displayString(6, "POINTS: Harman: %i  Player: %i  ",score.array[0], score.array[1]);
				displayString(8, "PLEASE REPLACE THE BONE");
				wait1Msec(1000);
				eraseDisplay();
				cardScan = true;
			}
			else
			{
				playSound(soundDownwardTones);
				displayString(6, "Invalid Color.");
				wait1Msec(1000);
				eraseDisplay();
			}
		}while(cardScan == false);
}

void getBone(int boneID, Bones bone, Points score)
{
	bool turnDone = false;

		do
		{
			displayString(6, "color:  %i", boneID);
			wait1Msec(1000);
			eraseDisplay();
			if(boneID == colorWhite)
			{
				if(bone.array[0] != 0)
				{
					longDist(40);
					wait1Msec(500);
					crossDist(posLeft);
					wait1Msec(500);
					makeGrab();
					wait1Msec(1000);
					goHome();
					wait1Msec(500);
					gripperOpen(true);
					turnDone = true;
					while(SensorValue[S4] != colorBlack)
					{

						displayString(6, "Please remove the card");
						wait1Msec(1000);
						eraseDisplay();
					}

					successCheck(0, bone, score, 0);

				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN.");
					displayString(8, "PLEASE SCAN A NEW CARD");
				}
				eraseDisplay();
			}
			else if(boneID == colorBlue)
			{
				if(bone.array[1] != 0)
				{
					longDist(40);
					wait1Msec(500);
					crossDist(posRight - 20);
					wait1Msec(500);
					makeGrab();
					wait1Msec(1000);
					goHome();
					wait1Msec(500);
					gripperOpen(true);
					turnDone = true;
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();

					successCheck(1, bone, score, 0);
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN.");
					displayString(8, "PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
				eraseDisplay();
			}
			else if(boneID == colorGreen)
			{
				if(bone.array[2] != 0)
				{
					longDist(25);
					wait1Msec(500);
					crossDist(posCenter);
					wait1Msec(500);
					makeGrab();
					wait1Msec(1000);
					goHome();
					wait1Msec(500);
					gripperOpen(true);
					turnDone = true;
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();

					successCheck(2, bone, score, 0);
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN.");
					displayString(8, "PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
				eraseDisplay();
			}
			else if(boneID == colorYellow)
			{
				if(bone.array[3] != 0)
				{
					longDist(10);
					wait1Msec(500);
					crossDist(posLeft);
					wait1Msec(500);
					makeGrab();
					wait1Msec(1000);
					goHome();
					wait1Msec(500);
					gripperOpen(true);
					turnDone = true;
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();
					successCheck(3, bone, score, 0);
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN.");
					displayString(8, "PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
			}
			else if(boneID == colorRed)
			{
				if(bone.array[4] != 0)
				{
					longDist(10);
					wait1Msec(500);
					crossDist(posRight);
					wait1Msec(500);
					makeGrab();
					wait1Msec(1000);
					goHome();
					wait1Msec(500);
					gripperOpen(true);
					turnDone = true;
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();

					//determine success or failure
					successCheck(4, bone, score, 0);
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN.");
					displayString(8, "PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
			}
			else
			{
				playSound(soundException);
				displayString(6, "COLOR NOT RECOGNIZED. PLEASE SCAN AGAIN.");
				wait1Msec(1000);
				eraseDisplay();
			}
		}while(turnDone == false);
}

//PLAYERS TURN
	void playersTurn(Bones bone, Points score, int bonePick)
	{
		bool turnDone = false;

		do
		{
			while(SensorValue[S4] == colorBlack)
			{
				displayString(6, "PLAYERS TURN.");
				displayString(8, "DRAW A CARD AND SCAN IT");
			}
			eraseDisplay();
			nVolume = 4;
			playSound(soundUpwardTones);
			bonePick = SensorValue[S4];
			if(bonePick == colorWhite)
			{
				if(bone.array[0] != 0)
				{
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();
					wait1Msec(500);
					successCheck(0, bone, score, 1);
					turnDone = true;
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN. PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
			}
			else if(bonePick == colorBlue)
			{
				if(bone.array[1] != 0)
				{
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();
					wait1Msec(500);
					successCheck(1, bone, score, 1);
					turnDone = true;
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN. PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
			}
			else if(bonePick == colorGreen)
			{
				if(bone.array[2] != 0)
				{
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();
					wait1Msec(500);
					successCheck(2, bone, score, 1);
					turnDone = true;
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN. PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
			}
			else if(bonePick == colorYellow)
			{
				if(bone.array[3] != 0)
				{
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();
					wait1Msec(500);
					successCheck(3, bone, score, 1);
					turnDone = true;
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN. PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
			}
			else if(bonePick == colorRed)
			{
				if(bone.array[4] != 0)
				{
					while(SensorValue[S4] != colorBlack)
					{
						displayString(6, "Please remove the card");
					}
					eraseDisplay();
					wait1Msec(500);
					successCheck(4, bone, score, 1);
					turnDone = true;
				}
				else
				{
					playSound(soundException);
					displayString(6, "BONE ALREADY TAKEN. PLEASE SCAN A NEW CARD");
					wait1Msec(1000);
					eraseDisplay();
				}
			}
			else
			{
				playSound(soundException);
				displayString(6, "COLOR NOT RECOGNIZED. PLEASE SCAN AGAIN.");
				wait1Msec(3000);
				eraseDisplay();
			}
		}while(turnDone == false);

	}
	
////////////////
//END CONDITIONS

	void checkDone(Bones bone, Points score, bool theEnd)
	{
		int count = 0;
		for(int index = 0; index < size2; index++)
		{
			if(bone.array[index] == 0)
				count++;
		}
		if(count == 5)
		{
			playSound(soundFastUpwardTones);
			displayString(6, "GAME OVER. ALL BONES ARE GONE.");
			wait1Msec(2000);
			displayString(6, "FINAL SCORE:");
			displayString(8, "Harman: %i  Player: %i", score.array[0], score.array[1]);
			wait1Msec(10000);
			eraseDisplay();
			theEnd = true;
		}
	}


///////
//MAIN

task main()
{

	//configure servo controller port
	SensorType[S1] = sensorI2CCustom9V;

	//set up sensors

	SensorType[S2] = sensorEV3_Touch;
	SensorType[S3] = sensorEV3_Touch;
	SensorType[S4] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S4] = modeEV3Color_Color;
	wait1Msec(50);

	//wait for start button press & then initialize position
	while(!getButtonPress(buttonEnter))
	{}
	while(getButtonPress(buttonEnter))
	{}

	goHome();
	wait1Msec(500);
	armUp();
	gripperOpen(false);
	wait1Msec(750);

	//set up arrays
	Bones bone;
	Points score;

	initPoints(score);
	initBones(bone);

	//set up end condition
	bool theEnd = false;

	while(theEnd == false)
	{
		int bonePick = -1;
		//ROBOT GOES FIRST
		checkDone(bone, score, theEnd);
		if(theEnd == false)
		{
			displayString(6, "PLEASE DRAW A CARD AND SCAN IT");
			wait1Msec(5000);
			eraseDisplay();

			while(SensorValue[S4] == colorBlack)
			{}
			nVolume = 4;
			playSound(soundUpwardTones);
			bonePick = SensorValue[S4];

			getBone(bonePick, bone, score);
		}
		checkDone(bone, score, theEnd);

		if(theEnd == false)
		{
			wait1Msec(2000);
			playersTurn(bone, score, bonePick);
		}
	}
}
