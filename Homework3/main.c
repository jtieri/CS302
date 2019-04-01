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
   int processorCount = 0;       /* the number of allocated processors */
   ProcessorData *processorPool; /* an array of ProcessorData structures */
   HANDLE *processHandles;       /* an array of handles to processes */
   unsigned int *jobDurationTimes;   /* an array of the process times to be ran */

   if (argc < 3) {
      fprintf(stderr, "usage, %s  SCHEDULE_TYPE  SECONDS...\n", argv[0]);
      fprintf(stderr, "Where: SCHEDULE_TYPE = 0 means \"first come first serve\"\n");
      fprintf(stderr, "       SCHEDULE_TYPE = 1 means \"shortest job first\"\n");
      fprintf(stderr, "       SCHEDULE_TYPE = 2 means \"longest job first\"\n");

      return 0;
   }

   /* read the job duration times off the command-line */

   jobDurationTimes = (unsigned int *) malloc(4 * (argc - 2)); // Allocate memory for the job duration times being passed in from the CLI

   const unsigned int totalProcTimes = argc - 2;

   for (int i = 0; i < totalProcTimes; i++) {
      jobDurationTimes[i] = argv[i + 2];
   }

   /* get the processor affinity mask for this process */

   HANDLE currentProcessHandle = GetCurrentProcess(); // Get a handle to the program

   DWORD_PTR processAffinityMask;
   DWORD_PTR systemAffinityMask; // This data is not needed throughout the program but GetProcessAffinityMask() req's a memory location for it

   GetProcessAffinityMask(currentProcessHandle, &processAffinityMask, &systemAffinityMask);

   /* count the number of processors set in the affinity mask */

   if (processAffinityMask & 1) {
      processorCount++;
   } else if (processAffinityMask & 2) {
      processorCount++;
   } else if (processAffinityMask & 4) {
      processorCount++;
   } else if (processAffinityMask & 8) {
      processorCount++;
   } else if (processAffinityMask & 0x10) {
      processorCount++;
   } else if (processAffinityMask & 0x20) {
      processorCount++;
   } else if (processAffinityMask & 0x40) {
      processorCount++;
   } else if (processAffinityMask & 0x80) {
      processorCount++;
   }

   /* create, and then initialize, the processor pool data structure */

   processorPool = malloc(processorCount * sizeof(ProcessorData));

   for (int i = 0; i < processorCount; i++) {
      if ((processAffinityMask & 1) && (i <= 1)) {
         processorPool[i] = {1, 0, 0};
      } else if ((processAffinityMask & 2) && (i <= 2)) {
         processorPool[i] = {2, 0, 0};
      } else if ((processAffinityMask & 4) && (i <= 4)) {
         processorPool[i] = {4, 0, 0};
      } else if ((processAffinityMask & 8) && (i <= 8)) {
         processorPool[i] = {8, 0, 0};
      } else if ((processAffinityMask & 0x10) && (i <= 0x10)) {
         processorPool[i] = {16, 0, 0};
      } else if ((processAffinityMask & 0x20) && (i <= 0x20)) {
         processorPool[i] = {32, 0, 0};
      } else if ((processAffinityMask & 0x40) && (i <= 0x40)) {
         processorPool[i] = {64, 0, 0};
      } else if ((processAffinityMask & 0x80) && (i <= 0x80)) {
         processorPool[i] = {128, 0, 0};
      }
   }

   /* start the first group of processes */

   if (processorCount) {
      unsigned int loopCounter = 0;

      STARTUPINFO startupinfo;
   
      do {
         unsigned int workingJobDurationTime = *jobDurationTimes;

         ZeroMemory(&startupinfo, sizeof(startupinfo));
         startupinfo.cb = sizeof(startupinfo);

         char *targetProgramName = "computeProgram_64.exe";
         sprintf(targetProgramName, "%s %d", targetProgramName, workingJobDurationTime;

         if (! CreateProcessA(NULL, targetProgramName, 0, 0, 1, CREATE_SUSPENDED, 0, 0, &startupinfo, *processorPool.processInfo)) {
            //TODO handle error of process not creating properly
         }

         //SetProcessAffinityMask()
         //ResumeThread()

         processorPool++;
         jobDurationTimes++;
         loopCounter++;
      } while (loopCounter != processorCount);
   }
   

   /* Repeatedly wait for a process to finish and then,
      if there are more jobs to run, run a new job on
      the processor that just became free. */
   while (1) {
      DWORD result;

      /* get, from the processor pool, handles to the currently running processes */
      /* put those handles in an array */
      /* use a parallel array to keep track of where in the processor pool each handle came from */

      /* check that there are still processes running, if not, quit */

      /* wait for one of the running processes to end */
      if (WAIT_FAILED == (result = WaitForMultipleObjects(handleCount, processHandles, FALSE, INFINITE)))
         printError("WaitForMultipleObjects");

      /* translate result from an index in processHandles[] to an index in processorPool[] */

      /* close the handles of the finished process and update the processorPool array */

      /* check if there is another process to run on the processor that just became free */


   }

   return 0;
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
