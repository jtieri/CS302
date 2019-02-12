/* Author: Justin T.
 * Course: CS 302
 * Date: 2/5/19
 * Assignment: Homework Two
 * Implemented in Windows 10 using CLion + C99
 *
 * launcher.c serves as a launcher interface to some common Windows programs. The user has the ability to repeatedly
 * create processes for any of the following Windows programs.
 *
 * 1. notepad
 * 2. wordpad
 * 3. command line shell
 * 4. calculator
 * 5. explorer
 *
 * In the case of a user running the command line shell, the launcher program will enter a blocking process state where
 * it will wait for the command line shell to terminate. Once the command line shell has terminated, the launcher
 * program will return the exit code returned from the command line shell process.
 * */

#include <windows.h>
#include <stdio.h>

void printError(char *functionName);

int main(void) {
    STARTUPINFO startupinfo;
    ZeroMemory(&startupinfo, sizeof(startupinfo));
    startupinfo.cb = sizeof(startupinfo);

    char buffer[5][256];

    char *environment_var = getenv("SystemRoot");
    sprintf(buffer[0], "%s\\system32\\NOTEPAD.EXE", environment_var);

    environment_var = getenv("ProgramFiles");
    sprintf(buffer[1], "%s\\Windows NT\\Accessories\\WORDPAD.EXE", environment_var);

    environment_var = getenv("COMSPEC");
    sprintf(buffer[2], "%s /T:F9", environment_var);

    environment_var = getenv("SystemRoot");
    sprintf(buffer[3], "%s\\system32\\calc.exe", environment_var);

    environment_var = getenv("SystemRoot");
    sprintf(buffer[4], "%s\\explorer.exe", environment_var);

    PROCESS_INFORMATION process_information; // To be used in the loop while creating processes
    DWORD exit_code = 0; // To be used in returning the exit value from the command shell

    int user_input = -1;
    do {
        printf("Please make a selection from the following list. \n");
        printf("\t 0: Quit \n");
        printf("\t 1: Run Notepad \n");
        printf("\t 2: Run Wordpad \n");
        printf("\t*3: Run cmd shell \n");
        printf("\t 4: Run Calculator \n");
        printf("\t 5: Run Explorer \n");
        printf("Enter your choice now: \n");

        scanf("%d", &user_input);

        if ((user_input > 0) && (user_input !=3) && (user_input < 5)) {

            if (CreateProcessA(NULL, buffer[user_input - 1], NULL, NULL, FALSE,
                    NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, NULL, NULL, &startupinfo, &process_information)) {
                printf("Started program %d with pid = %d\n\n", user_input, (int) process_information.dwProcessId);

                CloseHandle(process_information.hThread);
                CloseHandle(process_information.hProcess);
            } else {
                printError("CreateProcessA");
            }

        } else if (user_input == 3) {
            startupinfo.dwFlags = 4;
            startupinfo.dwX = 0;
            startupinfo.dwY = 0;
            startupinfo.lpTitle = "What is your command?";
            _putenv("prompt=Speak to me$G");

            if (CreateProcessA(NULL, buffer[user_input - 1], NULL, NULL, FALSE,
                    NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, NULL, NULL, &startupinfo, &process_information)) {
                printf("Started program %d with pid = %d\n", user_input, (int) process_information.dwProcessId);
                printf("  waiting for program %d to terminate...\n", user_input);

                WaitForSingleObject(process_information.hProcess, INFINITE);

                GetExitCodeProcess(process_information.hProcess, &exit_code);
                printf("  program %d exited with return value %d\n\n", user_input, (int) exit_code);

                CloseHandle(process_information.hThread);
                CloseHandle(process_information.hProcess);
            } else {
                printError("CreateProcess");
            }
        }

        // Zero out the STARTUPINFO for the next iteration of the loop.
        ZeroMemory(&startupinfo, sizeof(startupinfo));
        startupinfo.cb = sizeof(startupinfo);

        _putenv("prompt="); // Reset to the default command shell prompt
    } while (user_input != 0);

    exit(EXIT_SUCCESS);
}


/*
 * The following function can be used to print out "meaningful"
 * error messages. If you call a Win32 function and it returns
 * with an error condition, then call this function right away and
 * pass it a string containing the name of the Win32 function that
 * failed. This function will print out a reasonable text message
 * explaining the error and then (if chosen) terminate the program.
 */
void printError(char *functionName) {
    LPVOID lpMsgBuf;
    int error_no;

    error_no = GetLastError();
    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error_no,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
    );

    fprintf(stderr, "\n%s failed on error %d: ", functionName, error_no); // Display the string.
    fprintf(stderr, (char *)lpMsgBuf);

    LocalFree(lpMsgBuf); // Free the buffer.
    //ExitProcess(1);   // terminate the program
}