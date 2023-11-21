#pragma once
#include "Volume.h"

class VolumeUtil
{
public:
    static Volume* createVolume(int args, char* arg1, char* arg2)
    {
        if (args == 3) {
            if (FileUtil::isExist(arg1)) {
                cout << "\nVolume has already existed!" << endl;
                return nullptr;
            }

            Volume* vol = new Volume(stoi(arg2), string(arg1));
            vol->writeVolume();
            cout << "\nCreate new volume successfully!" << endl;

            return vol;
        }
        else {
            cout << "\nUsage: create <vol-name> <size>\n";
            return nullptr;
        }
    }

    static Volume* openVolume(int args, char* arg1, char* arg2)
    {
        if (args > 1) {
            if (!FileUtil::isExist(arg1)) {
                cout << "\nVolume has not existed!" << endl;
                return nullptr;
            }

            Volume* vol = new Volume(string(arg1));
            vol->readVolume();
            string password = vol->volumeInfo()->password();

            if (args == 2) {
                if (password == "") {
                    cout << "\nOpen volume successfully!" << endl;
                    return vol;
                }
                else {
                    cout << "\nThis volume has a password. Usage: open <vol-name> <pass>" << endl;
                    return nullptr;
                }
            }
            else if (args == 3) {
                if (password == md5(arg2)) {
                    cout << "\nOpen volume successfully!" << endl;
                    return vol;
                }
                else {
                    cout << "\nWrong password!" << endl;
                    return nullptr;
                }
            }
        }
        else {
            cout << "\nUsage: open <vol-name> <pass>\n";
            cout << "OR: open <vol-name> (if it don't have password)\n";
            return nullptr;
        }
    }

    static void setPassword(Volume* vol, int args, char* arg1, char* arg2)
    {
        if (args == 2) {
            vol->setVolumePassword(arg1);
        }
        else if (args == 3) {
            vol->setFilePassword(arg1, arg2);
        }
        else {
            cout << "\nUsage: setpassword <file-name> <pass>\n";
            cout << "OR: setpassword <pass> (set password for volume)\n";
        }
    }

    static void changePassword(Volume* vol, int args, char* arg1, char* arg2, char* arg3)
    {
        if (args == 3) {
            vol->changeVolumePassword(arg1, arg2);
        }
        else if (args == 4) {
            vol->changeFilePassword(arg1, arg2, arg3);
        }
        else {
            cout << "\nUsage: changepassword <file-name> <old-pass> <new-pass>\n";
            cout << "OR: changepassword <old-pass> <new-pass> (change password for volume)\n";
        }
    }

    static void list(Volume* vol, int args)
    {
        if (args == 1) {
            vol->list();
        }
        else {
            cout << "\nUsage: ls\n";
        }
    }

    static void importFile(Volume* vol, int args, char* arg1, char* arg2)
    {
        if (args == 2)
        {
            vol->importFile(arg1);
        }
        else
        {
            cout << "\nUsage:import <file-path>\n";
            //cout << "OR:import <file-path> <inside-path>\n";
        }
    }

    static void outportFile(Volume* vol, int args, char* arg1, char* arg2, char* arg3)
    {
        if (args == 3)
        {
            vol->outportFile(arg1, arg2, nullptr);
        }
        else if (args == 4)
        {
            vol->outportFile(arg1, arg2, arg3);
        }
        else
        {
            cout << "\nUsage: outport <file-path> <des-path> <password>\n";
        }
    }

    static void removeFile(Volume* vol, int args, char* arg1, char* arg2)
    {
        if (args > 1)
        {
            vol->removeFile(arg1, args == 3 && (string(arg2) == "true" || string(arg2) == "1"));
        }
        else
        {
            cout << "\nUsage:rm <file-name> <permanently-delete: true/false>\n";
        }
    }

    static void help()
    {
        cout << "\nCommands are avaiable for the following:\n";
        cout << "  create <volume-name> <size>\n";
        cout << "    + Purpose: Create a new volume.\n";
        cout << "    + <volume-name>: Volume's name (maximum: 256 ascii character).\n";
        cout << "    + <size>: Size of volume  (KB, maximum: 4096, minimum: 100).\n";
        cout << "    + Ex: create MyFS.dat 512\n";

        cout << "  open <vol-name> <pass>\n";
        cout << "    + Purpose: Open a volume already exists.\n";
        cout << "    + <volume-name>: Volume's name (maximum: 256 ascii character).\n";

        cout << "  ls\n";
        cout << "    + Purpose: List files in the volume.\n";

        cout << "  import <src-path>\n";
        cout << "    + Purpose: Import a file outside into volume.\n";
        cout << "    + <src-path>: Source directory.\n";
        cout << "    + Ex: import main.cpp\n";

        cout << "  outport <file-path> <des-path>\n";
        cout << "    + Purpose: Export a file inside volume to a directory.\n";
        cout << "    + <file-name>: Name of the file inside the volume.\n";
        cout << "    + <des-path>: Destination directory.\n";
        cout << "    + Ex: outport main.cpp main.cpp\n";

        cout << "  rm <file-name> <permanently-delete: true/false>\n";
        cout << "    + Purpose: Remove a file inside volume to a directory. True for permanent remove\n";
        cout << "    + <file-name>: Name of the file inside the volume.\n";
        cout << "    + Ex: rm main.cpp\n";

        cout << "  setpassword <password>\n";
        cout << "    + Purpose: Set password for the volume.\n";
        cout << "    + <password>: Password.\n";
        cout << "    + Ex: setpassword pass@123\n";

        cout << "  setpassword <file-name> <password>\n";
        cout << "    + Purpose: Set password for the file inside the volume.\n";
        cout << "    + <file-name>: Name of the file inside the volume.\n";
        cout << "    + <password>: Password.\n";
        cout << "    + Ex: setpassword main.cpp pass@123\n";

        cout << "  changepassword <password>\n";
        cout << "    + Purpose: Change password for the volume.\n";
        cout << "    + <password>: Password.\n";
        cout << "    + Ex: changepassword pass@123\n";

        cout << "  changepassword <file-name> <password>\n";
        cout << "    + Purpose: Change password for the file inside the volume.\n";
        cout << "    + <file-name>: Name of the file inside the volume.\n";
        cout << "    + <password>: Password.\n";
        cout << "    + Ex: changepassword main.cpp pass@123\n";

        cout << "  help\n";
        cout << "    + Purpose: View all commands\n";

        cout << "  exit\n";
        cout << "    + Purpose: Exit the program\n";

        cout << "  quit\n";
        cout << "    + Purpose: Exit the program\n";
    }
};