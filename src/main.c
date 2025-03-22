#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "monitor.h"

volatile sig_atomic_t sigint_received = 0;

void sigint_handler(int a) {
  sigint_received = 1;
  (void)a;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: <name of the program>\n");
    return 2;
  }

  struct sigaction act;
  memset(&act, 0, sizeof(act));

  act.sa_handler = sigint_handler;
  sigemptyset(&act.sa_mask); // Initialize the signal mask to empty
  act.sa_flags = 0;          // Set flags to 0

  sigaction(SIGINT, &act, NULL);

  int result = setup(argv[1]);
  if (result) {
    return 1;
  }

  while (!sigint_received) {
    if (analyse()) {
      return 3;
    }

    sleep(10);
  }

  printf("Normal shutdown\n");
  return 0;
}
