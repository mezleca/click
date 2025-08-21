# elterclick  

A simple autoclicker with a lightweight **ImGui** interface.  
Supports both **Windows** and **Linux (X11)**.  

## Features  
- Cross-platform (Windows & Linux)  
- Simple GUI using **Dear ImGui** 
- Click randomizer

## Requirements
- CMake 3.13+
- Linux: gcc, X11, Xrandr, pthread
- Windows: MinGW / mingw32-make

## Build & Run  

### Linux 
```bash
# ensure proper perms
chmod +x ./elter.sh

# configure project
./elter.sh --configure

# build 
./elter.sh --build

# run       
./elter.sh --run
```

### Windows
```bash
# configure project
.\elter.ps1 --configure 

# build 
.\elter.ps1 --build

# run
.\elter.ps1 --run
```