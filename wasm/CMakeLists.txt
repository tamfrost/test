# cmake_minimum_required(VERSION <specify CMake version here>)
project(iqplayer)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 14)

include_directories("include")
include_directories("/usr/local/include")
include_directories("/usr/local/include/fftw3")
#include_directories("/emsdk/upstream/emscripten/system/include")

message("COMMIT HASH: " ${COMMIT_HASH})
add_definitions(-DCOMMIT_HASH="${COMMIT_HASH}")

#find_library(fftw3Lib NAMES libfftw3.a PATH /wasm/fftw3-3.3.10/.libs/ NO_DEFAULT_PATH NO_CACHE)
#message(${fftw3Lib})

add_executable(iqplayer src/dsp.cpp src/SignalAnalyser.cpp)
set_target_properties(iqplayer PROPERTIES LINK_FLAGS "--bind -O3 \
-s WASM=1 \
-s DISABLE_EXCEPTION_CATCHING=0 \
-s ALLOW_MEMORY_GROWTH=1 \
-s EXPORTED_RUNTIME_METHODS=\"['ccall', 'cwrap', 'intArrayFromString', 'allocate', 'ALLOC_NORMAL', 'UTF8ToString', 'writeArrayToMemory', 'stringToUTF8']\" \
-s MODULARIZE=1 \
-s EXPORT_NAME=dspModule \
-s ASSERTIONS=1 \
-s SINGLE_FILE=1")

target_link_libraries(iqplayer "/usr/lib/x86_64-linux-gnu/libfftw3.a")

add_custom_command(TARGET iqplayer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:iqplayer>" ${PROJECT_SOURCE_DIR}/../lib/iqplayer-wasm.js)
