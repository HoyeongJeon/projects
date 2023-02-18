# Simple chat application in C
This is a Server/Client chat application using TCP Socket and threads.

## Running
* Working on Linux

- server (should run this first)
$ gcc -o server sever.c -lpthread

- client
$ gcc -o client client.c -lpthread
