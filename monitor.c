#include "common.h"
#include <errno.h>
#include "monitor.h"
#include "utils.h"

#include <sys/types.h>
#include <signal.h>

#define MAX_MEMORY 1048576
#define MAX_TRACK_SIZE 30

FILE *popen(const char *command, const char *mode);
int pclose(FILE *stream);
char *strerror(int errnum);

void printGenericStatusError()
{
    printf("Error while parsing file\n");
    printf("Processes may not be correctly monitored\n");
}

void checkStatus(FILE *file, int *currRealMem, int *peakRealMem,
                 int *currVirtMem, int *peakVirtMem)
{

    char result[BUFFER_SIZE];
    bzero(result, sizeof(result));

    while (fscanf(file, " %1023s", result) == 1)
    {
        if (strcmp(result, "VmRSS:") == 0)
        {
            if (fscanf(file, " %d", currRealMem) == EOF)
            {
                printGenericStatusError();
                return;
            }
        }

        if (strcmp(result, "VmHWM:") == 0)
        {
            if (fscanf(file, " %d", peakRealMem) == EOF)
            {
                printGenericStatusError();
                return;
            }
        }

        if (strcmp(result, "VmSize:") == 0)
        {
            if (fscanf(file, " %d", currVirtMem) == EOF)
            {
                printGenericStatusError();
                return;
            }
        }

        if (strcmp(result, "VmPeak:") == 0)
        {
            if (fscanf(file, " %d", peakVirtMem) == EOF)
            {
                printGenericStatusError();
                return;
            }
        }
    }
}

int isProcessWild(int currRealMem, int peakRealMem,
                  int currVirtMem, int peakVirtMem, int ID)
{
    if (currRealMem <= MAX_MEMORY)
    {
        return 0;
    }

    // Sends a polite request to terminate
    printf("Terminating process: %d. Too much memory!\n", ID);
    int result = kill(ID, SIGTERM);
    if (result)
    {
        printf("Error while terminating process: %d\n", ID);
        printf("Error message: %s\n", strerror(errno));
    }

    // Logs
    FILE *fp;
    fp = fopen("Diavolo.txt", "a");
    if (fp == NULL)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    printTimeStamp(fp);
    fprintf(fp, "WARNING: Exceeded %d KiB of Real Memory for process ID: %d\n", MAX_MEMORY, ID);
    fprintf(fp, "Process ID: %d\ncurrRealMem:%d KiB\npeakRealMem:%d KiB\ncurrVirtMem:%d KiB\npeakVirtMem:%d KiB\n", ID, currRealMem, peakRealMem, currVirtMem, peakVirtMem);
    fclose(fp);
    return 1;
}

static int processID[MAX_TRACK_SIZE] = {0};

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

        if (i >= MAX_TRACK_SIZE)
        {
            printf("Too many processes found\n");
            break;
        }
    }

    if (i == 0)
    {
        printf("No process was found\n");
        exit(EXIT_SUCCESS);
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
        if (status == NULL)
        {
            perror("popen");
            exit(EXIT_FAILURE);
        }

        checkStatus(status, &currRealMem, &peakRealMem, &currVirtMem, &peakVirtMem);
        if (isProcessWild(currRealMem, peakRealMem, currVirtMem, peakVirtMem, processID[i]))
        {
            processID[i] = 0;
        }

        pclose(status);
        currRealMem = 0, peakRealMem = 0, currVirtMem = 0, peakVirtMem = 0;
    }
}
