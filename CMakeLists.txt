cmake_minimum_required(VERSION 3.20)
project(WinProj CXX)

set(ClassesDir src/classes/)
set(AssetsDir src/assets/)
set(Dependencies deps/)
set(ImguiPath ${Dependencies}imgui/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512bw -mavx512cd -mavx512dq")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX512")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Classes 
    ${ClassesDir}Assets/Mesh/Mesh.cpp
    ${ClassesDir}Assets/Light/Light.cpp
    ${ClassesDir}Assets/Vertex.hpp
    ${ClassesDir}Assets/Color4b.hpp
    ${ClassesDir}Assets/Texture/Texture.cpp
    ${ClassesDir}Assets/Material/Material.cpp
    
    ${ClassesDir}Engine/Renderer/Renderer.cpp
    ${ClassesDir}Engine/Scene/Scene.cpp
    ${ClassesDir}Engine/Camera/Camera.cpp
    ${ClassesDir}Engine/Triangle/Triangle.cpp
    ${ClassesDir}Engine/Settings.hpp
    ${ClassesDir}Engine/Buffers/ColorBuffer/ColorBuffer.cpp
    ${ClassesDir}Engine/Buffers/DepthBuffer/DepthBuffer.cpp
    
    ${ClassesDir}Loader/Loader.cpp

    ${ClassesDir}Viewer/ShaderProg/ShaderProg.cpp
    ${ClassesDir}Viewer/Shader/Shader.cpp
    ${ClassesDir}Viewer/Shader/ShaderType.cpp
    ${ClassesDir}Viewer/Window/Window.cpp
    ${ClassesDir}Viewer/Menu/Menu.cpp
    ${ClassesDir}Viewer/App/App.cpp

    ${ClassesDir}path.hpp
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
add_executable(main src/main.cpp ${IMGUI_SRCS} ${Classes})
target_precompile_headers(main
  PUBLIC
    include/pch.hpp
)


add_subdirectory(deps/glm)
add_subdirectory(deps/glad)
add_subdirectory(deps/glfw)
add_subdirectory(deps/tinyobjloader)
add_subdirectory(deps/stb_image)
target_link_libraries(main PUBLIC 
                            glm
                            glfw
                            opengl32
                            tinyobjloader
                            stb_image
                            glad
)



target_include_directories(main PUBLIC ${ClassesDir})
target_include_directories(main PUBLIC ${AssetsDir})
target_include_directories(main PUBLIC ${ImguiPath})
target_include_directories(main PUBLIC include)
target_include_directories(main PUBLIC deps/stb_image/include)

#target_include_directories(main PUBLIC ${Dependencies}tinyobjloader)

