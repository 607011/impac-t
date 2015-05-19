cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(impact)

# default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to 'Release' as none was specified.")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
	# Set the possible values of build type for cmake-gui
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo" "ctinternal")
endif()

find_package(Boost 1.55.0 REQUIRED COMPONENTS system filesystem regex serialization signals)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
# FIXME this is needed for libminizip and looks like a bug in that pkg-config file. We shouldn't need this
find_package(ZLIB REQUIRED)
# XXX Needed for XInitThreads()
find_package(X11 REQUIRED)

# we don't have CMake modules for all libraries used, but they all come with pkg-config files AFAICT
find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML REQUIRED sfml-graphics sfml-window sfml-audio sfml-system)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(MINIZIP REQUIRED minizip)
pkg_check_modules(BOX2D REQUIRED box2d)

add_subdirectory(Impact)
