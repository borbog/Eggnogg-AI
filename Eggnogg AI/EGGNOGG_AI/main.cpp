
#include "main.h";
#include "TrainingData.h";
#include "AI.h";


#include <string>

CharInfo Bot_Info;
CharInfo Enemy_Info;
GameInfo Game_Info;
GameState Game_State;

bool botActive;
bool autoRestart;

bool DebugInputOutput;
int gameTime;

queue <BotOutput>* OutputList;



PlayerInput bot_input;

void assignInput() {
	
	system("cls");
	char letter;

	cout << "Is the enemy going to be Left or Right? (L/R) (then press enter)" << endl;
	inputagain:

	(cin >> letter).get();
	if (letter == 'r' || letter == 'R')
		botLeftOrRight = true;
	else if (letter == 'l' || letter == 'L')
		botLeftOrRight = false;
	else {
		cout << "Wrong input, try again - (L/R)?" << endl;
		goto inputagain;
	}
	
	system("cls");
	cout << "READY TO FIGHT! press F4 to start the bot, press F6 to stop the bot." << endl << endl;

	if (botLeftOrRight) {
		cout << "Change the inputs for the right character (PLAYR 2) in the option menu to:" << endl << endl;

		bot_input.left = 0x21;
		bot_input.right = 0x23;
		bot_input.down = 0x22;
		bot_input.up = 0x14;
		bot_input.jump = 0x16;
		bot_input.attack = 0x15;
	}
	else
	{
		cout << "Change the inputs for the left character (PLAYR 1) in the option menu to:" << endl << endl;
		//WASD VB
		bot_input.left = 0x1E;
		bot_input.right = 0x20;
		bot_input.down = 0x1F;
		bot_input.up = 0x11;
		bot_input.jump = 0x13;
		bot_input.attack = 0x12;
	}
	
	cout << "UP: " << (char)MapVirtualKeyA(bot_input.up, 1) << endl;
	cout << "LEFT: " << (char)MapVirtualKeyA(bot_input.left, 1) << endl;
	cout << "DOWN: " << (char)MapVirtualKeyA(bot_input.down, 1) << endl;
	cout << "RIGHT: " << (char)MapVirtualKeyA(bot_input.right, 1) << endl;
	cout << "ATTACK: " << (char)MapVirtualKeyA(bot_input.attack, 1) << endl;
	cout << "JUMP: " << (char)MapVirtualKeyA(bot_input.jump, 1) << endl;


	/*
	INPUT ips[1];
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wVk = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
	ip.ki.dwFlags = KEYEVENTF_SCANCODE; // 0 for key press

	ip.ki.wScan = bot_input.right; //A //instead of left and right, forward and backward for the AI.cpp, so it is easier to write an ai for both characters
	ips[0] = ip;
	SendInput(1, ips, sizeof(INPUT));
	*/


	// manual input - but better not to, because it does not recognize some letters on the keyboard as input
	/*
	cout << "Now press the input buttons that are defined in the game (then press enter)" << endl;

	cout << "Up" << endl;
	(cin >> letter).get();
	bot_input.up = OemKeyScan(letter) & 0x0ff;

	cout << "Left" << endl;
	(cin >> letter).get();
	bot_input.left = OemKeyScan(letter) & 0x0ff;

	cout << "Down" << endl;
	(cin >> letter).get();
	bot_input.down = OemKeyScan(letter) & 0x0ff;

	cout << "Right" << endl;
	(cin >> letter).get();
	bot_input.right = OemKeyScan(letter) & 0x0ff;

	cout << "Jump" << endl;
	(cin >> letter).get();
	bot_input.jump = OemKeyScan(letter) & 0x0ff;

	cout << "Attack" << endl;
	(cin >> letter).get();
	bot_input.attack= OemKeyScan(letter) & 0x0ff;
	
	system("cls");
	cout << "READY TO FIGHT! press F4 to start the bot, press F6 to stop the bot.";
	*/
	//UHJK IO
	/*bot_input.left = 0x23;
	bot_input.right = 0x25;
	bot_input.down = 0x24;
	bot_input.up = 0x16;
	bot_input.jump = 0x17;
	bot_input.attack = 0x18;
	botLeftOrRight = true;
	*/

	//WASD VB
	/*bot_input.left = 0x1E;
	bot_input.right = 0x20;
	bot_input.down = 0x1F;
	bot_input.up = 0x11;
	bot_input.jump = 0x2F;
	bot_input.attack = 0x30;*/
	

}



clock_t this_time;
clock_t last_time;
double deltaTime;
bool timerPaused;
void timer() {
	if (!timerPaused) {
	this_time = clock();
	deltaTime = (double)(this_time - last_time) / 1000;
	last_time = this_time;
	}
}


void timerCheck() {
	timer();

}



HWND WaitUntilProgramInFocus() {
	HWND hwnd = FindWindowA(NULL, "eggnoggplus");
	if (hwnd == NULL)
	{
		cout << "WAITING UNTIL EGGNOGG PLUS IS STARTED..." << endl;
	}

	while (hwnd == NULL) {
		Sleep(1000);
		hwnd = FindWindowA(NULL, "eggnoggplus");
	}
	return hwnd;
}

void resetTimer() {
	this_time = last_time = clock();
	deltaTime = 0; // timer
	timerPaused = false;
	gameTime = 0;
}


void pressBotsOutputs() { // this is what simulates the key presses
getNewOutputElement:
	INPUT ips[6];
	vector<bool> OutputQueue;
	stateListBot = "";

	if (OutputList->size() == 0) {
		for (int i = 0; i < 6; i++) OutputQueue.push_back(false);
		AIfinishedAction = true;  // wether the AI can make inputs or not (if not finished with previous ones, cannot do new ones)
	}
	else {

		BotOutput output = OutputList->front();
		AIfinishedAction = false;

	addNewActionForThisFrame:

		
		stateListBot += (OutputList->front().name);
		OutputList->front().time -= deltaTime;

		//if there are dublicates of actions
		(output.back) |= OutputList->front().back;
		(output.forward) |= OutputList->front().forward;
		(output.down) |= OutputList->front().down;
		(output.up) |= OutputList->front().up;
		(output.jump) |= OutputList->front().jump;
		(output.attack) |= OutputList->front().attack;

		if (OutputList->size() > 0) {
			if (OutputList->front().time < 0) { // if the action ends this frame, execute next one
				OutputList->pop();
				AIfinishedAction = true;

				if (OutputList->size() > 0)
					goto addNewActionForThisFrame;
			}
		}
		else { // no actions left to execute
			AIfinishedAction = true;
		}

	OutputQueue.push_back(output.back);
	OutputQueue.push_back(output.forward);
	OutputQueue.push_back(output.down);
	OutputQueue.push_back(output.up);
	OutputQueue.push_back(output.jump);
	OutputQueue.push_back(output.attack);



	}


	for (int i = 0; i < OutputQueue.size(); i++) { // key press simulation
		INPUT ip;
		ip.type = INPUT_KEYBOARD;
		ip.ki.wVk = 0;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		if (OutputQueue[i]) {
			ip.ki.dwFlags = KEYEVENTF_SCANCODE; // 0 for key press
			switch (i) {
			case 0: ip.ki.wScan = (botLeftOrRight? bot_input.right: bot_input.left); break; //A //instead of left and right, forward and backward for the AI.cpp, so it is easier to write an ai for both characters
			case 1: ip.ki.wScan = (botLeftOrRight ? bot_input.left : bot_input.right); break; //D
			case 2: ip.ki.wScan = bot_input.down; break; //S
			case 3: ip.ki.wScan = bot_input.up; break; //W
			case 4: ip.ki.wScan = bot_input.jump; break; //V - Jump
			case 5: ip.ki.wScan = bot_input.attack; break; //B -Attack
			}
			ips[i] = ip;
		}
		else {
			ip.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE; // 0 for key press
			switch (i) {
			case 0: ip.ki.wScan = (botLeftOrRight ? bot_input.right : bot_input.left); break; //A
			case 1: ip.ki.wScan = (botLeftOrRight ? bot_input.left : bot_input.right); break; //D
			case 2: ip.ki.wScan = bot_input.down; break; //S
			case 3: ip.ki.wScan = bot_input.up; break; //W
			case 4: ip.ki.wScan = bot_input.jump; break; //V - Jump
			case 5: ip.ki.wScan = bot_input.attack; break; //B -Attack
			}
			ips[i] = ip;
		}
	}
	SendInput(6, ips, sizeof(INPUT));

}

void main() {
	srand(time(NULL));
	startTrainingDataGenerator(WaitUntilProgramInFocus());
	assignInput();
	autoRestart = false;

restart: // when pressing F5 in the game, restarting the level
	stateListBot = "";
	AIStart();
	AIfinishedAction = true;
	Game_Info.bot = &Bot_Info;
	Game_Info.enemy = &Enemy_Info;
	Game_Info.gamestate = &Game_State;


	
	OutputList = new queue<BotOutput>();







	while (true)
	{
		if (botActive == false && GetAsyncKeyState(VK_F4)) { //start enemy play
			botActive = true;
		}
		else if (botActive == true && GetAsyncKeyState(VK_F6)) { // stop enemy play
			botActive = false;
			OutputList->empty(); // release all buttons. otherwise the pressed keys in the moment of stop, will continue being simulated by the computer machine.
			pressBotsOutputs();

		}
		else if (GetAsyncKeyState(VK_F5)) { // stop enemy play
			goto restart;
		}
		if (!F7Pressed && GetAsyncKeyState(VK_F7)) {
			autoRestart = !autoRestart; 
			F7Pressed = true;
		}
		else if (F7Pressed == true && !GetAsyncKeyState(VK_F7)) {
			F7Pressed = false;
		}

		// always check if the window is still in focus
		HWND hwnd = GetForegroundWindow();
		wchar_t wnd_title[16];
		wchar_t wnd_titleEggnogg[16];
		GetWindowText(hwnd, wnd_title, sizeof(wnd_title));
		wcsncpy(wnd_titleEggnogg, L"eggnoggplus", 16);
		
		
		timerCheck();

		
		if (wcscmp(wnd_title, wnd_titleEggnogg) == 0 && botActive == true) { // virutal keypresses are executed only when eggnogg is in focus and the bot is active
			


			// current state of enemy and bot for the AI.cpp to work with
			if (botLeftOrRight) { // bot is right
				fillRightWithValue(Game_Info.bot);
				fillLeftWithValue(Game_Info.enemy); // if the bot is right, then the enemy is left
			}
			else { // bot is left
				fillLeftWithValue(Game_Info.bot);
				fillRightWithValue(Game_Info.enemy);
			}

			fillGameStateWithValue(Game_Info.gamestate, botLeftOrRight);


			// queue up the actions which the bot wants to make, depending on the state the enemy and bot are in
			queueBotActions(&Game_Info, botLeftOrRight, OutputList, &stateListBot, AIfinishedAction);


			//execute actions
			pressBotsOutputs();

			


			// auto restart when at the end (usefull when both are AI)
			
			if (autoRestart && (getRightPlayerPositionX() < 440 || getLeftPlayerPositionX() > 5370))
			{
				INPUT ip;
				ip.type = INPUT_KEYBOARD;
				ip.ki.wVk = 0;
				ip.ki.time = 0;
				ip.ki.dwExtraInfo = 0;

				ip.ki.dwFlags = KEYEVENTF_SCANCODE; // 0 for key press
				ip.ki.wScan = 0x3F;  //F5
				SendInput(1, &ip, sizeof(INPUT));

				Sleep(50);

				ip.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE; // 0 for key press
				ip.ki.wScan = 0x3F;  //F5

				SendInput(1, &ip, sizeof(INPUT));

			}
			
			cout << "BOT RUNNING! STOP WITH F6 | TOGGLE AUTO-RESTART WHEN WON WITH F7" << endl << (autoRestart?"autorestart : ON":"autorestart : OFF") << endl;
			
			Sleep(40);

			system("cls"); // clear console

		}
		else {

			Sleep(40);

		}
	
	}

}