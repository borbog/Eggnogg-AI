#include "AI.h";
string stateBot;
queue<BotOutput>* OutputQueue;
queue<BotOutput>* NewOutputQueue;
vector <int> scores;
GameInfo* gameInfo;
bool AIfinishedActionLocal;
LevelInfo* levelInfo;
CustomTimer* StuckTimer;
string localState;


bool hasState(string s) { // check if something is currently executing (not queued for next frame!)
	return (stateBot.find(s) != string::npos);
}


void makeAction(string action, float time) { // create an action of key presses which last a "time" amount.
	BotOutput bot_out;
	bot_out.name = action;
	
	if(action == "_runForward") {
		bot_out.forward = true;
	}
	else if (action == "_runBack") {
		bot_out.back = true;
	}
	else if (action == "_jump") {
		bot_out.jump = true;
	}
	else if (action == "_jumpForward") {
		bot_out.jump = true;
		bot_out.forward = true;
	}
	else if (action == "_jumpBack") {
		bot_out.jump = true;
		bot_out.back = true;
	}
	else if (action == "_attack") {
		bot_out.attack = true;
	}
	else if (action == "_attack_throw_back") {
		bot_out.back = true;
		bot_out.attack = true;
	}
	else if (action == "_attack_throw_forward") {
		bot_out.forward = true;
		bot_out.attack = true;
	}
	else if (action == "_up") {
		bot_out.up = true;
	}
	else if (action == "_down") {
		bot_out.down = true;
	}
	else if (action == "_downForward") {
		bot_out.down = true;
		bot_out.forward = true;
	}

	else if (action == "_duck") {
		bot_out.down = true;
		bot_out.jump = true;
	}
	
	bot_out.time = time;
	NewOutputQueue->push(bot_out);

}



void execute() { // queue the new actions if the ai is done with the previous ones
	if (AIfinishedActionLocal) {
		while (NewOutputQueue->size() > 0) {
			OutputQueue->push(NewOutputQueue->front());
			NewOutputQueue->pop();
		}
	}
}

void stopActions(string name) {  //atop all actions (and queued) from the AI
	queue<BotOutput> tempqueue;
	//delete from running actions
	while (!OutputQueue->empty()) {
		if (OutputQueue->front().name.find(name) == string::npos)
			tempqueue.push(OutputQueue->front());
		OutputQueue->pop();
	}

	while (!tempqueue.empty()) {
			OutputQueue->push(tempqueue.front());
			tempqueue.pop();
	}

	//delete from new queue of actions
	while (!NewOutputQueue->empty()) {
		
		if (NewOutputQueue->front().name.find(name) == string::npos)
			tempqueue.push(NewOutputQueue->front());
		NewOutputQueue->pop();
	}

	while (!tempqueue.empty()) {
		NewOutputQueue->push(tempqueue.front());
		tempqueue.pop();
	}


}

// --------- Calculate scores
int getScore_RunToGoal() {
	int score = (
		gameInfo->gamestate->arrowDirection * 50 + // when you can run forward
		(gameInfo->enemy->life==0?50:0) + // when the other player is dead
		(botNear(levelInfo->pit, (sizeof(levelInfo->pit) / sizeof(levelInfo->pit[0])), 13)? 60:0) + // when there is a pit nearby
		(botNear(levelInfo->largePit, (sizeof(levelInfo->largePit) / sizeof(levelInfo->largePit[0])), 30) ? 60 : 0) + // when ther eis a large pit nearby
		(botNear(levelInfo->mine, (sizeof(levelInfo->mine) / sizeof(levelInfo->mine[0])), 10) ? 60 : 0) // when there is a mine nearby

		);
	return score;
}

int getScore_Attack() {
	int score = (
		(localState == "aggressiveaggressive" ? 60 : 0) +
		(gameInfo->gamestate->arrowDirection == 0 ? 50:0)+
		(gameInfo->gamestate->arrowDirection == -1 ? 50 : 0)+
		(botNear(levelInfo->pit, (sizeof(levelInfo->pit) / sizeof(levelInfo->pit[0])), 13) ? -30 : 0) +
		(botNear(levelInfo->largePit, (sizeof(levelInfo->largePit) / sizeof(levelInfo->largePit[0])), 30) ? -60 : 0) +
		(botNear(levelInfo->mine, (sizeof(levelInfo->mine) / sizeof(levelInfo->mine[0])), 10) ? -45 : 0)

		);
	return score;
}

// --------- Calculate actions to execute
void queueAction_RunToGoal() {
	
	if (!hasState("_attack"))
	{
		makeAction("_runForward", 0); // when running to goal, goal is always in front

		if (abs(gameInfo->enemy->position_y - gameInfo->bot->position_y) < 60 && (gameInfo->gamestate->distanceToEnemy) > 44) { // if the enemy is close (on the y axis aswell) and enemy attacks
			if (gameInfo->enemy->attack) {
				if (!hasState("_jump")) {
					makeAction("_jumpForward", 0.1f);
				}
			}
		}
		if(abs(gameInfo->enemy->position_y - gameInfo->bot->position_y) < 45){ // when enemy not above too high
			
			if (gameInfo->gamestate->distanceToEnemy < 80 && gameInfo->gamestate->distanceToEnemy > 75) // a bit infront of player
			{
				if (!hasState("_attack") && gameInfo->bot->has_sword) { // bot with sword
					int attackType = Random(0, 5) + (gameInfo->enemy->has_sword ? 1 : 0); // throw sword more likely when enemy has a sword too.

					if (attackType >= 5) { // when enemy in front of the bot, throw sword
						makeAction("_attack_throw_forward", 0.5f);
					}
				}
			}
			
			if ((gameInfo->gamestate->distanceToEnemy) < 45 && (gameInfo->gamestate->distanceToEnemy) > -5) { // enemy closeby and in the bots way to run to the goal  |  either jump over him or slide under him
				if (gameInfo->enemy->life > 0) { // no point in evading when enemy is dead
					if (!gameInfo->bot->jump && !hasState("_duck")) { 
						int randNum = Random(0, 2);
						randNum = 1;
						if (randNum == 0 && gameInfo->enemy->position_sword <= 0) { // if sword is not pointing up
							if (!hasState("_jump")) {
								makeAction("_jumpForward", 0.1f);
								makeAction("_downForward", 0.5f);
							}
						}
						else if (randNum == 1) {
							if (abs(gameInfo->gamestate->distanceToEnemy) > 5 && gameInfo->enemy->position_sword >= 0) { // if the sword is not pointing down, then slide, otherwise jump
								if (!hasState("_duck")) {
									makeAction("_duck", 0.42f);
								}
							}
							else {
								if (!hasState("_jump")) {
									makeAction("_jumpForward", 0.1f);
									makeAction("_downForward", 0.5f);
								}
							}
						}
						else { // if enemy sword is up or random = 2  | attack enemy
							localState = "aggressive";
						}
					}
				}
			}
		}

		if (gameInfo->gamestate->distanceToEnemy > -45 && gameInfo->gamestate->distanceToEnemy < 0) // Running away (to goal), and back is turned to enemy -> jump so if he attacks he doesnt hit
		{

			if (!hasState("_jump")) { // jump so if the enemy attacks, he misses
				makeAction("_jump", 0.1f);
			}
			if (gameInfo->enemy->attack ) { 
					if (gameInfo->bot->has_sword) { // trying to block the attack if has sword
						int attackType = Random(0, 6); // throw sword more likely when enemy has a sword too.
						if (attackType >= 5) { // when enemy in front of the bot, throw sword
							stopActions("_runForward");
							stopActions("_jump");
							makeAction("_runBack", 0.1f);
						}
					}
					else makeAction("_runForward", 0.4f);
			}

		}

	}

}

void queueAction_Attack() {
	if(gameInfo->enemy->life<0) localState = "normal"; // after being aggressive, and killing the enemy, calm down to normal behavior

	if (abs(gameInfo->enemy->position_y - gameInfo->bot->position_y) < 45) { // player not above or under enemy


		if (gameInfo->gamestate->distanceToEnemy >= 40) { // in front of the player

			makeAction("_runForward", 0); // run towards the player

		}
		if (gameInfo->gamestate->distanceToEnemy < 80 && gameInfo->gamestate->distanceToEnemy > 75) // a bit infront of player
		{
			if (!hasState("_attack") && gameInfo->bot->has_sword) { // bot with sword
				int attackType = Random(0, 5) + (gameInfo->enemy->has_sword ? 1 : 0); // throw sword more likely when enemy has a sword too.
				cout << attackType << endl;

				if (attackType >= 5 ) { // when enemy in front of the bot, throw sword
					makeAction("_attack_throw_forward", 0.5f);
				}
			}
		}
		if (gameInfo->gamestate->distanceToEnemy < 60 && gameInfo->gamestate->distanceToEnemy > -28) { // nearby


				
			if(Random(0,5)==5 && gameInfo->gamestate->distanceToEnemy < 60 && gameInfo->gamestate->distanceToEnemy > 40){ // jump if enemy far. it helps when the enemy tries jumping over the bot
				int rand = Random(0,1);
				 if (rand == 1) { // aim either up or down when in air
					 stopActions("_down");
					 if (!hasState("_up"))
						 makeAction("_up", (float)Random(0, 5) / 20.0f);
				 }
				 if (rand == 0)
				 {
					 stopActions("_up");
					 if (!hasState("_down"))
						 makeAction("_down", (float)Random(0, 5) / 20.0f);
				 }

				if (!hasState("_jump")) { // make a jump attack. hopefully the enemy wants to jump over the bot and lands directly on its sword
					makeAction("_jump", 0);
					makeAction("_attack", 0);
				}
			}

			else if (!hasState("_attack")) // normal attacking behavior
			{
				if (gameInfo->bot->has_sword) { // bot with sword
					
						if (!hasState("_up") && !hasState("_down"))
						{

							if (gameInfo->enemy->sliding && gameInfo->enemy->life > 0) { // if the enemy is sliding, just always attack him from the bottom
								stopActions("_up");
								if (!hasState("_down"))
									makeAction("_down", 0.2f);
							}
							else if (gameInfo->enemy->position_sword <= 0)  // enemy sword at the top
							{
								if (gameInfo->enemy->attack) { // if enemy attacks, put sword down, to defend from his down attack
									if (!hasState("_down"))
										makeAction("_down", 0.2f);
								}
								else { // if enemy does not attack, and his sword is at the bottom, attack him from the top
									if (!hasState("_up"))
									makeAction("_up", 0.2f);
								}
							}
							else {  // enemy sword at the bottom
								if (gameInfo->enemy->attack) {
									if (!hasState("_up"))
										makeAction("_up", 0.2f);
								}
								else {
									if (!hasState("_down"))
										makeAction("_down", 0.2f);
								}
							}
						}

						if (gameInfo->gamestate->distanceToEnemy >= 0)	//face the enemy
							makeAction("_runForward", 0);
						else
							makeAction("_runBack", 0);

						if(!gameInfo->bot->attack)
							makeAction("_attack", 0); 
				}
				else if (!gameInfo->bot->has_sword) { // bot doesnt have a sword
					if (abs(gameInfo->gamestate->distanceToEnemy) < 45) { // closeby

						if (!hasState("_jump"))
						{

							if (gameInfo->gamestate->distanceToEnemy >= 0) // jump towards the enemy (a strategy to jump and attack to make the enemy lose his sword)
							{
								makeAction("_jumpForward", 0.1f);
							}
							else {
								makeAction("_jumpBack", 0.1f);
							}

						}
						else {
							if (!hasState("_down")) // face down when jumping - useful to grab the sword after beating it out of the enemies hand
								makeAction("_down", 0.1f);

						}

						if(gameInfo->gamestate->distanceToEnemy >= 0) // face the enemy
							makeAction("_runForward", 0);
						else 
							makeAction("_runBack", 0);

						makeAction("_attack", 0);
					}
				}
			}
		}
		if(gameInfo->gamestate->distanceToEnemy <= -28) { // behind the player (enemy running towards its goal) // if bot with sword, then prefer to throw it than attack. if bot without sword, he is faster, so he can try running after
			if (gameInfo->bot->has_sword) {
				if (!gameInfo->bot->attack) // execute throwing 
				{
					stopActions("_run");

					int randNum = Random(0, 2);
					if (randNum > 0)
						stopActions("_jump");

					//aim depending on the distance to the enemy
					if(gameInfo->enemy->jump)
						makeAction("_up", 0.1f);

					else if (gameInfo->gamestate->distanceToEnemy <= -125)
						makeAction("_up", 0.2f);

					else if (gameInfo->gamestate->distanceToEnemy <= -75)
						makeAction("_up", 0.1f);


					makeAction("_runBack", 0.2f);
					makeAction("_attack_throw_back", 0.5f);
				}
			}
			else { // no sword
				if ((gameInfo->enemy->has_sword && gameInfo->gamestate->arrowDirection == -1) || (gameInfo->gamestate->distanceToEnemy < -40 && gameInfo->gamestate->arrowDirection == -1)) { // far and enemy can run to goal
					makeAction("_runForward", 0);  // run back to respawn again in front of enemy's direction
				}
				else {
					makeAction("_runBack", 0); // run after the enemy (to maybe punch him)
				}
			}
		}
	}
	else { // if enemy out of sight on the Y axis - do random things until in sight again
		if (!hasState("_runBack") && !hasState("_runForward")) {
			if(Random(0,1) == 1)
				makeAction("_runBack", Random(0, 150) * 0.005f);
			else
				makeAction("_runForward", Random(0, 150) * 0.005f);

		}
		if (!hasState("_jump")) {
				makeAction("_jump", 0);
		}
	}
}

bool botNear(float* places, int length, int size) { // check if near an important area on the map (like spikes)
	for (int i = 0; i < length; i++) {
		if (gameInfo->bot->position_x > places[i] - size && gameInfo->bot->position_x < places[i] + size) {
			return true;
		}
	}
	return false;
}

// FOR LEVEL INTERACTION
void checkLevel() {
	if (botNear(levelInfo->dontJump, (sizeof(levelInfo->dontJump)/ sizeof(levelInfo->dontJump[0])), 40)) { // the number indicates how wide the area around the point should be a trigger
		stopActions("_jump");
	}
	if (botNear(levelInfo->pit, (sizeof(levelInfo->pit) / sizeof(levelInfo->pit[0])), 25)) {
		if (!hasState("_jump")) {
			makeAction("_jump",0);
		}
	}
	if (botNear(levelInfo->forwardStepUp, (sizeof(levelInfo->forwardStepUp) / sizeof(levelInfo->forwardStepUp[0])), 25)) {
		if (!hasState("_jump")) {
			makeAction("_jump", 0);
		}
	}
	if (botNear(levelInfo->highstep, (sizeof(levelInfo->highstep) / sizeof(levelInfo->highstep[0])), 25)) {
		if (!hasState("_jump")) {
			makeAction("_jump", 0);
		}
	}
	if (botNear(levelInfo->largePit, (sizeof(levelInfo->largePit) / sizeof(levelInfo->largePit[0])), 25)) {
		if (!hasState("_jump")) {
			makeAction("_jump", 0);
		}
	}
	


	if (gameInfo->gamestate->flyingSwordPosList.size() > 0 && abs(gameInfo->gamestate->distanceToNextFlyingSword) < 50) // check if any of the flying around swords are close to the bot
	{
		stopActions("_attack");
		if (!hasState("_jump") && !hasState("_duck")) {

			int i = Random(0, 4);
			if (i == 0) { // 25% chance that bot ducks the flying sword
				if (gameInfo->enemy->life > 0) {
					makeAction("_duck", 0.5f);
					goto end;
				}
				else {
					if (!hasState("_jump")) {
						makeAction("_jump", 0);
					}
					goto turnToSword;
				}
			}
			else {
				if (!hasState("_jump")) {
					makeAction("_jump", 0);
				}
				goto turnToSword;
			}
		}
		else {
			goto end;
		}

	turnToSword: // only when with sword, and jumping, trying to block it with own sword
		if (gameInfo->enemy->life > 0) {
			stopActions("_runForward");
			stopActions("_runBack");

			if (gameInfo->gamestate->distanceToNextFlyingSword < 0 && gameInfo->bot->has_sword)
				makeAction("_runBack", 0);
			else
				makeAction("_runForward", 0);
		}
	}
end:;
}

void checkTimers() {
	// a stuck timer. f.e. when jumped on a wall, and not sliding down, but trying to continue running to the goal
	if (gameInfo->bot->position_x == gameInfo->bot->previous_position_x && !gameInfo->bot->attack) {
		StuckTimer->Update();
		if (StuckTimer->duration < 0) {
			if (!hasState("_jump")) {
				makeAction("_jump", 0);
			}
			if (!hasState("_run")) {
					makeAction("_runBack", 0.5f);
					makeAction("_runForward", 0.5f);
			}
		}
	}
	else  {
		StuckTimer->Reset();

	}

}

void AIStart() {
	levelInfo = new LevelInfo();
	StuckTimer = new CustomTimer(0.38f);
}


void queueBotActions(GameInfo* info, bool botLeftOrRight, queue<BotOutput>* OutputQueue_temp, string* statelist, bool AIfinishedAction) {
	stateBot = *statelist;
	OutputQueue = OutputQueue_temp;
	NewOutputQueue = new queue<BotOutput>();
	gameInfo = info;
	AIfinishedActionLocal = AIfinishedAction;


	scores.clear();

	if (gameInfo->bot->life >= 0) { // don tneed to do anything when bot dead

	scores.push_back(getScore_RunToGoal());
	scores.push_back(getScore_Attack());

	//utility - calculate score of what to execute
	int maxScore = -999999;
	int index = -1;
	for(int i = 0; i < scores.size(); i++)
	{
		if (scores[i] > maxScore) {
			maxScore = scores[i];
			index = i;
		}
	}

	//decision tree
	switch (index) {
	case 0: queueAction_RunToGoal(); break;
	case 1: queueAction_Attack();  break;

	}

	checkLevel();  //check if there are things to consider in the level (like mines)
	checkTimers(); // check stuck timer
	
	}

	execute();

}