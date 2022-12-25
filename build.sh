#!/bin/bash

mkdir -p bin
gcc -g -O0 -Wall source/server.c -o bin/server
gcc -g -O0 -Wall source/client.c -o bin/client

