cmake_minimum_required(VERSION 3.0)
project(podorgana LANGUAGES C)

set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION POD game archive format organizer including libpodfmt library in C)

include(GNUInstallDirs)

if(WIN32 AND (MSYS OR MINGW))
	execute_process(COMMAND cygpath -ma /usr OUTPUT_VARIABLE CMAKE_INSTALL_PREFIX)
	string(STRIP ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX)
	add_definitions( -D__MSYS__ -DMSYS )
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(SOURCE_FILES
	libpodfmt/ccitt32_crc.c
	libpodfmt/libpodfmt.c
	libpodfmt/pod_common.c
	libpodfmt/pod1.c
	libpodfmt/pod2.c
	libpodfmt/pod3.c
	libpodfmt/pod4.c
	libpodfmt/pod5.c
	libpodfmt/pod6.c
	libpodfmt/epd.c
)

set(INCLUDE_FILES
	libpodfmt/ccitt32_crc.h
	libpodfmt/libpodfmt.h
	libpodfmt/pod_common.h
	libpodfmt/pod1.h
	libpodfmt/pod2.h
	libpodfmt/pod3.h
	libpodfmt/pod4.h
	libpodfmt/pod5.h
	libpodfmt/pod6.h
	libpodfmt/epd.h
)


set(EXAMPLE_SOURCE_FILES
	podorgana.c
)

# libpodfmt library
add_library( podfmt SHARED ${SOURCE_FILES} ${INCLUDE_FILES} )
target_include_directories(podfmt PUBLIC
			$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
			$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/libpodfmt>
			)

target_link_libraries( podfmt PUBLIC Msys-2.0 )
target_link_options( podfmt PUBLIC -L/usr/bin ) 

# podorgana example
add_executable( podorgana ${EXAMPLE_SOURCE_FILES} ${INCLUDE_FILES} )
target_link_libraries( podorgana PRIVATE podfmt )
set(install_prefix "${CMAKE_INSTALL_PREFIX}")
configure_file(libpodfmt/libpodfmt.pc.in ${PROJECT_BINARY_DIR}/libpodfmt.pc @ONLY)

set(ENV{PKG_CONFIG_PATH} "${PROJECT_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}")

install(TARGETS podfmt podorgana EXPORT ${PROJECT_NAME}Config
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libpodfmt
	)

install(FILES libpodfmt.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig/)
