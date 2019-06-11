set(PROJECT_NAME SoftwareRenderer)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_EXE_LINKER_FLAGS -static)

cmake_minimum_required(VERSION 3.9)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)

add_executable(SoftwareRenderer src/main.c src/camera.h src/camera.c src/display.c src/display.h src/bitmap.c src/bitmap.h src/geometry.c src/geometry.h src/lodepng.c src/lodepng.h src/objfile.c src/objfile.h src/model.h src/model.c src/input.c src/input.h src/bresenham.c src/bresenham.h src/rendercontext.c src/rendercontext.h src/shader.c src/shader.h src/uniforms.c src/uniforms.h src/textrenderer.c src/textrenderer.h src/stopwatch.c src/stopwatch.h src/shaderutils.c src/shaderutils.h src/postprocess.c src/postprocess.h)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Threads REQUIRED)


message(STATUS "found OpenGL: '${OPENGL_FOUND}'  at: '${OPENGL_INCLUDE_DIR}', '${OPENGL_LIBRARIES}'")
message(STATUS "found GLUT: '${GLUT_FOUND}'  at: '${GLUT_INCLUDE_DIR}', '${GLUT_LIBRARIES}'")
message(STATUS "found Threads: '${THREADS_FOUND}'")
message(STATUS "  use win32_threads '${CMAKE_USE_WIN32_THREADS_INIT}'")
message(STATUS "  use pthreads '${CMAKE_USE_PTHREADS_INIT}'")
message(STATUS "  use sproc '${CMAKE_USE_SPROC_INIT}'")

include_directories(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} Threads::Threads)