#pragma once

#ifndef TRAININGDATA_H_
#define TRAININGDATA_H_


#include <iostream>
#include <Windows.h>
#include "Common.h";


void startTrainingDataGenerator(HWND);

void fillLeftWithValue(CharInfo*);
void fillRightWithValue(CharInfo*);
void fillGameStateWithValue(GameState*, bool);

int getDirection();
double getDistanceOnMapLeft();
double getDistanceOnMapRight();
double getDistancePlayers();
double getAttackLeft();
double getAttackRight();
double getRightDead();
double getLeftDead();
float getLeftPlayerPositionX();
float getRightPlayerPositionX();
float getRightPlayerPositionY();
float getLeftPlayerPositionY();
bool getRightSliding();
bool getLeftSliding();

#endif