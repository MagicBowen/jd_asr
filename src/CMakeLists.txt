file(GLOB_RECURSE all_files
*.cpp
*.cc
*.c++
*.c
*.C)

add_library(asr_jd STATIC ${all_files})

install(TARGETS asr_jd ARCHIVE DESTINATION lib)