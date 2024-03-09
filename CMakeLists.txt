cmake_minimum_required(VERSION 3.20)
project(WinProj)

set(ClassesDir src/classes/)
set(AssetsDir src/assets/)
set(Dependencies deps/)
set(FeatureDetector ${Dependencies}featureDetector/src/)
set(ImguiPath ${Dependencies}imgui/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512bw -mavx512cd -mavx512dq")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Classes 
    ${ClassesDir}Viewer/ShaderProg/ShaderProg.cpp
    ${ClassesDir}Viewer/Shader/Shader.cpp
    ${ClassesDir}Viewer/Shader/ShaderType.cpp
    ${ClassesDir}Viewer/Window/Window.cpp
    ${ClassesDir}Viewer/Menu/Menu.cpp
    ${ClassesDir}Viewer/App/App.cpp
)
set(IMGUI_SRCS
    ${ImguiPath}imgui_demo.cpp
    ${ImguiPath}imgui_draw.cpp
    ${ImguiPath}imgui_tables.cpp
    ${ImguiPath}imgui_widgets.cpp
    ${ImguiPath}imgui.cpp
    ${ImguiPath}backends/imgui_impl_opengl3.cpp
    ${ImguiPath}backends/imgui_impl_glfw.cpp
)
add_executable(main src/main.cpp ${Classes} ${IMGUI_SRCS})


add_subdirectory(deps/glm)
add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)
add_subdirectory(deps/featureDetector)
add_subdirectory(deps/tinyobjloader)
target_link_libraries(main PUBLIC 
                            FeatureDetector
                            glm
                            glfw
                            opengl32
                            tinyobjloader
                            glad
)



target_include_directories(main PUBLIC ${ClassesDir})
target_include_directories(main PUBLIC ${AssetsDir})
target_include_directories(main PUBLIC ${ImguiPath})
target_include_directories(main PUBLIC include)
target_include_directories(main PUBLIC ${FeatureDetector})
target_include_directories(main PUBLIC ${Dependencies}thread-pool/include)
#target_include_directories(main PUBLIC ${Dependencies}tinyobjloader)

