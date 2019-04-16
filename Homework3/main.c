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

void swap(unsigned int * array1, unsigned int * array2);

int main(int argc, char *argv[]) {
   int processorCount = 0;           /* the number of allocated processors */
   ProcessorData *processorPool;     /* an array of ProcessorData structures */
   HANDLE *processHandles;           /* an array of handles to processes */
   unsigned int *jobDurationTimes;   /* an array of the job duration times to be ran */
   unsigned int jobsRan = 0;         /* the number of jobs ran so far by the scheduler */
   STARTUPINFO startupinfo;
   char buffer[256];                 /* buffer used to load each process with the job time */

   if (argc < 3) {
      fprintf(stderr, "usage, %s  SCHEDULE_TYPE  SECONDS...\n", argv[0]);
      fprintf(stderr, "Where: SCHEDULE_TYPE = 0 means \"first come first serve\"\n");
      fprintf(stderr, "       SCHEDULE_TYPE = 1 means \"shortest job first\"\n");
      fprintf(stderr, "       SCHEDULE_TYPE = 2 means \"longest job first\"\n");

      return 0;
   }

   printf("The scheduler type selected was %d \n", atoi(argv[1]));

   /* read the job duration times off the command-line */

   const int totalJobTimes = argc - 2; // the total number of job times to be ran

   printf("The total number of jobs to be ran: %d \n", totalJobTimes);

   // Allocate memory for the job duration times being passed in from the CLI
   jobDurationTimes = (unsigned int *) malloc(sizeof(unsigned int) * totalJobTimes);

   printf("The job times to be ran are: ");

   for (int i = 0; i < totalJobTimes; i++) {
       jobDurationTimes[i] = (unsigned int) atoi(argv[i + 2]);
       printf("%d  ", jobDurationTimes[i]);
   }

   printf(" \n");

   /* sort the array of times based on the scheduler type selected at runtime */
   unsigned int tmpVal = 0;

   if (atoi(argv[1]) == 1) {
       // Sort shortest job first
       for (int i = 0; i < totalJobTimes - 1; i++) {
           for (int j = i + 1; j < totalJobTimes; j++) {
               if (jobDurationTimes[i] >  jobDurationTimes[j]) {
                   swap(&jobDurationTimes[i],  &jobDurationTimes[j]);
               }
           }
       }

   } else if (atoi(argv[1]) == 2) {
       // Sort longest job first
       for (int i = 0; i < totalJobTimes - 1; i++) {
           for (int j = i + 1; j < totalJobTimes; j++) {
               if (jobDurationTimes[i] <  jobDurationTimes[j]) {
                   swap(&jobDurationTimes[i],  &jobDurationTimes[j]);
               }
           }
       }
   }

   printf("The sorted job times to be ran are: ");

   for (int i = 0; i < totalJobTimes; i++) {
       printf("%d  ", jobDurationTimes[i]);
   }

   printf(" \n");


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

   printf("The total number of processors to have jobs delegated to is: %d \n", processorCount);

   /* create, and then initialize, the processor pool data structure */

   processorPool = (ProcessorData *) malloc(processorCount * sizeof(ProcessorData));

   for (int i = 0; i < processorCount; i++) {
      if ((processAffinityMask & 1) && (i == 0)) {
          processorPool[i].affinityMask = 1;
          processorPool[i].running = 0;
          printf("ProcessorData created on processor %d with affinity mask %d \n", i + 1, processorPool[i].affinityMask);
      } else if ((processAffinityMask & 2) && (i == 1)) {
          processorPool[i].affinityMask = 2;
          processorPool[i].running = 0;
          printf("ProcessorData created on processor %d with affinity mask %d \n", i + 1, processorPool[i].affinityMask);
      } else if ((processAffinityMask & 4) && (i == 2)) {
          processorPool[i].affinityMask = 4;
          processorPool[i].running = 0;
          printf("ProcessorData created on processor %d with affinity mask %d \n", i + 1, processorPool[i].affinityMask);
      } else if ((processAffinityMask & 8) && (i == 3)) {
          processorPool[i].affinityMask = 8;
          processorPool[i].running = 0;
          printf("ProcessorData created on processor %d with affinity mask %d \n", i + 1, processorPool[i].affinityMask);
      } else if ((processAffinityMask & 0x10) && (i == 4)) {
          processorPool[i].affinityMask = 16;
          processorPool[i].running = 0;
          printf("ProcessorData created on processor %d with affinity mask %d \n", i + 1, processorPool[i].affinityMask);
      } else if ((processAffinityMask & 0x20) && (i == 5)) {
          processorPool[i].affinityMask = 32;
          processorPool[i].running = 0;
          printf("ProcessorData created on processor %d with affinity mask %d \n", i + 1, processorPool[i].affinityMask);
      } else if ((processAffinityMask & 0x40) && (i == 6)) {
          processorPool[i].affinityMask = 64;
          processorPool[i].running = 0;
          printf("ProcessorData created on processor %d with affinity mask %d \n", i + 1, processorPool[i].affinityMask);
      } else if ((processAffinityMask & 0x80) && (i == 7)) {
          processorPool[i].affinityMask = 128;
          processorPool[i].running = 0;
          printf("ProcessorData created on processor %d with affinity mask %d \n", i + 1, processorPool[i].affinityMask);
      }
   }

   /* start the first group of processes */

   unsigned int workingJobDurationTime = 0;

   if (processorCount) {
      do {
         workingJobDurationTime = jobDurationTimes[jobsRan];
         printf("The current job time to be ran is: %d \n", workingJobDurationTime); // Test

         ZeroMemory(&startupinfo, sizeof(startupinfo));
         startupinfo.cb = sizeof(startupinfo);

         char *targetProgramName = "computeProgram_64_debug.exe";
         sprintf(buffer, "%s %d", targetProgramName, workingJobDurationTime);

         if (! CreateProcessA(NULL, buffer, 0, 0, 1, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, 0, 0, &startupinfo, &processorPool[jobsRan].processInfo)) {
             printError("CreateProcessA ");
             EXIT_FAILURE;
         }

         SetProcessAffinityMask(processorPool[jobsRan].processInfo.hProcess, (DWORD) processorPool[jobsRan].affinityMask);
         ResumeThread(processorPool[jobsRan].processInfo.hThread);

         processorPool[jobsRan].running = 1; // set process to running

         printf("Process %d was created on processor %d with .running = %d \n", jobsRan + 1, processorPool[jobsRan].affinityMask, processorPool[jobsRan].running);

         jobsRan++;
      } while ((jobsRan < processorCount) && (jobsRan < totalJobTimes));
   }

    /* Repeatedly wait for a process to finish and then,
       if there are more jobs to run, run a new job on
       the processor that just became free. */
    if (processorCount) {
        while (1) {
            DWORD result;
            int handleCount = 0;

            /* get, from the processor pool, handles to the currently running processes */
            /* put those handles in an array */
            /* use a parallel array to keep track of where in the processor pool each handle came from */

            for (int i = 0; i < processorCount; i++) {
                if (processorPool[i].running) {
                    ++handleCount;
                }
            }

            printf("The handle count is %d \n", handleCount);

            processHandles = (HANDLE *) malloc(handleCount * sizeof(HANDLE));

            unsigned int counter = 0;
            for (int i = 0; i < processorCount; i++) {
                if (processorPool[i].running) {
                    processHandles[counter] = processorPool[i].processInfo.hProcess;
                    printf("Handle inserted into processHandles at index: %d \n", counter);
                    counter++;
                }
            }

            /* check that there are still processes running, if not, quit */
            unsigned int quitLoop = 1;

            for (int i = 0; i < processorCount; i++) {
                if (processorPool[i].running) {
                    quitLoop = 0;
                    break;
                }
            }

            if (quitLoop) {
                break;
            }

            /* wait for one of the running processes to end */
            if (handleCount > 0) {
                if (WAIT_FAILED == (result = WaitForMultipleObjects((DWORD) handleCount, processHandles, FALSE, INFINITE))) {
                    printError("WaitForMultipleObjects");
                }
            }

            /* close the handles of the finished process and update the processorPool array */
            CloseHandle(processHandles[result]);

            int processorPoolIndex = 0; // the index into processorPool converted from index in processorHandles

            for (int i = 0; i < processorCount; i++) {
                if (processHandles[result] == processorPool[i].processInfo.hProcess) {
                    processorPool[i].running = 0;
                    processorPoolIndex = i;
                }
            }

            /* check if there is another process to run on the processor that just became free */
            if (jobsRan < totalJobTimes) {
                workingJobDurationTime = jobDurationTimes[jobsRan - 1];

                printf("The current job time to be ran is: %d \n", workingJobDurationTime);

                ZeroMemory(&startupinfo, sizeof(startupinfo));
                startupinfo.cb = sizeof(startupinfo);

                char *targetProgramName = "computeProgram_64_debug.exe";
                sprintf(buffer, "%s %d", targetProgramName, workingJobDurationTime);

                if (! CreateProcessA(NULL, buffer, 0, 0, 1, CREATE_SUSPENDED | CREATE_NEW_CONSOLE, 0, 0, &startupinfo, &processorPool[processorPoolIndex].processInfo)) {
                    printError("CreateProcessA ");
                    EXIT_FAILURE;
                }

                SetProcessAffinityMask(processorPool[processorPoolIndex].processInfo.hProcess, (DWORD) processorPool[processorPoolIndex].affinityMask);
                ResumeThread(processorPool[processorPoolIndex].processInfo.hThread);

                processorPool[processorPoolIndex].running = 1; // set process to running

                jobsRan++;
            }
        }
    }

   EXIT_SUCCESS;
}

void swap(unsigned int * array1, unsigned int * array2) {
    unsigned int tmpVal = *array2;
    *array2 = *array1;
    *array1 = tmpVal;
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
