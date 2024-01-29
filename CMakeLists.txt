cmake_minimum_required(VERSION 3.20)
project(WinProj)

set(ClassesDir src/classes/)
set(AssetsDir src/assets/)
set(Dependencies deps/)

set(Classes 
    ${ClassesDir}App/App.cpp
    ${ClassesDir}ObjParser/ObjParser.cpp
    ${ClassesDir}Object/Object.cpp
    ${ClassesDir}Camera/Camera.cpp
)
add_executable(main WIN32 src/main.cpp ${Classes})

add_subdirectory(deps/glm)
target_link_libraries(main PUBLIC glm)
target_link_libraries(main PUBLIC windowscodecs)
target_link_libraries(main PUBLIC Msimg32)
target_link_libraries(main PUBLIC Gdiplus)

target_include_directories(main PUBLIC ${ClassesDir})
target_include_directories(main PUBLIC ${AssetsDir})