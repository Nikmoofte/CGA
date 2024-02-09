cmake_minimum_required(VERSION 3.20)
project(WinProj)

set(ClassesDir src/classes/)
set(AssetsDir src/assets/)
set(Dependencies deps/)
set(FeatureDetector ${Dependencies}featureDetector/src/)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512bw -mavx512cd -mavx512dq")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Classes 
    ${ClassesDir}App/App.cpp
    ${ClassesDir}ObjParser/ObjParser.cpp
    ${ClassesDir}Object/Object.cpp
    ${ClassesDir}Camera/Camera.cpp
    ${ClassesDir}Renderer/Renderer.cpp
    ${ClassesDir}ShaderProg/ShaderProg.cpp
    ${ClassesDir}Shader/Shader.cpp
    ${ClassesDir}Shader/ShaderType.cpp
    ${ClassesDir}VBO/VBO.cpp
    ${ClassesDir}VAO/VAO.cpp
    ${ClassesDir}EBO/EBO.cpp
    ${ClassesDir}VAO/VBLayout.cpp
)
add_executable(main src/main.cpp src/glad.c ${Classes})


add_subdirectory(deps/glm)
add_subdirectory(deps/glfw)
add_subdirectory(deps/featureDetector)
target_link_libraries(main PUBLIC FeatureDetector)
target_link_libraries(main PUBLIC glm)
target_link_libraries(main PUBLIC glfw)
target_link_libraries(main PUBLIC opengl32)


target_include_directories(main PUBLIC ${ClassesDir})
target_include_directories(main PUBLIC ${AssetsDir})
target_include_directories(main PUBLIC include)
target_include_directories(main PUBLIC ${FeatureDetector})