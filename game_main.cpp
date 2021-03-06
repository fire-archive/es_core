/*
Copyright (c) 2013, Timothee Besset
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Timothee Besset nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Timothee Besset BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "nn.hpp"
#include "nanomsg/bus.h"
#include "nanomsg/pubsub.h"
#include "nanomsg/pipeline.h"

#include "SDL.h"
#include "SDL_thread.h"

#include "OgreVector2.h"
#include "OgreVector3.h"

// includes common to all projects
#include "game_main.h"

// per project includes:
// those headers are in each project subdirectory
#include "shared_render_state.h"
#include "game.h"

int game_thread( void * _parms ) {
  auto parms = (GameThreadParms*)_parms;
  GameThreadSockets gsockets;

  GameState gs;
  SharedRenderState srs;

  nn::socket nn_control_socket( AF_SP, NN_BUS );
  gsockets.nn_control_socket = &nn_control_socket;
  {
    auto ret = gsockets.nn_control_socket->connect( "tcp://127.0.0.1:60206" ); // control_game
    assert( ret == 0 );
  }

  nn::socket nn_render_socket( AF_SP, NN_BUS );
  gsockets.nn_render_socket = &nn_render_socket;
  gsockets.nn_render_socket->bind( "tcp://127.0.0.1:60210" ); // game_render

  nn::socket nn_input_mouse_sub( AF_SP, NN_SUB );  
  nn_input_mouse_sub.setsockopt ( NN_SUB, NN_SUB_SUBSCRIBE, "input.mouse:", 1 );
  gsockets.nn_input_mouse_sub = &nn_input_mouse_sub;
  {
    auto ret = gsockets.nn_input_mouse_sub->connect( "tcp://127.0.0.1:60208" ); // input
    assert ( ret == 0 );
  }

  nn::socket nn_input_kb_sub( AF_SP, NN_SUB );
  nn_input_kb_sub.setsockopt ( NN_SUB, NN_SUB_SUBSCRIBE, "input.kb:", 1 );
  gsockets.nn_input_kb_sub = &nn_input_kb_sub;
  {
    auto ret = gsockets.nn_input_kb_sub->connect( "tcp://127.0.0.1:60208" ); // input
    assert ( ret == 0 );
  }

  nn::socket nn_input_push( AF_SP, NN_PUSH );
  gsockets.nn_input_push = &nn_input_push;
  {
    auto ret = gsockets.nn_input_push->connect( "tcp://127.0.0.1:60209" ); // input_pull
    assert ( ret == 0 );
  }

  game_init( gsockets, gs, srs );

  auto baseline = SDL_GetTicks();
  unsigned int framenum = 0;
  while ( true ) {
    auto now = SDL_GetTicks();
    auto target_frame = ( now - baseline ) / GAME_DELAY;
    if ( framenum <= target_frame ) {
      framenum++;
      // NOTE: build the state of the world at t = framenum * GAME_DELAY,
      // under normal conditions that's a time in the future
      // (the exception to that is if we are catching up on ticking game frames)
      game_tick( gsockets, gs, srs, now );
      // notify the render thread that a new game state is ready.
      // on the next render frame, it will start interpolating between the previous state and this new one

      emit_render_state( gsockets.nn_render_socket, baseline + framenum * GAME_DELAY, srs );
    } else {
      int ahead = framenum * GAME_DELAY - ( now - baseline );
      assert( ahead > 0 );
      printf( "game sleep %d ms\n", ahead );
      SDL_Delay( ahead );
    }
    char * cmd = nullptr;
    gsockets.nn_control_socket->nstr_recv(&cmd, NN_DONTWAIT);

    if ( cmd != nullptr ) {
      assert( strcmp( cmd, "stop" ) == 0 );
      nn_freemsg( cmd );
      break;
    }
  }
  return 0;
}
