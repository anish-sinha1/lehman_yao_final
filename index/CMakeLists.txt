add_library(index include/common.h src/lehman-yao.c include/lehman-yao.h)
target_include_directories(index PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include /usr/local/include)
link_directories(/usr/local/lib)
target_link_libraries(index PUBLIC cslice)
