cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(aircraft)

list(APPEND CMAKE_PREFIX_PATH "C:/mavsdk")

find_package(MAVSDK REQUIRED)

add_compile_options(/Z7)

set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)

option(aircraft_HEADER_ONLY "Build as header-only" OFF)

if (aircraft_HEADER_ONLY)
    add_library(aircraft INTERFACE)
    target_include_directories(aircraft INTERFACE include)
else()
    add_library(aircraft "src/aircraft.cpp")
    target_include_directories(aircraft PUBLIC include)
    target_link_libraries(aircraft PUBLIC MAVSDK::mavsdk)
    target_compile_features(aircraft PUBLIC cxx_std_20)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET aircraft PROPERTY CXX_STANDARD 20)
endif()
