cmake_minimum_required(VERSION 3.20)
project(WinProj)

set(ClassesDir src/classes/)
set(AssetsDir src/assets/)
set(Dependencies deps/)
set(GLAD_SOURCES_DIR ${Dependencies}glad/)
set(FeatureDetector ${Dependencies}featureDetector/src/)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512bw -mavx512cd -mavx512dq")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Classes 

)
add_executable(main src/main.cpp ${Classes})


add_subdirectory(deps/glm)
add_subdirectory("${GLAD_SOURCES_DIR}/cmake" glad_cmake)
add_subdirectory(deps/glfw)
add_subdirectory(deps/featureDetector)
add_subdirectory(deps/tinyobjloader)
target_link_libraries(main PUBLIC FeatureDetector)
target_link_libraries(main PUBLIC glm)
target_link_libraries(main PUBLIC glfw)
target_link_libraries(main PUBLIC opengl32)
target_link_libraries(main PUBLIC tinyobjloader)
glad_add_library(glad_gl_core_46 REPRODUCIBLE API gl:core=4.6)
target_link_libraries(main PUBLIC glad_gl_core_46)


target_include_directories(main PUBLIC ${ClassesDir})
target_include_directories(main PUBLIC ${AssetsDir})
target_include_directories(main PUBLIC include)
target_include_directories(main PUBLIC ${FeatureDetector})
target_include_directories(main PUBLIC ${Dependencies}thread-pool/include)
target_include_directories(main PUBLIC ${Dependencies}tinyobjloader)

