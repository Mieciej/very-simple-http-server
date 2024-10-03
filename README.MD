# Very Simple HTTP server implemented in C.
Currently it only supports `HTML`,`JPG`, `css` and `js` files as responses, which have to be contained in ./res directory.

## Usage

To build run:
```
gcc -o server server.c
```

Start server using:
```
./server <portno>
```
