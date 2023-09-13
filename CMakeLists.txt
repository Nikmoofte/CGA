cmake_minimum_required(VERSION 3.5)
project(WinProj)

set(ClassesDir src/classes/)

set(Classes 
    
)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
add_executable(main src/main.cpp ${Classes})

target_include_directories(main PUBLIC include ${ClassesDir})