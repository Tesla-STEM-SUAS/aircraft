## Dependencies

**You will need**:
* **Microsoft Visual Studio C++** supporting C++17 or higher; MinGW is not supported
* CMake 3.1
* MAVSDK

To install MAVSDK:
1. Download `mavsdk-windows-x64.zip` from the latest [MAVSDK release](https://github.com/mavlink/MAVSDK/releases)
2. Extract the downloaded files to `C:\mavsdk`

## Building from source

```
cmake -S . -B build
cmake --build build --config Release
```
