#pragma once
#include <cstdint>

class Entry
{
private:
	uint16_t _status;
	uint8_t _type;
	uint8_t _dadIndex;
	uint32_t _firstBlockOfData;
	uint32_t _size;
	char _password[16] = { 0 };
	char _filename[20] = { 0 };

public:
	// Constructor
	Entry()
	{
		_status = 0;
		_type = 0;
		_dadIndex = 0;
		_firstBlockOfData = 0;
		_size = 0;
	}

	// Getter - Setter
	uint16_t status() { return _status; }
	void setStatus(uint16_t value) { _status = value; }

	uint8_t type() { return _type; }

	uint8_t dadIndex() { return _dadIndex; }

	uint32_t firstBlockOfData() { return _firstBlockOfData; }

	uint32_t size() { return _size; }
	void setSize(uint32_t value) { _size = value; }

	string password() { return string(_password).substr(0, 16); }
	void setPassword(string value) {
		for (int i = 0; i < 16; i++) {
			_password[i] = value[i];
		}
	}

	char* filename() { return _filename; }

	void setUseMode(uint8_t type, uint8_t dadIndex, uint32_t firstBlockOfData, uint32_t size, string filename)
	{
		_status = 1;
		_type = type;
		_dadIndex = dadIndex;
		_firstBlockOfData = firstBlockOfData;
		_size = size;

		int len = filename.length() < 20 ? filename.length() : 20;
		for (int i = 0; i < len; i++) {
			_filename[i] = filename[i];
		}
	}

	void reset()
	{
		_status = 0;
		_type = 0;
		_dadIndex = 0;
		_firstBlockOfData = 0;
		_size = 0;
		for (int i = 0; i < 16; i++) {
			_password[i] = 0;
		}
		for (int i = 0; i < 20; i++) {
			_filename[i] = 0;
		}
	}
};