# elterclick  

A simple autoclicker with a lightweight **ImGui** interface.  
Supports both **Windows** and **Linux (X11)**.  

## Features  
- Cross-platform (Windows & Linux)  
- Simple GUI using **Dear ImGui** 
- Click randomizer

## Requirements
- CMake 3.13+
- Linux: gcc, x11 stuff
- Windows: MinGW / mingw32-make

## Build & Run  

```bash
# ensure proper perms on linux
chmod +x ./elter.sh

# configure project
./elter --configure

# build 
./elter --build

# run       
./elter --run
```
