cmake_minimum_required(VERSION 3.1)
project(post_stretch)

set (CMAKE_CXX_STANDARD 11)

find_package(Boost 1.54.0 REQUIRED COMPONENTS system filesystem program_options)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

FIND_PACKAGE(OpenMP)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPENMP_CXX_FLAGS}") 
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_library(CAIRO_LIBRARY names libcairo cairo HINTS /usr/lib64)
find_path(CAIRO_INCLUDE_DIR names cairo.h HINTS /usr/include/cairo)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CAIRO_INCLUDE_DIR})

add_library(stretch
    GCodeParser.cpp
    GCodeDebugView.cpp
    StretchAlgorithmImpl.cpp
    microgeo.cpp
    )

add_executable(post_stretch
    main.cpp
   )

target_link_libraries(post_stretch
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY} 
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${CAIRO_LIBRARY}
    stretch
    )

find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc_stretch
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
        )
endif(DOXYGEN_FOUND)

install(TARGETS post_stretch RUNTIME DESTINATION bin)

