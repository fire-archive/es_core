cmake_minimum_required( VERSION 2.8 )
cmake_policy( SET CMP0007 NEW ) # this one it actually complained

# NOTE: we assume that CMAKE_INSTALL_PREFIX is provided, and that all the dependencies are installed there already

project( es_core )

find_package( OGRE REQUIRED )

# FIXME: replace with a proper SDL2 cmake package, if/when there is such a thing
# or piggyback on the sdl2 pkg-config (PKG_CONFIG_PATH and all that)
# --cflags: -D_THREAD_SAFE -I/Users/timo/es_core/run/include/SDL2 
# --libs: -L/Users/timo/es_core/run/lib -lSDL2
include_directories( "${CMAKE_INSTALL_PREFIX}/include/SDL2" )
link_directories( "${CMAKE_INSTALL_PREFIX}/lib" )

include_directories( ${OGRE_INCLUDE_DIRS} )

include_directories( "${CMAKE_INSTALL_PREFIX}/include" )

# NOTE: under the assumption that all the es_core dependencies are getting installed to the same prefix,
# the above has defined all the header and library directories needed for ZeroMQ

IF( APPLE )
SET( ES_CORE_OSX_WRAP OSX_wrap.mm )
include_directories( "${CMAKE_INSTALL_PREFIX}/include/OGRE/RenderSystems/GL/OSX" )
include_directories( "${CMAKE_INSTALL_PREFIX}/include/OGRE/RenderSystems/GL" )
ENDIF( APPLE )

function( add_es_framework executable)
# FIXME: APPLE
if ( APPLE )
  find_library( FOUNDATION_FRAMEWORK Foundation )
  find_library( APPKIT_FRAMEWORK AppKit )
  target_link_libraries( ${executable} ${FOUNDATION_FRAMEWORK} ${APPKIT_FRAMEWORK} )
endif ( APPLE )
endfunction( add_es_framework )

set ( LIBZMQ libzmq-v110-mt-3_3_0 )
set ( WINDOWS_LIBS imm32.lib winmm.lib version.lib )

# BSP -----------------

add_executable( bsp main.cpp game_main.cpp render_main.cpp bsp_src/game.cpp bsp_src/render.cpp ${ES_CORE_OSX_WRAP} )
get_target_property( BSP_INCLUDES bsp INCLUDE_DIRECTORIES )
# FIXME: fix ogre cmake so it sets this up properly instead of digging it up ourselves
#pkg_message( BSP "BSP: ${OGRE_Plugin_BSPSceneManager_INCLUDE_DIR}" )
list( APPEND BSP_INCLUDES "${CMAKE_INSTALL_PREFIX}/include/OGRE/Plugins/BSPSceneManager" )
list( APPEND BSP_INCLUDES "${CMAKE_SOURCE_DIR}/bsp_src" )
set_property( TARGET bsp PROPERTY INCLUDE_DIRECTORIES ${BSP_INCLUDES} )
target_link_libraries( bsp czmq ${LIBZMQ} )
target_link_libraries( bsp SDL2 )
target_link_libraries( bsp ${OGRE_LIBRARIES} )
target_link_libraries( bsp ${WINDOWS_LIBS} )
add_es_framework( bsp )

# HEAD ----------------

add_executable( head main.cpp game_main.cpp render_main.cpp head_src/game.cpp head_src/render.cpp ${ES_CORE_OSX_WRAP} )
get_target_property( HEAD_INCLUDES head INCLUDE_DIRECTORIES )
list( APPEND HEAD_INCLUDES "${CMAKE_SOURCE_DIR}/head_src" )
set_property( TARGET head PROPERTY INCLUDE_DIRECTORIES ${HEAD_INCLUDES} )
target_link_libraries( head czmq ${LIBZMQ} )
target_link_libraries( head SDL2 )
target_link_libraries( head ${OGRE_LIBRARIES} )
target_link_libraries( head ${WINDOWS_LIBS} )
add_es_framework( head )

# SCENE ---------------

add_executable( scene main.cpp game_main.cpp render_main.cpp scene_load_src/game.cpp scene_load_src/render.cpp ${ES_CORE_OSX_WRAP} )
get_target_property( SCENE_INCLUDES scene INCLUDE_DIRECTORIES )
list( APPEND SCENE_INCLUDES "${CMAKE_SOURCE_DIR}/scene_load_src" )
set_property( TARGET scene PROPERTY INCLUDE_DIRECTORIES ${SCENE_INCLUDES} )
target_link_libraries( scene czmq ${LIBZMQ} )
target_link_libraries( scene SDL2 )
target_link_libraries( scene ${OGRE_LIBRARIES} )
target_link_libraries( scene ${WINDOWS_LIBS} )
add_es_framework( scene )

# TEMPLATE ------------

add_executable( template main.cpp game_main.cpp render_main.cpp template_src/game.cpp template_src/render.cpp ${ES_CORE_OSX_WRAP} )
get_target_property( TEMPLATE_INCLUDES template INCLUDE_DIRECTORIES )
list( APPEND TEMPLATE_INCLUDES "${CMAKE_SOURCE_DIR}/template_src" )
set_property( TARGET template PROPERTY INCLUDE_DIRECTORIES ${TEMPLATE_INCLUDES} )
target_link_libraries( template czmq ${LIBZMQ} )
target_link_libraries( template SDL2 )
target_link_libraries( template ${OGRE_LIBRARIES} )
target_link_libraries( template ${WINDOWS_LIBS} )
add_es_framework( template )

# Install targets -----

install (TARGETS bsp head scene template DESTINATION bin)
