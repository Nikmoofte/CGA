cmake_minimum_required(VERSION 3.5)
project(WinProj)

set(ClassesDir src/classes/)
set(Dependencies deps/)

set(Classes 
    ${ClassesDir}App/App.cpp
    ${ClassesDir}Box/Box.cpp
)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
add_executable(main src/main.cpp ${Classes})

add_subdirectory(deps/glm)
target_link_libraries(main PUBLIC glm)

target_include_directories(main PUBLIC ${ClassesDir})