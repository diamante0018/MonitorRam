#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "monitor.h"

#include "common.h"
#include "utils.h"

#define MAX_MEMORY 1048576
#define MAX_TRACK_SIZE 30

struct process_monitor {
  int id;
  int active;
};

static struct process_monitor processes[MAX_TRACK_SIZE];

FILE* popen(const char* command, const char* type);
int pclose(FILE* stream);
char* strerror(int errnum);

static void print_generic_status_error() {
  printf("Error while parsing file\n");
  printf("Processes may be monitored incorrectly\n");
}

static void check_status(FILE* file, int* curr_real_mem, int* peak_real_mem,
                         int* curr_virt_mem, int* peak_virt_mem) {

  char result[BUFFER_SIZE];
  memset(result, 0, sizeof result);

  while (fscanf(file, " %1023s", result) == 1) {
    if (strcmp(result, "VmRSS:") == 0) {
      if (fscanf(file, " %d", curr_real_mem) == EOF) {
        print_generic_status_error();
        return;
      }
    }

    if (strcmp(result, "VmHWM:") == 0) {
      if (fscanf(file, " %d", peak_real_mem) == EOF) {
        print_generic_status_error();
        return;
      }
    }

    if (strcmp(result, "VmSize:") == 0) {
      if (fscanf(file, " %d", curr_virt_mem) == EOF) {
        print_generic_status_error();
        return;
      }
    }

    if (strcmp(result, "VmPeak:") == 0) {
      if (fscanf(file, " %d", peak_virt_mem) == EOF) {
        print_generic_status_error();
        return;
      }
    }
  }
}

static int is_process_wild(int curr_real_mem, int peak_real_mem,
                           int curr_virt_mem, int peak_virt_mem, int id) {
  if (curr_real_mem <= MAX_MEMORY) {
    return 0;
  }

  // Sends a polite request to terminate
  printf("Terminating process: %d. Memory: %d\n", id, curr_real_mem);
  int result = kill(id, SIGTERM);

  // Logs
  FILE* fp;
  fp = fopen("log.txt", "a");
  if (fp == NULL) {
    printf("Error while opening log.txt\n");
    perror("popen");
    return 1;
  }

  char time[32];
  get_time_stamp(time);

  fprintf(fp, "\nTimeStamp: %s\n", time);
  fprintf(fp, "WARNING: Exceeded %d KiB of Real Memory for process ID: %d\n",
          MAX_MEMORY, id);
  fprintf(fp,
          "Process ID: %d\ncurr_real_mem:%d KiB\npeak_real_mem:%d "
          "KiB\ncurr_virt_mem:%d KiB\npeak_virt_mem:%d KiB\n",
          id, curr_real_mem, peak_real_mem, curr_virt_mem, peak_virt_mem);

  if (result) {
    fprintf(fp, "Error while terminating process: %d\n", id);
    fprintf(fp, "Error message: %s\n", strerror(errno));
  }

  fclose(fp);

  return 1;
}

int setup(const char* pid) {
  memset(&processes, 0, sizeof(processes));

  char* command = va("pgrep %s", pid);
  FILE* file = popen(command, "r");
  if (!file) {
    perror("popen");
    return 1;
  }

  char result[BUFFER_SIZE];
  memset(result, 0, sizeof result);

  int i = 0;
  while (fgets(result, BUFFER_SIZE, file) && i < MAX_TRACK_SIZE) {
    printf("Process ID: %s", result);
    processes[i].id = strtol(result, NULL, 10);
    processes[i].active = 1;

    ++i;
  }

  if (!i) {
    printf("No process was found\n");
    pclose(file);
    return 1;
  }

  return 0;
}

int analyse() {
  int i;
  FILE* status;

  for (i = 0; i < MAX_TRACK_SIZE && processes[i].active == 1; ++i) {
    char* command = va("cat /proc/%d/status", processes[i].id);
    status = popen(command, "r");
    if (!status) {
      perror("popen");
      return 1;
    }

#ifdef _DEBUG
    printf("Checking %d\n", processes[i].id);
#endif

    int curr_real_mem = 0, peak_real_mem = 0, curr_virt_mem = 0,
        peak_virt_mem = 0;

    check_status(status, &curr_real_mem, &peak_real_mem, &curr_virt_mem,
                 &peak_virt_mem);
    if (is_process_wild(curr_real_mem, peak_real_mem, curr_virt_mem,
                        peak_virt_mem, processes[i].id)) {
      processes[i].active = 0;
    }

    pclose(status);
  }

  return 0;
}
