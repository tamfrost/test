#!/usr/bin/env bash

emcc --version
emcc --bind -O3 \
-Iinclude \
-I/usr/local/include/fftw3 \
-s WASM=1 \
-s DISABLE_EXCEPTION_CATCHING=0 \
-s ALLOW_MEMORY_GROWTH=1 \
-s EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'intArrayFromString', 'allocate', 'ALLOC_NORMAL', 'UTF8ToString', 'writeArrayToMemory']" \
-s MODULARIZE=1 \
-s EXPORT_NAME="dspModule" \
-s ASSERTIONS=1 \
-s SINGLE_FILE=1 \
-DCOMMIT_HASH=\"$CI_COMMIT_SHORT_SHA\" \
-o ../lib/iqplayer-wasm.js \
src/dsp.cpp src/SignalAnalyser.cpp /usr/lib/x86_64-linux-gnu/libfftw3.a
