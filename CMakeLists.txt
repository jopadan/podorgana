cmake_minimum_required(VERSION 3.0)
project(podorgana LANGUAGES C)

set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION POD game archive format organizer including libpodfmt library in C)

include(GNUInstallDirs)
include(FindPkgConfig)

set(EXAMPLE_SOURCE_FILES
	podorgana.c
)

pkg_search_module(PODFMT REQUIRED libpodfmt)

# podorgana example
add_executable( podorgana ${EXAMPLE_SOURCE_FILES} )
target_compile_options( podorgana PUBLIC ${PODFMT_CFLAGS} )
target_include_directories( podorgana PUBLIC ${PODFMT_INCLUDEDIR} )
target_link_libraries( podorgana PUBLIC ${PODFMT_LIBRARIES} )

install(TARGETS podorgana EXPORT ${PROJECT_NAME}Config)

