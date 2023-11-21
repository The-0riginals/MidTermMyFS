#include <conio.h>
#include "VolumeUtil.h"

#pragma warning(disable : 4996)

#define streq(a, b) (strcmp((a), (b)) == 0)

int main(int argc, char* argv)
{
    Volume* vol = nullptr;
    string currPath = "";
    bool isShowAddr = true;

    cout << "\nA simple MyFs design project by 19120659-20120382. Let's play around! \n   + Type 'help' to instruction." << endl;

    while (true)
    {
        char* password = NULL;
        char line[BUFSIZ], cmd[BUFSIZ], arg1[BUFSIZ], arg2[BUFSIZ], arg3[BUFSIZ];

        if (isShowAddr) cout << "\nMyFS@FS:~$ ";

        fflush(stderr);
        if (fgets(line, BUFSIZ, stdin) == NULL) break;
        int args = sscanf(line, "%s %s %s %s", cmd, arg1, arg2, arg3);

        if (args <= 0) {
            isShowAddr = false;
            continue;
        }
        else {
            isShowAddr = true;
        }

        if (streq(cmd, "create"))
        {
            vol = VolumeUtil::createVolume(args, arg1, arg2);
        }
        else if (streq(cmd, "open"))
        {
            vol = VolumeUtil::openVolume(args, arg1, arg2);
        }
        else if (streq(cmd, "setpassword"))
        {
            if (vol != nullptr) {
                VolumeUtil::setPassword(vol, args, arg1, arg2);
            }
            else cout << "\nYou do not work with any volume." << endl;
        }
        else if (streq(cmd, "changepassword"))
        {
            if (vol != nullptr) {
                VolumeUtil::changePassword(vol, args, arg1, arg2, arg3);
            }
            else cout << "\nYou do not work with any volume." << endl;
        }
        else if (streq(cmd, "ls"))
        {
            if (vol != nullptr) VolumeUtil::list(vol, args);
            else cout << "\nYou do not work with any volume." << endl;
        }
        else if (streq(cmd, "import"))
        {
            if (vol != nullptr) {
                VolumeUtil::importFile(vol, args, arg1, arg2);
            }
            else cout << "\nYou do not work with any volume." << endl;
        }
        else if (streq(cmd, "outport"))
        {
            if (vol != nullptr) VolumeUtil::outportFile(vol, args, arg1, arg2, arg3);
            else cout << "\nYou do not work with any volume." << endl;
        }
        else if (streq(cmd, "rm"))
        {
            if (vol != nullptr) {
                VolumeUtil::removeFile(vol, args, arg1, arg2);
            }
            else cout << "\nYou do not work with any volume." << endl;
        }
        else if (streq(cmd, "help"))
        {
            VolumeUtil::help();
        }
        else if (streq(cmd, "exit") || streq(cmd, "quit")) {
            break;
        }
        else {
            printf("\nUnknown command: %s", line);
            printf("Type 'help' for a list of commands.\n");
        }
    }

    system("pause");
    return 0;
}