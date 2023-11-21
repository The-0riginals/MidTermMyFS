#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "VolumeInfo.h"
#include "Entry.h"
#include "Data.h"
#include "FileUtil.h"
#include "MD5.h"
#include "AES.h"

#define NUM_ENTRY 30	// number of entries, must be 32 * k - 2
#define ENTRY_SIZE 48	// size of an entry (bytes), can't change
#define PADDING MAX_JAMS * (MAX_JAM_LEN + 2) + 1	// reserve to encrypt

using namespace std;

class Volume
{
private:
	VolumeInfo* _volumeInfo;
	vector<Entry*> _entryTable;
	vector<Data*> _dataTable;

	string _path;

public:
	// Constructor
	Volume(const string& path)
	{
		_volumeInfo = new VolumeInfo();

		for (int i = 0; i < NUM_ENTRY; i++) {
			_entryTable.push_back(new Entry());
		}

		_path = path;
	}

	Volume(uint32_t size, const string& path)
	{
		_volumeInfo = new VolumeInfo(size);

		for (int i = 0; i < NUM_ENTRY; i++) {
			_entryTable.push_back(new Entry());
		}

		int dataSize = ceil((size * 1024.0 - (NUM_ENTRY + 2) * ENTRY_SIZE) / BLOCK_SIZE);
		for (int i = 0; i < dataSize; i++) {
			_dataTable.push_back(new Data());
		}

		_path = path;
	}

	// Getter - Setter
	VolumeInfo* volumeInfo() { return _volumeInfo; }
	vector<Entry*> entryTable() { return _entryTable; }
	vector<Data*> dataTable() { return _dataTable; }

	// Method
	void writeVolume()
	{
		fstream fo(_path, ios::out | ios::binary);

		// Write Volume Info
		FileUtil::writeBlock(fo, 0, (char*)_volumeInfo);

		// Write Entry Table
		for (int i = 0; i < _entryTable.size(); i++) {
			FileUtil::writeBlock(fo, (i + 2) * ENTRY_SIZE, (char*)_entryTable[i]);
		}

		// Write Data Table
		for (int i = 0; i < _dataTable.size(); i++) {
			FileUtil::writeBlock(fo, (NUM_ENTRY + 2) * ENTRY_SIZE + i * BLOCK_SIZE, (char*)_dataTable[i]);
		}

		fo.close();
	}

	void readVolume()
	{
		fstream fi(_path, ios::in | ios::binary);

		// Read Volume Info
		FileUtil::readBlock(fi, 0, (char*)_volumeInfo, 2 * ENTRY_SIZE);

		// Read Entry Table
		for (int i = 0; i < NUM_ENTRY; i++) {
			FileUtil::readBlock(fi, (i + 2) * ENTRY_SIZE, (char*)_entryTable[i], ENTRY_SIZE);
		}

		// Read Data Table
		int dataSize = ceil((_volumeInfo->size() * 1024.0 - (NUM_ENTRY + 2) * ENTRY_SIZE) / BLOCK_SIZE);
		for (int i = 0; i < dataSize; i++) {
			_dataTable.push_back(new Data());
		}
		for (int i = 0; i < _dataTable.size(); i++) {
			FileUtil::readBlock(fi, (NUM_ENTRY + 2) * ENTRY_SIZE + i * BLOCK_SIZE, (char*)_dataTable[i]);
		}

		fi.close();
	}

	void setVolumePassword(char* password)
	{
		if (_volumeInfo->password() == "") {
			_volumeInfo->setPassword((md5(password)));
			writeVolume();
			cout << "\nSet password successfully!" << endl;
		}
		else {
			cout << "\nThis volume already has password!" << endl;
		}
	}

	void encryptFile(int entryIndex, string password)
	{
		string fileData = getFileData(entryIndex);
		fileData = fileData.substr(0, _entryTable[entryIndex]->size());
		FileUtil::perplexity(fileData);

		unsigned char* plain = (unsigned char*)fileData.c_str();
		unsigned char* key = (unsigned char*)password.c_str();
		unsigned int plainLen = fileData.length();

		AES aes(AESKeyLength::AES_128);
		unsigned char* cipher = aes.EncryptECB(plain, plainLen, key);

		// Write file size
		_entryTable[entryIndex]->setSize(plainLen);

		// Write Data
		int start = _entryTable[entryIndex]->firstBlockOfData();
		int i = 0;
		bool isWrite = 1;

		while (true)
		{
			if (isWrite)
			{
				int len = DATA_SIZE;
				if (plainLen < (i + 1) * DATA_SIZE) {
					len = plainLen % DATA_SIZE;
					isWrite = 0;
				}

				_dataTable[start]->resetData();
				_dataTable[start]->setData((char*)cipher + i * DATA_SIZE, len);

				start = _dataTable[start]->nextBlock();
				i++;
			}
			else {
				break;
			}
		}
	}

	void decryptFile(int entryIndex, string password)
	{
		char* cipher = getFileData(entryIndex);
		unsigned char* key = (unsigned char*)password.c_str();
		unsigned int cipherLen = _entryTable[entryIndex]->size();

		AES aes(AESKeyLength::AES_128);
		unsigned char* plain = aes.DecryptECB((unsigned char*)cipher, cipherLen, key);

		string fileData((char*)plain);
		fileData = fileData.substr(0, cipherLen);

		FileUtil::clear(fileData);

		// Write file size
		int plainLen = fileData.length();
		_entryTable[entryIndex]->setSize(plainLen);

		// Write Data
		int start = _entryTable[entryIndex]->firstBlockOfData();
		int i = 0;
		bool isWrite = 1;

		while (true)
		{
			_dataTable[start]->resetData();

			if (isWrite) {
				int len = DATA_SIZE;
				if (plainLen < (i + 1) * DATA_SIZE) {
					len = plainLen % DATA_SIZE;
					isWrite = 0;
				}
				_dataTable[start]->setData((char*)fileData.c_str() + i * DATA_SIZE, len);
			}

			if (_dataTable[start]->nextBlock() == 0xFFFFFFFF) {
				break;
			}
			else {
				start = _dataTable[start]->nextBlock();
				i++;
			}
		}
	}

	void setFilePassword(char* path, char* password)
	{
		string target = FileUtil::getFileName(path).back();

		for (int i = 0; i < _entryTable.size(); i++) {
			if (_entryTable[i]->status() == 1 && _entryTable[i]->filename() == target)
			{
				if (_entryTable[i]->password() == "") {
					_entryTable[i]->setPassword((md5(md5(password))));
					encryptFile(i, md5(password));
					writeVolume();
					cout << "\nSet password successfully!" << endl;
				}
				else {
					cout << "\nThis file already has a password!" << endl;
				}
				return;
			}
		}

		cout << "\nFile not found!" << endl;
	}

	void changeVolumePassword(char* oldPass, char* newPass)
	{
		string hashPass = _volumeInfo->password();

		if (hashPass != "") {
			if (hashPass == md5(oldPass)) {
				_volumeInfo->setPassword((md5(newPass)));
				writeVolume();
				cout << "\nChange password successfully!" << endl;
			}
			else {
				cout << "\nWrong password!" << endl;
			}
		}
		else {
			cout << "\nThis volume did not have a password." << endl
				<< "To set password, use: setpassword <pass>" << endl;
		}
	}

	void changeFilePassword(char* path, char* oldPass, char* newPass)
	{
		string target = FileUtil::getFileName(path).back();

		for (int i = 0; i < _entryTable.size(); i++) {
			if (_entryTable[i]->status() == 1 && _entryTable[i]->filename() == target)
			{
				string hashPass = _entryTable[i]->password();

				if (hashPass != "") {
					if (hashPass == md5(md5(oldPass))) {
						decryptFile(i, md5(oldPass));

						encryptFile(i, md5(newPass));
						_entryTable[i]->setPassword((md5(md5(newPass))));

						writeVolume();
						cout << "\nChange password successfully!" << endl;
					}
					else {
						cout << "\nWrong password!" << endl;
					}
				}
				else {
					cout << "\nThis file did not have a password." << endl
						<< "To set password, use: setpassword <file-name> <pass>" << endl;
				}
				return;
			}
		}

		cout << "\nFile not found!" << endl;
	}

	void list()
	{
		cout << "\n" << FileUtil::getFileName((char*)_path.c_str()).back() << endl;
		for (int i = 0; i < _entryTable.size(); i++) {
			if (_entryTable[i]->status() == 1) {
				cout << "|--- " << _entryTable[i]->filename() << endl;
			}
		}
	}

	int validEntry()
	{
		// Find the entry with status 0
		for (int i = 0; i < _entryTable.size(); i++) {
			if (_entryTable[i]->status() == 0)
				return i;
		}

		// If not entry 0, find the first entry deleted
		for (int i = 0; i < _entryTable.size(); i++) {
			if (_entryTable[i]->status() == _volumeInfo->firstEntryDelete())
				return i;
		}

		// Else all entries are in use
		return -1;
	}

	vector<int> validBlocks(int numBlocks)
	{
		// Create a copy of dataStatus and first delete
		vector<uint32_t> dataStatus;
		for (int i = 0; i < _dataTable.size(); i++) {
			dataStatus.push_back(_dataTable[i]->status());
		}

		uint32_t firstBlockDelete = _volumeInfo->firstBlockDelete();

		// Work with copy
		vector<int> result;

		for (int i = 0; i < numBlocks; i++) {
			int start = validData(dataStatus, firstBlockDelete);
			if (start != -1) {
				if (dataStatus[start] != 0) {
					firstBlockDelete = FileUtil::nextBlockValue(firstBlockDelete);
				} // if this is a deleted block, first delete ++

				result.push_back(start);
				dataStatus[start] = 1;
			}
			else {
				return {};
			}
		}

		return result;
	}

	int validData(vector<uint32_t> dataStatus, uint32_t firstBlockDelete)
	{
		// Find the block with status 0
		for (int i = 0; i < dataStatus.size(); i++) {
			if (dataStatus[i] == 0)
				return i;
		}

		// If not block 0, find the first block deleted
		for (int i = 0; i < dataStatus.size(); i++) {
			if (dataStatus[i] == firstBlockDelete)
				return i;
		}

		// Else all block are in use
		return -1;
	}

	void importFile(char* path)
	{
		if (FileUtil::isExist(path)) {
			if (validEntry() != -1)
			{
				string data = FileUtil::getDataFromFile(path);
				string name = FileUtil::getFileName(path).back();

				// Check if data available
				int numBlocks = ceil((data.length() + PADDING) * 1.0 / DATA_SIZE);

				vector<int> dataOffset = validBlocks(numBlocks);
				if (dataOffset.empty()) {
					cout << "\nYou dont have enough space for data to import." << endl;
					return;
				}

				// Write Data
				bool isWrite = 1;

				for (int i = 0; i < dataOffset.size(); i++)
				{
					int start = dataOffset[i];

					if (_dataTable[start]->status() != 0) {
						_volumeInfo->setFirstBlockDelete(FileUtil::nextBlockValue(_volumeInfo->firstBlockDelete()));
					} // if this is a deleted block, first delete ++

					_dataTable[start]->reset(); // Reset before overwrite
					_dataTable[start]->setStatus(1); // Set status

					// Set next block
					if (i == dataOffset.size() - 1) {
						_dataTable[start]->setNextBlock(0xFFFFFFFF);
					}
					else {
						_dataTable[start]->setNextBlock(dataOffset[i + 1]);
					}

					// Set data
					if (isWrite)
					{
						int len = DATA_SIZE;
						if (data.length() < (i + 1) * DATA_SIZE) {
							len = data.length() % DATA_SIZE;
							isWrite = 0;
						}
						_dataTable[start]->setData((char*)data.c_str() + i * DATA_SIZE, len);
					}
				}

				// Write Entry
				int valid = validEntry();

				if (_entryTable[valid]->status() != 0) {
					_volumeInfo->setFirstEntryDelete(FileUtil::nextEntryValue(_volumeInfo->firstEntryDelete()));
				} // if this is a deleted entry, first delete ++

				_entryTable[valid]->reset();
				_entryTable[valid]->setUseMode(0x20, 0, dataOffset[0], data.length(), name);

				// Save
				cout << "\nNumber of blocks: " << dataOffset.size() << "\n" << "Write to block  : ";
				for (int i = 0; i < dataOffset.size(); i++) cout << dataOffset[i] << " ";
				cout << endl;

				writeVolume();
				cout << "\nImport file successfully!" << endl;
			}
			else {
				cout << "\nYou reached the limit of files/folders." << endl;
			}
		}
		else {
			cout << "\nFile not found!" << endl;
		}
	}

	char* getFileData(int entryIndex)
	{
		uint32_t firstBlockOfData = _entryTable[entryIndex]->firstBlockOfData();
		uint32_t size = _entryTable[entryIndex]->size();

		char* result = new char[size];

		for (int i = 0; i < ceil(size * 1.0 / DATA_SIZE); i++)
		{
			int len = DATA_SIZE;
			if (i == ceil(size * 1.0 / DATA_SIZE) - 1) len = size % DATA_SIZE;

			for (int j = 0; j < len; j++) {
				result[j + i * DATA_SIZE] = _dataTable[firstBlockOfData]->data()[j];
			}

			firstBlockOfData = _dataTable[firstBlockOfData]->nextBlock();
		}

		return result;
	}

	void outportFile(char* inPath, char* outPath, char* password)
	{
		if (FileUtil::isExist(outPath)) {
			cout << "\nFile name exists. Please choose another name." << endl;
			return;
		}

		string target = FileUtil::getFileName(inPath).back();

		for (int i = 0; i < _entryTable.size(); i++)
		{
			if (_entryTable[i]->status() == 1)
			{
				string name = _entryTable[i]->filename();

				if (name == target)
				{
					if (password == nullptr) {
						if (_entryTable[i]->password() != "") {
							cout << "\nThis file has a password." << endl;
							return;
						}
					}
					else {
						if (_entryTable[i]->password() == md5(md5(password))) {
							decryptFile(i, md5(password));
						}
						else {
							cout << "\nWrong password!" << endl;
							return;
						}
					}
					FileUtil::writeFile(outPath, getFileData(i), _entryTable[i]->size());
					cout << "\nOutport file successfully!" << endl;
					readVolume();
					return;
				}
			}
		}
		cout << "\nFile not found!" << endl;
	}

	void removeFile(char* path, bool permanently)
	{
		string target = FileUtil::getFileName(path).back();

		for (int i = 0; i < _entryTable.size(); i++)
		{
			if (_entryTable[i]->status() == 1)
			{
				string name = _entryTable[i]->filename();

				if (name == target) {
					if (permanently)
					{
						// Delete data
						int firstBlock = _entryTable[i]->firstBlockOfData();

						while (true) {
							int nextBlock = _dataTable[firstBlock]->nextBlock();
							_dataTable[firstBlock]->reset();

							if (nextBlock == 0xFFFFFFFF) break;
							else firstBlock = nextBlock;
						}

						// Delete entry
						_entryTable[i]->reset();
					}
					else
					{
						// Delete entry
						uint16_t nextE = _volumeInfo->nextEntryDelete();
						_entryTable[i]->setStatus(nextE);
						_volumeInfo->setNextEntryDelete(FileUtil::nextEntryValue(nextE));

						// Delete data
						int firstBlock = _entryTable[i]->firstBlockOfData();
						while (true) {
							uint16_t nextB = _volumeInfo->nextBlockDelete();
							_dataTable[firstBlock]->setStatus(nextB);
							_volumeInfo->setNextBlockDelete(FileUtil::nextBlockValue(nextB));

							if (_dataTable[firstBlock]->nextBlock() == 0xFFFFFFFF) {
								break;
							}
							else {
								firstBlock = _dataTable[firstBlock]->nextBlock();
							}
						}
					}
					writeVolume();
					cout << "\nRemove file successfully!" << endl;
					return;
				}
			}
		}
		cout << "\nFile not found!" << endl;
	}

	void printInfo()
	{
		cout << "\nVolume Info" << endl
			<< "   Signature          : " << _volumeInfo->signature() << endl
			<< "   Password           : " << _volumeInfo->password() << endl
			<< "   Size               : " << _volumeInfo->size() << endl
			<< "   First entry delete : " << _volumeInfo->firstEntryDelete() << endl
			<< "   Next entry delete  : " << _volumeInfo->nextEntryDelete() << endl
			<< "   First block delete : " << _volumeInfo->firstBlockDelete() << endl
			<< "   Next block delete  : " << _volumeInfo->nextBlockDelete() << endl;
	}
};