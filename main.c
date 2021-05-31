#include "common.h"
#include <signal.h>
#include "monitor.h"

volatile sig_atomic_t sigint_received = 0;

void sigint_handler(int a)
{
    sigint_received = 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Name of the program is missing\n");
        printf("Usage: <name of the program>\n");
        return 2;
    }

    struct sigaction act;
    act.sa_handler = sigint_handler;
    sigaction(SIGINT, &act, NULL);

    setup(argv[1]);

    while (!sigint_received)
    {
        analyse();
        sleep(10);
    }

    printf("Normal shutdown\n");
    return 0;
}
