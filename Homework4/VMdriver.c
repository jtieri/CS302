/* Author: Justin T.
 * Course: CS 302
 * Date: 4/15/19
 * Assignment: Homework Four
 * Implemented in Windows 10 using CLion + C99
 *
 * This program accepts commands that cause it to perform virtual memory
 * operations. The commands are read from standard input, but it is better
 * to put the commands in a "script file" and use the operating system's
 * command line to redirect the script file to this program's standard input
 * (as in "C:\VMdriver < VMcmds.txt").
 *
 * The commands that this program accepts are of the form
 *
 * time, vmOp, vmAddress, units, access
 *
 * The five parameters have the following meaning:
 *
 * time - Seconds to wait after reading the command before performing the VM operation.
 * vmOp - Code that represents the VM operation to perform.
 * vmAddress - virtual memory address (in hex) where the VM operation is to be performed
 * units - The number of units to use in the VM operation.
 *      For reserving memory, each unit represents 65536 bytes of memory.
 *      For committing memory, each unit represents 4096 bytes of memory.
 * access - Code that represents the access protection.
 *
 * The vmOp codes and their meanings are:
 *      1 - Reserve a region of virtual memory.
 *      2 - Commit a block of pages.
 *      3 - Touch pages in a block.
 *      4 - Lock a block of pages.
 *      5 - Unlock a block of pages.
 *      6 - Create a guard page.
 *      7 - Decommit a block of pages.
 *      8 - Release a region.
 *
 * The access codes and their meaning are:
 *      1 - PAGE_READONLY
 *      2 - PAGE_READWRITE
 *      3 - PAGE_EXECUTE
 *      4 - PAGE_EXECUTE_READ
 *      5 - PAGE_EXECUTE_READWRITE
 *      6 - PAGE_NOACCESS
 *
 * Most of the commands are described in the file:
 *      "Virtual Memory from 'Beginning Windows NT Programming' by Julian Templeman.pdf".
 *
 * The only command not mentioned there is the "Touch pages in a block" command. This means
 * that you should access (read) a memory location from each page in a specified block.
 *
 * Be absolutely sure that you check for any errors created by the VM operations
 * since you will be trying to cause errors.
 *
 * This program should create a process that runs the program VMmapper.exe so that
 * you can observe the memory operations as they happen. The program VMmapper takes
 * a PID on its command line and then it repeatedly maps and displays (once a second)
 * the virtual memory space of the process with that PID. This program should pass on
 * the command line its own PID to the VMmapper program.
 *
 * When this program has completed all of its operations, it goes into an infinite loop.
*/

#include <windows.h>
#include <stdio.h>
#include <string.h>


void printError(char* functionName); // prototype for the function, defined below, that prints err messages

int main(int argc, char *argv[]) {
    int time, vmOp, units, access;
    LPVOID vmAddress;
    DWORD flProtect, flProtectCopy, currentProcessId;
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;
    char commandLine[256];

    // You need to provide the code that starts up the
    // VMmapper.exe program with the PID of this program
    // on the command line. Use the Windows function
    // GetCurrentProcessId() to get this program's PID at
    // runtime.

    currentProcessId = GetCurrentProcessId();

    sprintf(commandLine, "%s %d", "VMmapper", (int) currentProcessId);

    ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

    if (! CreateProcessA(NULL, commandLine, 0, 0, 0, CREATE_NEW_CONSOLE, 0, 0, &startupInfo, &processInfo)) {
        printError("CreateProcess");
    }

    Sleep(5000);  // give VMmapper.exe time to start

    // Process loop
    printf("next VM command: ");
    while(scanf("%d%d%p%d%d", &time, &vmOp, &vmAddress, &units, &access) != EOF) {
        // wait until it is time to execute the command
        Sleep(time*1000);

        switch (access) {
            case 1:
                flProtect = 2;
                break;
            case 2:
                flProtect = 4;
                break;
            case 3:
                flProtect = 16;
                break;
            case 4:
                flProtect = 32;
                break;
            case 5:
                flProtect = 64;
                break;
            default:
                flProtect = access == 6;
                break;
        }

        // Parse the command and execute it
        switch (vmOp) {
            case 1:  // Reserve a region
                if (! VirtualAlloc(vmAddress, units << 16, MEM_RESERVE, flProtect) ) {
                    printError("VirtualAlloc");
                }

                break;
            case 2:  // Commit a block of pages
                if (! VirtualAlloc(vmAddress, units << 12, MEM_COMMIT, flProtect) ) {
                    printError("VirtualAlloc");
                }

                break;
            case 3: // Touch a page
                for (int i = 0; i < units; ++i ) {
                    printf("Touching 0x%p\n", (char *)vmAddress + 4096 * i);
                }

                break;
            case 4:  // Lock a block of pages
                if (! VirtualLock(vmAddress, units << 12) ) {
                    printError("VirtualLock");
                }

                break;
            case 5:  // Unlock a block of pages
                if (! VirtualUnlock(vmAddress, units << 12) ) {
                    printError("VirtualUnlock");
                }

                break;
            case 6:  // Create a guard page
                //flProtectCopy = flProtect >> 1;
                //flProtectCopy |= 1;

                if (! VirtualAlloc(vmAddress, units, MEM_COMMIT, flProtect) ) {
                    printError("VirtualAlloc");
                }

                break;
            case 7:  // Decommit a block of pages
                if (! VirtualFree(vmAddress, units << 12, MEM_DECOMMIT) ) {
                    printError("VirtualAlloc");
                }

                break;
            case 8:  // Release a region
                if (! VirtualFree(vmAddress, 0, MEM_RELEASE) ) {
                    printError("VirtualAlloc");
                }

                break;
            default:
                break;
        }

        printf("Processed %d %d 0x%p %d %d\n", time, vmOp, vmAddress, units, access);
        printf("next VM command: ");
    }

    while (1) {
        Sleep(1000); // spin until killed
    }

    EXIT_SUCCESS;
}



void printError(char* functionName) {
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

    // Display the string.
    fprintf(stderr, "\n%s failed on error %d: ", functionName, error_no);
    fprintf(stderr, lpMsgBuf);

    MessageBox(NULL, lpMsgBuf, "Error", MB_OK);

    // Free the buffer.
    LocalFree( lpMsgBuf );
}
