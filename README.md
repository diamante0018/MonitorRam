# MonitorRam
A simple program that monitors real memory usage of processes.

# Example
I have multiple processes, and all of them are named similarly. In my case, all the processes I need to monitor begin with the same string.
The program will monitor up to 30 different processes; the max value is defined in `monitor.c`.
I would start this program using the following command: `./monitor iw5`. It will monitor all my game servers as they all share a common name on the process list.
In my case, each process should not exceed more than 1 GiB of real memory, again, the max value is defined in `monitor.c`.
If that's the case, a memory leak has occurred, and this program will send a request to terminate that process gracefully (SIGTERM).
SIGTERM usually works for me. I have in place a separate system to automatically restart my game servers.
I hope this program can be useful to others. I wrote this alongside another script that automatically restarts my game servers.

## How to build from source?

Generate the make file using the provided premake5 lua script
Usage: `./monitor <NAME OF PROGRAM>`
This program is written in C89 using some GNU extensions
