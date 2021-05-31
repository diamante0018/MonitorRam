#include "common.h"
#include <errno.h>
#include "monitor.h"
#include "utils.h"

#include <sys/types.h>
#include <signal.h>

#define MAX_MEMORY 1048576
#define PROGRAM_NAME 255
#define MAX_TRACK_SIZE 30

FILE *popen(const char *command, const char *mode);
int pclose(FILE *stream);
char *strerror(int errnum);

void checkStatus(FILE *file, int *currRealMem, int *peakRealMem,
                 int *currVirtMem, int *peakVirtMem)
{

    char result[BUFFER_SIZE];
    bzero(result, sizeof(result));

    if (file == NULL)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, " %1023s", result) == 1)
    {
        if (strcmp(result, "VmRSS:") == 0)
        {
            fscanf(file, " %d", currRealMem);
        }

        if (strcmp(result, "VmHWM:") == 0)
        {
            fscanf(file, " %d", peakRealMem);
        }

        if (strcmp(result, "VmSize:") == 0)
        {
            fscanf(file, " %d", currVirtMem);
        }

        if (strcmp(result, "VmPeak:") == 0)
        {
            fscanf(file, " %d", peakVirtMem);
        }
    }
}

void isProcessWild(int currRealMem, int peakRealMem,
                   int currVirtMem, int peakVirtMem, int ID)
{
    // Logs to a file in case something is up
    if (currRealMem > MAX_MEMORY)
    {
        FILE *fp;
        fp = fopen("Diavolo.txt", "a");
        printTimeStamp(fp);
        fprintf(fp, "WARNING: Exceeded %d KiB of Real Memory for process ID: %d\n", MAX_MEMORY, ID);
        fprintf(fp, "Process ID: %d\ncurrRealMem:%d KiB\npeakRealMem:%d KiB\ncurrVirtMem:%d KiB\npeakVirtMem:%d KiB\n", ID, currRealMem, peakRealMem, currVirtMem, peakVirtMem);
        fclose(fp);
        // Sends a polite request to terminate
        int result = kill(ID, SIGTERM);
        if (result)
        {
            printf("Error while terminating process: %d\n", ID);
            printf("Error message: %s\n", strerror(errno));
        }
    }
}

static processID[MAX_TRACK_SIZE] = {0};

void setup(const char *pid)
{
    char *command = va("pgrep %s", pid);
    FILE *file = popen(command, "r");

    if (file == NULL)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    char result[BUFFER_SIZE];
    bzero(result, sizeof(result));
    int i = 0;

    while (fgets(result, BUFFER_SIZE, file))
    {
        printf("Process ID: %s", result);
        processID[i] = atoi(result);
        i++;

        if (i > MAX_TRACK_SIZE - 1)
        {
            printf("Too many processes found\n");
            break;
        }
    }

    pclose(file);
}

void analyse()
{
    int i;
    FILE *status;
    int currRealMem = 0, peakRealMem = 0, currVirtMem = 0, peakVirtMem = 0;

    for (i = 0; i < MAX_TRACK_SIZE && processID[i] != 0; i++)
    {
        char *command = va("cat /proc/%d/status", processID[i]);
        status = popen(command, "r");
        if (file == NULL)
        {
            perror("popen");
            exit(EXIT_FAILURE);
        }

        checkStatus(status, &currRealMem, &peakRealMem, &currVirtMem, &peakVirtMem);
        isProcessWild(currRealMem, peakRealMem, currVirtMem, peakVirtMem, processID[i]);
        pclose(status);
    }
}
