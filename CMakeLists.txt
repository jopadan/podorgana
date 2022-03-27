cmake_minimum_required(VERSION 3.0)
project(podorgana LANGUAGES C)

set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "POD game archive format organizer")

include(GNUInstallDirs)
include(FindPkgConfig)

set(EXAMPLE_SOURCE_FILES
	podorgana.c
)

pkg_search_module(PODFMT REQUIRED libpodfmt)

# podorgana example
add_executable( ${PROJECT_NAME} ${EXAMPLE_SOURCE_FILES} )
target_compile_options( ${PROJECT_NAME} PUBLIC -g ${PODFMT_CFLAGS} )
target_include_directories( ${PROJECT_NAME} PUBLIC ${PODFMT_INCLUDEDIR} )
target_link_libraries( ${PROJECT_NAME} PUBLIC ${PODFMT_LIBRARIES} )

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Config)

