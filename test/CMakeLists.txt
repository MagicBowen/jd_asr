project(test_asr_jd)

include_directories($ENV{GTEST_HOME}/include)

file(GLOB_RECURSE all_files
*.cpp
*.cc
*.c++
*.c
*.C)

link_directories($ENV{GTEST_HOME}/lib)

add_executable(test_asr_jd ${all_files})

target_link_libraries(test_asr_jd asr_jd gtest)