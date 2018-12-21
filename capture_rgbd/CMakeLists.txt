cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME nuitrack_gl_sample)
project(${PROJECT_NAME})

add_definitions(-std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0)

#set(NUITRACK_INCLUDE_PATH ${CMAKE_CURRENT_LIST_DIR}/../../Nuitrack/include ${CMAKE_CURRENT_LIST_DIR}/../../Nuitrack/include/middleware)
set(NUITRACK_INCLUDE_PATH /usr/local/include/nuitrack /usr/local/include/nuitrack/middleware)

set(OPENGL_LIBS glut GL)

#set(NUITRACK_LIBRARY_PATH ${CMAKE_CURRENT_LIST_DIR}/../../Nuitrack/lib/${PLATFORM_DIR}) 
set(NUITRACK_LIBRARY_PATH /usr/local/lib/nuitrack) 

include_directories(
	.
	${NUITRACK_INCLUDE_PATH}
)

link_directories(${NUITRACK_LIBRARY_PATH})

set(SOURCES
    src/NuitrackGLSample.cpp
    src/main.cpp)
    
add_executable(${PROJECT_NAME} ${SOURCES})

set(LIBS ${OPENGL_LIBS} nuitrack ${OpenCV_LIBS}) 

target_link_libraries(${PROJECT_NAME} ${LIBS})
