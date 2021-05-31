# MonitorRam
A simple program that monitors real memory usage of processes.
For example, if you have multiple processes, like numerous game servers, that all begin with the same prefix, you can type that prefix as a command-line argument.
The program will monitor up to 30 different processes.
In my case, I have a few game servers that all share "iw5" as the name prefix.
I would type `./prog iw5` to monitor the actual memory usage of all my game servers.
In my case, each process should not exceed more than 1 GiB of real memory.
If that's the case, a memory leak has occurred, and this program will send a request to terminate that process gracefully (SIGTERM).
Max memory usage is defined in monitor.c

## How to?

Compile from source using the provided makefile
`./prog <NAME OF PROGRAM>`
