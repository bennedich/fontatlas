cmake_minimum_required( VERSION 2.8.4 )

set( CXX_STANDARD_REQUIRED ON )
set( CMAKE_C_COMPILER gcc )
set( CMAKE_CXX_COMPILER g++ )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )

project( fontatlas )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/" )
#set_directory_properties( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "CMakeCache.txt" )  # "make clean"

if( WIN32 )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" ) #multi-processor compilation
else()
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-attributes" )
endif()

find_package( Freetype REQUIRED )
add_subdirectory( "lib/msdfgen-1.1" )

include_directories(
	${FREETYPE_INCLUDE_DIRS}
	${MSDFGEN_INCLUDES}
)

add_executable( fontatlas
	src/main.cpp
	${MSDFGEN_SOURCES}
)
target_link_libraries( fontatlas
	${FREETYPE_LIBRARIES}
	${MSDFGEN_LIBRARIES}
)
