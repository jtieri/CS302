/* Author: Justin T.
 * Course: CS 302
 * Date: 3/18/19
 * Assignment: Homework Three
 * Implemented in Windows 10 using CLion + C99
*/
#include <windows.h>
#include <stdio.h>


typedef struct processor_data {
   int affinityMask;                /* affinity mask of this processor (just one bit set) */
   PROCESS_INFORMATION processInfo; /* process currently running on this processor */
   int running;                     /* 1 when this processor is running a task, 0 otherwise */
} ProcessorData;


/* function prototypes */
void printError(char* functionName);



int main(int argc, char *argv[]) {
   int processorCount = 0;           /* the number of allocated processors */
   ProcessorData *processorPool;     /* an array of ProcessorData structures */
   HANDLE *processHandles;           /* an array of handles to processes */
   unsigned int *jobDurationTimes;   /* an array of the job duration times to be ran */

   if (argc < 3) {
      fprintf(stderr, "usage, %s  SCHEDULE_TYPE  SECONDS...\n", argv[0]);
      fprintf(stderr, "Where: SCHEDULE_TYPE = 0 means \"first come first serve\"\n");
      fprintf(stderr, "       SCHEDULE_TYPE = 1 means \"shortest job first\"\n");
      fprintf(stderr, "       SCHEDULE_TYPE = 2 means \"longest job first\"\n");

      return 0;
   }

   /* read the job duration times off the command-line */

   const int totalProcTimes = argc - 2; /* the total number of job times to be ran  */

   // Allocate memory for the job duration times being passed in from the CLI
   jobDurationTimes = (unsigned int *) malloc(sizeof(unsigned int) * totalProcTimes);

   for (int i = 0; i < totalProcTimes; i++) {
      jobDurationTimes[i] = (unsigned int) atoi(argv[i + 2]);
   }

   /* get the processor affinity mask for this process */

   HANDLE currentProcessHandle = GetCurrentProcess(); // Get a handle to the program

   DWORD_PTR processAffinityMask;
   DWORD_PTR systemAffinityMask; // This data is not needed throughout the program but GetProcessAffinityMask() req's a memory location for it

   GetProcessAffinityMask(currentProcessHandle, &processAffinityMask, &systemAffinityMask);

   /* count the number of processors set in the affinity mask */

   if (processAffinityMask & 1) {
      processorCount++;
   }

   if (processAffinityMask & 2) {
      processorCount++;
   }

   if (processAffinityMask & 4) {
      processorCount++;
   }

   if (processAffinityMask & 8) {
      processorCount++;
   }

   if (processAffinityMask & 0x10) {
      processorCount++;
   }

   if (processAffinityMask & 0x20) {
      processorCount++;
   }

   if (processAffinityMask & 0x40) {
      processorCount++;
   }

   if (processAffinityMask & 0x80) {
      processorCount++;
   }

   /* create, and then initialize, the processor pool data structure */

   processorPool = (ProcessorData *) malloc(processorCount * sizeof(ProcessorData));

   for (int i = 0; i < processorCount; i++) {
      if ((processAffinityMask & 1) && (i <= 1)) {
          processorPool[i].affinityMask = 1;
          processorPool[i].running = 0;
      } else if ((processAffinityMask & 2) && (i <= 2)) {
          processorPool[i].affinityMask = 2;
          processorPool[i].running = 0;
      } else if ((processAffinityMask & 4) && (i <= 4)) {
          processorPool[i].affinityMask = 4;
          processorPool[i].running = 0;
      } else if ((processAffinityMask & 8) && (i <= 8)) {
          processorPool[i].affinityMask = 8;
          processorPool[i].running = 0;
      } else if ((processAffinityMask & 0x10) && (i <= 0x10)) {
          processorPool[i].affinityMask = 16;
          processorPool[i].running = 0;
      } else if ((processAffinityMask & 0x20) && (i <= 0x20)) {
          processorPool[i].affinityMask = 32;
          processorPool[i].running = 0;
      } else if ((processAffinityMask & 0x40) && (i <= 0x40)) {
          processorPool[i].affinityMask = 64;
          processorPool[i].running = 0;
      } else if ((processAffinityMask & 0x80) && (i <= 0x80)) {
          processorPool[i].affinityMask = 128;
          processorPool[i].running = 0;
      }
   }

   /* start the first group of processes */

   if (processorCount) {
      unsigned int loopCounter = 0;

      STARTUPINFO startupinfo;

      char buffer[256];
      do {
         unsigned int workingJobDurationTime = *jobDurationTimes;

         ZeroMemory(&startupinfo, sizeof(startupinfo));
         startupinfo.cb = sizeof(startupinfo);

         char *targetProgramName = "computeProgram_64_debug.exe";
         sprintf(buffer, "%s %d", targetProgramName, workingJobDurationTime);

         if (! CreateProcessA(NULL, buffer, 0, 0, 1, CREATE_SUSPENDED, 0, 0, &startupinfo, &(*processorPool).processInfo)) {
             printError("CreateProcessA ");
             EXIT_FAILURE;
         }

         SetProcessAffinityMask((*processorPool).processInfo.hProcess, (DWORD) (*processorPool).affinityMask);
         ResumeThread((*processorPool).processInfo.hThread);

         (*processorPool).running = 1; // set process to running

         processorPool++;
         jobDurationTimes++;
         loopCounter++;
      } while (loopCounter != processorCount);
   }

   processHandles = (HANDLE *) malloc(processorCount * sizeof(HANDLE));

    /* Repeatedly wait for a process to finish and then,
       if there are more jobs to run, run a new job on
       the processor that just became free. */
    while (1) {
        DWORD result;
        DWORD handleCount;

        /* get, from the processor pool, handles to the currently running processes */
        /* put those handles in an array */
        /* use a parallel array to keep track of where in the processor pool each handle came from */
        for (int i = 0; i < processorCount; i++) {
            if (processorPool[i].running) {
                processHandles[i] = processorPool[i].processInfo.hProcess;
                handleCount++;
            }
        }

        /* check that there are still processes running, if not, quit */
        unsigned int quitLoop = 1;

        for (int i = 0; i < handleCount; i++) {
            if (processorPool[i].running) {
                quitLoop = 0;
                break;
            }
        }

        if (quitLoop) {
            break;
        }

        /* wait for one of the running processes to end */
        if (WAIT_FAILED == (result = WaitForMultipleObjects(handleCount, processHandles, FALSE, INFINITE))) {
            printError("WaitForMultipleObjects");
        }


        /* translate result from an index in processHandles[] to an index in processorPool[] */

        /* close the handles of the finished process and update the processorPool array */
        CloseHandle(processHandles[result]);

        processorPool[result].running = 0;

        /* check if there is another process to run on the processor that just became free */

    }

   EXIT_SUCCESS;
}









/****************************************************************
   The following function can be used to print out "meaningful"
   error messages. If you call a Windows function and it returns
   with an error condition, then call this function right away and
   pass it a string containing the name of the Windows function that
   failed. This function will print out a reasonable text message
   explaining the error.
*/
void printError(char* functionName) {
   LPVOID lpMsgBuf;
   int error_no;
   error_no = GetLastError();
   FormatMessage(
         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
         NULL,
         error_no,
         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* default language */
         (LPTSTR) &lpMsgBuf,
         0,
         NULL
   );

   /* Display the string. */
   fprintf(stderr, "\n%s failed on error %d: ", functionName, error_no);
   fprintf(stderr, (const char*)lpMsgBuf);
   /* Free the buffer. */
   LocalFree( lpMsgBuf );
}
