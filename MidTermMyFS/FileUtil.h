#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Random.h"

#define BLOCK_SIZE 512
#define MAX_JAMS 12		// number of max jams string (noises)
#define MAX_JAM_LEN 42	// if define smaller can cause loop

#pragma warning(disable : 4996)

using namespace std;

class FileUtil
{
public:
	static void readBlock(fstream& fi, streampos readPoint, char* buffer, int size = BLOCK_SIZE)
	{
		fi.seekg(readPoint);
		fi.read(buffer, size);
	}

	static void writeBlock(fstream& fo, streampos writePoint, char* buffer)
	{
		fo.seekg(writePoint);
		fo.write(buffer, BLOCK_SIZE);
	}

	static void writeFile(string path, char* buffer, int size)
	{
		fstream fo(path, ios::out | ios::binary);
		fo.write(buffer, size);
		fo.close();
	}

	//extract file name from path
	static vector<string> getFileName(char path[])
	{
		vector<string> result;

		char* token = strtok(path, "/");
		//result.push_back(token);

		while (token != NULL)
		{
			result.push_back(token);
			token = strtok(NULL, "/");
		}

		return result;
	}

	static string getDataFromFile(char* path)
	{
		string data, line;
		fstream fi(path, ios::in | ios::binary);
		if (fi.is_open())
		{
			while (getline(fi, line)) {
				data += line;
			}
			fi.close();
		}

		return data;
	}

	static bool isExist(const string& path)
	{
		ifstream f(path);
		return f.good();
	}

	static uint16_t nextEntryValue(uint16_t value) // 2 - 256
	{
		if (value == 256) return 2;
		return ++value;
	}

	static uint32_t nextBlockValue(uint32_t value) // 2 - 4 000 000 000
	{
		if (value == 4000000000) return 2;
		return ++value;
	}

	static bool invalidChar(int n)
	{
		if (n == 26 || n == 27 || n <= 0 || (n >= 7 && n <= 10) || n == 13 || n == 32 || n == 255) return true;
		return false;
	}

	// value length = 16 * k
	static void perplexity(string& value)
	{
		int n;
		do {
			n = Random::nextInt(1, value.length() < MAX_JAMS ? value.length() : MAX_JAMS); // Random number of jams
		} while (invalidChar(n));

		int index;
		int jamLength;
		string indexs = "";
		string jamsLength = "";
		string jam;

		bool flag = false;

		for (int i = 0; i < n; i++) {
			do {
				index = Random::nextInt(value.length() < 256 ? value.length() : 256); // Random position of jams
			} while (invalidChar(index));

			do {
				jamLength = Random::nextInt(1, MAX_JAM_LEN); // Random jams length
				if ((i == n - 1) && ((jamLength + value.length() + 2 * n + 1) % 16 != 0)) {
					flag = true;
				}
				else flag = false;
			} while (invalidChar(jamLength) || flag);

			jam = Random::nextString(jamLength); // Random jams

			value.insert(index, jam);

			indexs += index;
			jamsLength += jamLength;
		}

		value = char(n) + indexs + jamsLength + value;
	}

	static void clear(string& value)
	{
		unsigned char n = value[0];
		string header = value.substr(0, 2 * n + 1);
		value = value.substr(2 * n + 1, value.length() - (2 * n + 1));

		for (int i = n; i >= 1; i--) {
			value.erase(header[i] > 0 ? header[i] : header[i] + 256, header[i + n] > 0 ? header[i + n] : header[i + n] + 256);
		}
	}
};