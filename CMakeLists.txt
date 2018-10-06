cmake_minimum_required(VERSION 3.10)
project(wav_revival_lib_cpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/include)
include_directories(src)


#add_library(wav_revival_lib_cpp SHARED src/integration_test_kek.cpp src/include/queue.h src/include/wavfile.h)
add_executable(integration_test src/integration_test_kek.cpp src/include/queue.h src/include/wavfile.h)
