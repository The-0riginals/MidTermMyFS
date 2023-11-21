#pragma once
#include <cstdint>

#define DATA_SIZE 504

class Data
{
private:
	uint32_t _status;
	uint32_t _nextBlock;
	char _data[DATA_SIZE] = { 0 };

public:
	// Constructor
	Data()
	{
		_status = 0;
		_nextBlock = 0;
	}

	// Getter - Setter
	uint32_t status() { return _status; }
	void setStatus(uint32_t value) { _status = value; }

	uint32_t nextBlock() { return _nextBlock; }
	void setNextBlock(uint32_t value) { _nextBlock = value; }

	char* data() { return _data; }
	void setData(char* value, int len)
	{
		for (int i = 0; i < len; i++) {
			_data[i] = value[i];
		}
	}

	void resetData()
	{
		for (int i = 0; i < DATA_SIZE; i++) {
			_data[i] = 0;
		}
	}

	void reset()
	{
		_status = 0;
		_nextBlock = 0;
		for (int i = 0; i < DATA_SIZE; i++) {
			_data[i] = 0;
		}
	}
};