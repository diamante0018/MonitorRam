# MonitorRam
A simple program that monitors real memory usage of processes.
For example, I have multiple processes, all of them are named similarly. In my case, I use a common prefix as a command-line argument.
The program will monitor up to 30 different processes; the max value is defined in `monitor.c`.
I have a few game servers that all share "iw5" as the name prefix.
I would start this program with `./monitor iw5` to monitor the memory usage of all my game servers.
In my case, each process should not exceed more than 1 GiB of real memory, again, the max value is defined in `monitor.c`.
If that's the case, a memory leak has occurred, and this program will send a request to terminate that process gracefully (SIGTERM).
SIGTERM usually works for me. I have in place a separate system to automatically restart my game servers.
I hope this program can be useful to others.

## How to?

Compile from source using the provided makefile.
Execute the program like this: `./monitor <NAME OF PROGRAM>`
