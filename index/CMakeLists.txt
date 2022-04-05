add_library(index include/common.h src/fsutil.c include/fsutil.h src/lehman-yao.c include/lehman-yao.h)
target_include_directories(index PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)