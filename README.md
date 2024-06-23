# Dicsord

A Chat app written in C using the ncurses library for the UI.

## Compile
make sure Cmake and ncurses are installed

``` bash
mkdir build
cd build
cmake ..
cmake --build
```

## Running the application
run the server using
```bash
./server
```
run the client using
```bash
./client --ip <IP Address> --port <PORT> --name <Username>
```
