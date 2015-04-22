# source files
set(impact_sources
		Ball.cpp
		Block.cpp
		Body.cpp
		Bumper.cpp
		Explosion.cpp
		globals.cpp
		Ground.cpp
		Impact.cpp
		Level.cpp
		linux_amd64.cpp
		LocalSettings.cpp
		main.cpp
		Racket.cpp
		ScrollArea.cpp
		sha1.cpp
		stdafx.cpp
		Text.cpp
		util.cpp
		Wall.cpp
)

# TODO check out, what's missing for Recorder.cpp to be built
#if(not UNIX)
#		list(APPEND impact_sources Recorder.cpp)
#endif()

# name our target
add_executable(impact ${impact_sources})

# set include directories
target_include_directories(impact PUBLIC
		${Boost_INCLUDE_DIRS}
		${GLEW_INCLUDE_DIRS}
		${OPENGL_INCLUDE_DIR}
		${ZLIB_INCLUDE_DIRS}
		${SFML_INCLUDE_DIRS}
		${GTK3_INCLUDE_DIRS}
		${MINIZIP_INCLUDE_DIRS}
		${BOX2D_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}
)

# we need some flags and definitions
set(CMAKE_CXX_FLAGS_DEBUG          "-DDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE        "-DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL     "-DNDEBUG")

# custom build target
set(CMAKE_BUILD_TYPE ctinternal)
set(CMAKE_CXX_FLAGS_CTINTERNAL "-DDEBUG -DCT_VERSION_INTERNAL")

# right now we only set this for Linux
if(UNIX)
		set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb")
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O2 -ggdb")
		set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} -O4")
		set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os")
		set(CMAKE_CXX_FLAGS_CTINTERNAL     "${CMAKE_CXX_FLAGS_CTINTERNAL} -O4")

		# FIXME enforce CMake 3.1 usage, then we can enable C++11 as a target feature
		include(CheckCXXCompilerFlag)
		CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
		if(COMPILER_SUPPORTS_CXX11)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
		else()
			message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
		endif()

		# add default defines
		target_compile_definitions(impact
			PUBLIC "NO_RECORDER"
			PUBLIC "LINUX_AMD64"
		)

		# since we have Gtk in our dependency chain, we use -Wl,--as-needed here
		set_target_properties(impact PROPERTIES LINK_FLAGS "-Wl,--as-needed")
endif()

# link our target
target_link_libraries(impact LINK_PUBLIC
		${Boost_LIBRARIES}
		${GLEW_LIBRARIES}
		${OPENGL_LIBRARIES}
		${ZLIB_LIBRARIES}
		${SFML_LIBRARIES}
		${GTK3_LIBRARIES}
		${BOX2D_LIBRARIES}
		${MINIZIP_LIBRARIES}
		${CMAKE_THREAD_LIBS_INIT}
)

