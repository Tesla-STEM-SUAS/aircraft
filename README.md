# SUAS@STEM Aircraft

SUAS@STEM's aircraft backend for autonomous missions and ground control station dashboard. Communication with the aircraft's flight controller is through MAVSDK.

## Dependencies

Windows is the only supported operating system; support for Linux and Android may be made in the future (as they are all supported by all dependencies).

**You will need**:
* **Microsoft Visual Studio C++** supporting C++17 or higher; MinGW is not supported
* CMake 3.1
* MAVSDK
* OpenCV
* `nlohmann::json`

These dependencies are expected to be in the `extern/` directory.

Run `.\install.ps1` to install all required dependencies automatically, including the MSVC++ Build Tools and CMake, if they are not already installed.

## Building from source

Sources are built into a shared library intended to be used with a Git submodule and linked to an executable.

```
cmake -S . -B build
cmake --build build --config Release
```
