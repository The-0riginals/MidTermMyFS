#pragma once
#include <iostream>

using namespace std;

class Random {
private:
	static bool _seeded;
public:
	static int nextInt() {
		if (!_seeded) {
			srand(time(NULL));
			_seeded = true;
		}
		return rand();
	}

	static int nextInt(int ceiling) {
		return nextInt() % ceiling;
	}

	static int nextInt(int left, int right) {
		int delta = right - left + 1;
		return nextInt(delta) + left;
	}

	static string nextString(int length) {
		string result = "";
		for (int i = 0; i < length; i++) {
			result += nextInt(33, 126);
		}
		return result;
	}
};
bool Random::_seeded = false;