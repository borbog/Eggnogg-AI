#include "Common.h"



int Random(int from, int to ) {
	return rand() % ((to+1)-from) + from;

}


CustomTimer::CustomTimer(float dur) {
	duration = dur;
	last_time = 0;
	maxDuration = dur;
}

void CustomTimer::Update() {
	duration -= (double)(clock() - last_time) / 1000;
	last_time = clock();
}

void CustomTimer::Reset() {
	duration = maxDuration;
}
