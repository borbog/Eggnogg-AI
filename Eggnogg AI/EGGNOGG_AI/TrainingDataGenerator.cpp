#include "TrainingData.h"


using namespace std;





HANDLE handle;

// Memory Adresses 
int a_direction = 0x00541E0C;

int a_PlayerRightPosX = 0x0054235C;
int a_PlayerLeftPosX = 0x00542200;

int a_PlayerRightJump = 0x005423E4;
int a_PlayerLeftJump = 0x00542288;


int a_LeftHasSword = 0x005421ED; //(byte)
int a_RightHasSword = 0x00542349; //(byte)

int a_PlayerRightLife = 0x005423D2; // (byte) 100 is full life . when killed often 0, but sometimes 233
int a_PlayerLeftLife = 0x00542276; // (byte) 100 is full life . when killed often 0, but sometimes 233

int a_PlayerLeftPosY = 0x00542204;
int a_PlayerLeftSwordPosY = 0x0054223C;
int a_PlayerRightPosY = 0x00542360; // sword minus y position . Approx. 5 pixel up and down
int a_PlayerRightSwordPosY = 0x00542390;

int a_PlayerLeftAttack = 0x0054225C;
int a_PlayerRightAttack = 0x005423B8;

int a_PlayerRightSliding = 0x0054234A; //0 not 1 yes
int a_PlayerLeftSliding = 0x005421EE;


int* a_SwordList; // filled at the bottom

int a_CameraPosX = 0x0055A360;



				  //variable to save the values out of the adresses
float rightPlayerPositionX = 0;
float leftPlayerPositionX = 0;

float rightPlayerPositionY = 0;
float leftPlayerPositionY = 0;

bool LefthasSword;
bool RighthasSword;


double rightLife = 0;
double leftLife = 0;

double leftAttack = 0;
double rightAttack = 0;

int direction = 0; // arrow

double distanceMapLeft = 0; // total position from -1 to 1
double distanceMapRight = 0;

double distancePlayers = 0;
float absolutedistancePlayers = 0;

float playerRightSwordPosY;
float playerLeftSwordPosY;

bool rightJumping;
bool leftJumping;

bool rightSliding; //0 not 1 yes
bool leftSliding;


vector <float> swordList;



// reading functions - from address to variable
void readDirection() {
	char directionTemp;
	ReadProcessMemory(handle, (PBYTE*)a_direction, &directionTemp, sizeof(char), 0);

	if (directionTemp == -36) {
		direction = 1; // left player can progress right
	}
	else if (directionTemp == 56) {
		direction = -1; // right player can progress left
	}
	else {
		direction = 0; // nobody can progress
	}
}

void readDistanceOnMapLeft() {
	float distanceTemp;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerLeftPosX, &distanceTemp, sizeof(float), 0);
	distanceTemp -= 2875;
	distanceTemp /= 2875;
	distanceMapLeft = static_cast<double>(distanceTemp);
}

void readDistanceOnMapRight() {
	float distanceTemp;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerRightPosX, &distanceTemp, sizeof(float), 0);
	distanceTemp -= 2875;
	distanceTemp /= 2875;
	distanceMapRight = static_cast<double>(distanceTemp);
}

void readRightPlayerPositionX() {
	ReadProcessMemory(handle, (PBYTE*)a_PlayerRightPosX, &rightPlayerPositionX, sizeof(float), 0);
}

void readLeftPlayerPositionX() {
	ReadProcessMemory(handle, (PBYTE*)a_PlayerLeftPosX, &leftPlayerPositionX, sizeof(float), 0);
}

void readRightPlayerPositionY() {
	ReadProcessMemory(handle, (PBYTE*)a_PlayerRightPosY, &rightPlayerPositionY, sizeof(float), 0);
}

void readLeftPlayerPositionY() {
	ReadProcessMemory(handle, (PBYTE*)a_PlayerLeftPosY, &leftPlayerPositionY, sizeof(float), 0);
}



void readPlayerDistance() {
	readLeftPlayerPositionX();
	readRightPlayerPositionX();
	float distanceTemp = (rightPlayerPositionX - leftPlayerPositionX);

	if (distanceTemp > 200) distanceTemp = 200;
	distanceTemp /= 200;
	if (distanceTemp > 0) distanceTemp = 1 - distanceTemp;
	else if (distanceTemp < 0) distanceTemp = -1 - distanceTemp;

	distancePlayers = static_cast<double>(distanceTemp);

}

void readAbsoluteDistance() {
	readLeftPlayerPositionX();
	readRightPlayerPositionX();
	absolutedistancePlayers = abs(rightPlayerPositionX - leftPlayerPositionX);
}



void readLeftAttack() {
	byte attackLeftTemp;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerLeftAttack, &attackLeftTemp, sizeof(byte), 0);
	if (attackLeftTemp == 0)	leftAttack = 0;
	else leftAttack = 1;
}

void readRightAttack() {
	byte attackRightTemp;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerRightAttack, &attackRightTemp, sizeof(byte), 0);
	if (attackRightTemp == 0)	rightAttack = 0;
	else rightAttack = 1;

}

void readRightDead() {
	byte lifeRightTemp;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerRightLife, &lifeRightTemp, sizeof(byte), 0);
	if (lifeRightTemp == 0 || lifeRightTemp > 100 || lifeRightTemp  < 0)	rightLife = -1;
	else rightLife = 1;
	if(getRightPlayerPositionY() > 150) rightLife = -1;
}

void readLeftDead() {
	byte lifeLeftTemp;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerLeftLife, &lifeLeftTemp, sizeof(byte), 0);
	if (lifeLeftTemp == 0 || lifeLeftTemp > 100 || lifeLeftTemp  < 0)	leftLife = -1;
	else leftLife = 1;
	if (getLeftPlayerPositionY() > 150) leftLife = -1;

}

void readLeftHasSword() {
	byte hasSword;
	ReadProcessMemory(handle, (PBYTE*)a_LeftHasSword, &hasSword, sizeof(byte), 0);
	LefthasSword = !hasSword;
}

void readRightHasSword() {
	byte hasSword;
	ReadProcessMemory(handle, (PBYTE*)a_RightHasSword, &hasSword, sizeof(byte), 0);
	RighthasSword = !hasSword;
}

void readPositionSwordRight() {
	ReadProcessMemory(handle, (PBYTE*)a_PlayerRightSwordPosY, &playerRightSwordPosY, sizeof(float), 0);
}

void readPositionSwordLeft() {
	ReadProcessMemory(handle, (PBYTE*)a_PlayerLeftSwordPosY, &playerLeftSwordPosY, sizeof(float), 0);
}

void readSwordList() {
	swordList.clear();
	for (int i = 0; i < 13; i++) {
		float pos;
		ReadProcessMemory(handle, (PBYTE*)a_SwordList[i], &pos, sizeof(float), 0);
		swordList.push_back(pos);
	}
}

void readRightJump() {
	byte rightJump;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerRightJump, &rightJump, sizeof(byte), 0);
	if (rightJump == 1)	rightJumping = false;
	else rightJumping = true;

}

void readLeftJump() {
	byte leftJump;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerLeftJump, &leftJump, sizeof(byte), 0);
	if (leftJump == 1)	leftJumping = false;
	else leftJumping = true;

}

void readLeftSliding() {
	byte sliding;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerLeftSliding, &sliding, sizeof(byte), 0);
	if (sliding == 1)	leftSliding = true;
	else leftSliding = false;
}

void readRightSliding() {
	byte sliding;
	ReadProcessMemory(handle, (PBYTE*)a_PlayerRightSliding, &sliding, sizeof(byte), 0);
	if (sliding == 1)	rightSliding = true;
	else rightSliding = false;
}


// --------------------- GET --------------------

double getLeftHasSword() {
	readLeftHasSword();
	return LefthasSword;
}

double getRightHasSword() {
	readRightHasSword();
	return RighthasSword;
}

double getDistancePlayers() {
	readPlayerDistance();
	return distancePlayers;
}

int getDirection() {
	readDirection();
	return direction;
}

double getDistanceOnMapLeft() {
	readDistanceOnMapLeft();
	return distanceMapLeft;
}

double getDistanceOnMapRight() {
	readDistanceOnMapRight();
	return distanceMapRight;
}

float getLeftPlayerPositionX() {
	readLeftPlayerPositionX();
	return leftPlayerPositionX;

}

float getRightPlayerPositionX() {
	readRightPlayerPositionX();
	return rightPlayerPositionX;

}


float getAbsoluteDistance() {
	readAbsoluteDistance();
	return absolutedistancePlayers;

}


float getLeftPlayerPositionY() {
	readLeftPlayerPositionY();
	return leftPlayerPositionY;

}

float getRightPlayerPositionY() {
	readRightPlayerPositionY();
	return rightPlayerPositionY;

}

int getPositionSwordRight() {
	readPositionSwordRight();
	int positionSwordRight = rightPlayerPositionY - playerRightSwordPosY;
	return positionSwordRight;
}

int getPositionSwordLeft() {
	readPositionSwordLeft();
	int positionSwordLeft = leftPlayerPositionY - playerLeftSwordPosY;
	return positionSwordLeft;
}

vector<float> getSwordList() {
	readSwordList();
	return swordList;
}


double getAttackLeft() {
	readLeftAttack();
	return leftAttack;
}

double getAttackRight() {
	readRightAttack();
	return rightAttack;
}

double getLeftDead() {
	readLeftDead();
	return leftLife;
}

double getRightDead() {
	readRightDead();
	return rightLife;
}

bool getLeftJump() {
	readLeftJump();
	return leftJumping;
}

bool getRightJump() {
	readRightJump();
	return rightJumping;
}

bool getLeftSliding() {
	readLeftSliding();
	return leftSliding;
}

bool getRightSliding() {
	readRightSliding();
	return rightSliding;
}


// Executed every frame

void fillLeftWithValue(CharInfo* charinfo) {
	charinfo->attack = getAttackLeft();
	charinfo->jump = getLeftJump();
	charinfo->sliding = getLeftSliding();

	charinfo->previous_position_x = charinfo->position_x;
	charinfo->previous_position_y = charinfo->position_y;
	charinfo->position_x = getLeftPlayerPositionX();
	charinfo->position_y = getLeftPlayerPositionY();

	charinfo->life = getLeftDead();
	charinfo->has_sword = getLeftHasSword();
	charinfo->position_sword = getPositionSwordLeft();

}

void fillRightWithValue(CharInfo* charinfo) {
	charinfo->attack = getAttackRight();
	charinfo->jump = getRightJump();
	charinfo->sliding = getRightSliding();

	charinfo->previous_position_x = charinfo->position_x;
	charinfo->previous_position_y = charinfo->position_y;
	charinfo->position_x = getRightPlayerPositionX();
	charinfo->position_y = getRightPlayerPositionY();

	charinfo->life = getRightDead();
	charinfo->has_sword = getRightHasSword();
	charinfo->position_sword = getPositionSwordRight();

}

void fillGameStateWithValue(GameState* state, bool LeftOrRight)
{
	state->arrowDirection = getDirection() * (LeftOrRight ?(-1):1);
	if (getRightPlayerPositionX() - getLeftPlayerPositionX() >= 0) // check if the enemy stands in front of the goal
		state->distanceToEnemy = getAbsoluteDistance();
	else
		state->distanceToEnemy = -getAbsoluteDistance();



	vector<float> oldSwordList;
	oldSwordList = swordList;
	getSwordList();
	vector<float> flyingSwords;
	for (int i = 0; i < oldSwordList.size(); i++)
	{
		if (fabs(oldSwordList[i] - swordList[i]) >= 0.0001) {
			flyingSwords.push_back(swordList[i]);
		}
	}
	state->flyingSwordPosList = flyingSwords;

	float closest = 500;
	float closestNotAbs = 0;
	for (int i = 0; i < state->flyingSwordPosList.size(); i++)
	{
		if (abs((LeftOrRight?getRightPlayerPositionX():getLeftPlayerPositionX()) - state->flyingSwordPosList[i]) < closest)
		{
			closestNotAbs = ((LeftOrRight ? getRightPlayerPositionX() : getLeftPlayerPositionX()) - state->flyingSwordPosList[i]);
			closest = abs(closestNotAbs);
		}
	}
	state->distanceToNextFlyingSword = closestNotAbs * (LeftOrRight ? (1) : -1);

}

// executed on start
void startTrainingDataGenerator(HWND hwnd) {
		DWORD procID;
		GetWindowThreadProcessId(hwnd, &procID);
		handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);

		if (procID == NULL) { 
			cout << "ERROR, CAN'T OBTAIN PROCESS, START APPLICATION AS ADMINISTRATOR, OTHERWISE PLEASE WRITE THE DEVELOPER" << endl;
			Sleep(3000);
			exit(-1);
		}
		else {
			a_SwordList = new int[13]{ 0x00543508, 0x005433AC, 0x00543250, 0x005430F4, 0x00542F98, 0x00542E3C, 0x00542CE0, 0x00542B84, 0x00542A28, 0x005428CC, 0x00542770, 0x00542614, 0x005424B8 };
		}

}