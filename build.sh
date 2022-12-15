#!/bin/bash

mkdir -p bin
gcc -g source/server.c -o bin/server
gcc -g source/client.c -o bin/client

