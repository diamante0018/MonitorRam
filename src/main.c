#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "common.h"
#include "monitor.h"

volatile sig_atomic_t sigint_received = 0;

void sigint_handler(int a) {
  sigint_received = 1;
  (void) a;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Name of the program is missing\n");
    printf("Usage: <name of the program>\n");
    return 2;
  }

  struct sigaction act;
  act.sa_handler = sigint_handler;
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
