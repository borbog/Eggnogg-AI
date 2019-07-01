#pragma once

#include <Windows.h>
#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>       

using namespace std;
struct LevelInfo { // info that the bot knows about the level (the only info!) - their X position is used and the area and trigger are defined in AI.cpp
	float forwardStepUp[4];
	float mine[6];
	float pit[10];
	float highstep[4];
	float dontJump[13];
	float largePit[4];

	LevelInfo() :
		forwardStepUp{ 3072, 2735, 704, 5101 } , 
		mine{ 1448, 1322, 1188, 4360, 4488, 4615 } ,
		pit{ 2008, 1954, 1911, 974, 610, 3800, 3850, 3898, 5192, 4845 },
		highstep{ 1856, 1791,  3948, 4017 },
		dontJump{ 2145, 2079, 1610, 1564, 3670, 3727, 4192, 4242, 1074, 1021, 4785, 4745, 4920 },
		largePit{ 822, 760, 4985, 5044 }
	{}

};

class CustomTimer {
public:
	float duration;
	float last_time;
	float maxDuration;
public :
	CustomTimer(float);
	void Update();
	void Reset();

};


 
struct GameState { // infos that the AI knows
	int arrowDirection;
	float distanceToEnemy;
	float distanceToNextFlyingSword;
	vector<float> flyingSwordPosList;
};

struct CharInfo { // info of enemy and of bot that the ai has
	int previous_position_x=0; 
	int previous_position_y=0; 

	int position_x=0; 
	int position_y=0;
	bool has_sword=true;
	int position_sword = 0; // 0 middle // 1 up // -1 down
	int life = 100; 
	bool jump;
	int attack;
	bool sliding;

};

struct GameInfo { // a wrapper for all info that the bot has available
	GameState* gamestate;
	CharInfo* bot;
	CharInfo* enemy;

};

struct BotOutput { // Actions that the AI can do
	string name;

	bool back = false;
	bool forward = false;
	bool down = false;
	bool up = false;
	bool jump = false;
	bool attack = false;

	float time = 0;
};


struct PlayerInput {
	int left;
	int right;
	int down;
	int up;
	int jump;
	int attack;
};


int Random(int , int );
