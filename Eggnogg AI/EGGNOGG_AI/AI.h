
#pragma once

#ifndef AI_H
#define AI_H

#include "Common.h";




void queueBotActions(GameInfo*, bool, queue <BotOutput>*, string*, bool);
void AIStart();
bool botNear(float* places, int length, int size);
#endif