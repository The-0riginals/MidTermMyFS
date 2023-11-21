#pragma once
#include <cstdint>
#include <string>

using namespace std;

class VolumeInfo
{
private:
	uint16_t _signature;
	char _password[16] = { 0 };
	uint16_t _firstEntryDelete;
	uint32_t _size;
	uint32_t _firstBlockDelete;
	uint32_t _nextBlockDelete;
	uint16_t _nextEntryDelete;
	char _reserve[62] = { 0 }; // Reserve to encrypt

public:
	// Constructor
	VolumeInfo()
	{
		_signature = 0x6666;
		_firstEntryDelete = 0xCCCC;
		_size = 0xEEEEEEEE;
		_firstBlockDelete = 0xAAAAAAAA;
		_nextBlockDelete = 0XBBBBBBBB;
		_nextEntryDelete = 0XDDDD;
	}

	VolumeInfo(uint32_t size)
	{
		_signature = 0x6666;
		_firstEntryDelete = 2;
		_size = size;
		_firstBlockDelete = 2;
		_nextBlockDelete = 2;
		_nextEntryDelete = 2;
	}

	// Getter - Setter
	uint16_t signature() { return _signature; }

	string password() { return string(_password).substr(0, 16); }
	void setPassword(string value) {
		for (int i = 0; i < 16; i++) {
			_password[i] = value[i];
		}
	}

	uint16_t firstEntryDelete() { return _firstEntryDelete; }
	void setFirstEntryDelete(uint16_t value) {
		_firstEntryDelete = value;
	}

	uint32_t size() { return _size; }

	uint16_t nextEntryDelete() { return _nextEntryDelete; }
	void setNextEntryDelete(uint16_t value) {
		_nextEntryDelete = value;
	}

	char* reserve() { return _reserve; }

	uint32_t firstBlockDelete() { return _firstBlockDelete; }
	void setFirstBlockDelete(uint32_t value) {
		_firstBlockDelete = value;
	}

	uint32_t nextBlockDelete() { return _nextBlockDelete; }
	void setNextBlockDelete(uint32_t value) {
		_nextBlockDelete = value;
	}
};