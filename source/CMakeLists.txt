include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(filter-test src/bootstrap.cpp src/FfmpegFilter.cpp include/FfmpegFilter.h)
target_link_libraries(filter-test CONAN_PKG::ffmpeg CONAN_PKG::libx264 CONAN_PKG::libfdk_aac dl)
