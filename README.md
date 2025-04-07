# Gameloft J2ME Tools

A GUI tool and Python scripts for modifying Gameloft's J2ME asset files.

## Building

The following build instructions should work for both Windows and Linux, provided you have installed the appropriate dependencies. [MSYS2](https://www.msys2.org/) is recommended for Windows.

### Dependencies
- SDL3
- CMake

### Linux

```
git clone --recursive https://github.com/palaceswitcher/Gameloft-J2ME-Tools
cd Gameloft-J2ME-Tools
mkdir build && cd build
cmake ..
make
```

### Windows (MSYS2)

Using specifically UCRT64 is recommended, though other environments may also work under MSYS2.

#### Building (MSYS2)

```
git clone --recursive https://github.com/palaceswitcher/Gameloft-J2ME-Tools
cd Gameloft-J2ME-Tools
mkdir build && cd build
cmake -G "Unix Makefiles" ..
make
```
