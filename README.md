# A simple http server written in C

## Description
This is a simple implementation of the http protocol written in c using the unix socket api.
Currently it supports the following features:

* Creating a http server
* Accepting requests
* Sending requests
* Routes
* Sending HTML
* Sending images (does have some issues right now)

## Setup
This project is designed to be a shared library which will be linked with your code
and remember to include the header file which is in `include/http_server.h`.

### Compiling the shared library
`make`

### Installing the library
`sudo make install`

### Compiling example_server.c
`make example`

### Running the example
`bin/server`

