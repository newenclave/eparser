cmake_minimum_required( VERSION 2.8 )

set( PROJECT_NAME eparser )

project( ${PROJECT_NAME} )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED 14)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

list( APPEND src ./include/eparser
                 ./include/eparser/common
                 ./include/eparser/expressions
                 ./include/eparser/expressions/objects
                 ./tests
                 )

foreach(src_dir ${src})
    aux_source_directory(${src_dir} lib_src)
    file(GLOB headers ${src_dir}/*.h)
    list(APPEND lib_src ${headers}) 
endforeach()

add_executable( ${PROJECT_NAME} ${lib_src} )

