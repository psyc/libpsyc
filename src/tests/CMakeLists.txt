cmake_minimum_required(VERSION 2.6)


FILE(GLOB SRC
	parser.c
	init.cpp
	MainWindow.*
	interface.cpp
	Window.*
	ConnectWindow.h
	)

if(GTKMM_FOUND)
	include_directories(${GTKMM_INCLUDE_DIRS})
	add_executable(nprog ${NPROG_CLIENT_SRC})
	target_link_libraries(nprog ${GTKMM_LIBRARIES} -pthread -lgthread-2.0 -lrt -lglib-2.0 )
endif()

