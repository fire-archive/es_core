cmake_minimum_required( VERSION 2.8 )
cmake_policy( SET CMP0007 NEW ) # this one it actually complained

# NOTE: we assume that CMAKE_INSTALL_PREFIX is provided, and that all the dependencies are installed there already

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_BINARY_DIR}/Run/CMake")

project( es_core )

find_package( OGRE REQUIRED )

# FIXME: replace with a proper SDL2 cmake package, if/when there is such a thing
# or piggyback on the sdl2 pkg-config (PKG_CONFIG_PATH and all that)
# --cflags: -D_THREAD_SAFE -I/Users/timo/es_core/run/include/SDL2 
# --libs: -L/Users/timo/es_core/run/lib -lSDL2

# NOTE: under the assumption that all the es_core dependencies are getting installed to the same prefix,
# the above has defined all the header and library directories needed for ZeroMQ

link_directories( "${CMAKE_INSTALL_PREFIX}/lib" )

function( add_es executable)
include_directories( "${CMAKE_INSTALL_PREFIX}/include/SDL2" )
include_directories( ${OGRE_INCLUDE_DIRS} )
include_directories( "${CMAKE_INSTALL_PREFIX}/include" )

target_link_libraries( ${executable} czmq ${LIBZMQ} )
target_link_libraries( ${executable} SDL2 )
target_link_libraries( ${executable} ${OGRE_LIBRARIES} )
target_link_libraries( ${executable} ${WINDOWS_LIBS} )

string(TOUPPER ${executable} EXECUTABLE )
get_target_property( ${EXECUTABLE}_INCLUDES ${executable} INCLUDE_DIRECTORIES )
# FIXME: fix ogre cmake so it sets this up properly instead of digging it up ourselves
#pkg_message( BSP "BSP: ${OGRE_Plugin_BSPSceneManager_INCLUDE_DIR}" )
include_directories(${CMAKE_SOURCE_DIR}/${executable}_src)
include_directories(${CMAKE_INSTALL_PREFIX}/include/OGRE/Plugins/BSPSceneManager)
list( APPEND ${EXECUTABLE}_INCLUDES "${CMAKE_SOURCE_DIR}/${executable}_src" )

if ( APPLE )
  list( APPEND ${EXECUTABLE}_INCLUDES "${CMAKE_INSTALL_PREFIX}/include/OGRE/RenderSystems/GL" )
  list( APPEND ${EXECUTABLE}_INCLUDES "${CMAKE_INSTALL_PREFIX}/include/OGRE/RenderSystems/GL/OSX" )
  list( APPEND ${EXECUTABLE}_SOURCES OSX_wrap.mm )
  
  set_target_properties( ${executable} PROPERTIES MACOSX_BUNDLE TRUE )
  find_library( FOUNDATION_FRAMEWORK Foundation )
  find_library( APPKIT_FRAMEWORK AppKit )
  find_library( IOKIT_FRAMEWORK IOKit )
  find_library( FF_FRAMEWORK ForceFeedback )
  target_link_libraries( ${executable} ${FOUNDATION_FRAMEWORK} ${APPKIT_FRAMEWORK} ${IOKIT_FRAMEWORK} ${FF_FRAMEWORK} )
endif ( APPLE )
endfunction( add_es )

set ( LIBZMQ libzmq )
file(GLOB LIBZMQ ${CMAKE_INSTALL_PREFIX}/lib/libzmq-*)
set ( WINDOWS_LIBS imm32.lib winmm.lib version.lib )

# BSP -----------------

list( APPEND BSP_SOURCES main.cpp game_main.cpp render_main.cpp bsp_src/game.cpp bsp_src/render.cpp )
add_executable( bsp ${BSP_SOURCES} )

set_property( TARGET bsp PROPERTY INCLUDE_DIRECTORIES ${BSP_INCLUDES} )
add_es( bsp )

# HEAD ----------------

list( APPEND HEAD_SOURCES main.cpp game_main.cpp render_main.cpp head_src/game.cpp head_src/render.cpp )
add_executable( head ${HEAD_SOURCES} )

set_property( TARGET head PROPERTY INCLUDE_DIRECTORIES ${HEAD_INCLUDES} )
add_es( head )

# SCENE ---------------

list( APPEND SCENE_SOURCES main.cpp game_main.cpp render_main.cpp scene_load_src/game.cpp scene_load_src/render.cpp )
add_executable( scene ${SCENE_SOURCES} )

set_property( TARGET scene PROPERTY INCLUDE_DIRECTORIES ${SCENE_INCLUDES} )
add_es( scene )

# TEMPLATE ------------

list( APPEND TEMPLATE_SOURCES main.cpp game_main.cpp render_main.cpp template_src/game.cpp template_src/render.cpp )
add_executable( template ${TEMPLATE_SOURCES} )

set_property( TARGET template PROPERTY INCLUDE_DIRECTORIES ${TEMPLATE_INCLUDES} )
add_es( template )

# Install targets -----

install (TARGETS bsp head scene template DESTINATION bin)

add_custom_command(TARGET bsp head scene template POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_INSTALL_PREFIX}/bin/$<CONFIGURATION>"
        ${CMAKE_INSTALL_PREFIX}/bin)
        
add_custom_command(TARGET bsp head scene template POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    ${CMAKE_INSTALL_PREFIX}/bin/plugins.cfg)

add_custom_command(TARGET bsp head scene template POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
     ${CMAKE_INSTALL_PREFIX}/bin/media)

add_custom_command(TARGET bsp head scene template POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
     ${CMAKE_SOURCE_DIR}/binaries/media
     ${CMAKE_INSTALL_PREFIX}/bin/media)
