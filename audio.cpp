//
// audio.cpp
// $Id$
//
// Copyright 2008 by Alexander Lehmann <lehmanna@in.tum.de>
//
// This file is part of annot8.
// 
// annot8 is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
// 
// annot8 is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// annot8.  If not, see <http://www.gnu.org/licenses/>.
//


#include "audio.h"
#include <SDL/SDL.h>
#include <SDL/SDL_sound.h>
#include <cstdlib>


namespace annot8 {

namespace audio {


static bool _sdlInitialized = false;
static bool _exitHookRegistered = false;


bool initialize()
{
    if (_sdlInitialized)
        return true;

    do {
        // First initialize the mothership.
        bool audioInitDone =
            ((SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) == SDL_INIT_AUDIO);
        if (!audioInitDone && SDL_Init(SDL_INIT_AUDIO) != 0)
            break;
        // Then its allied forces.
        if (!Sound_Init()) {
            SDL_Quit();
            break;
        }
        // We register shutdown() via atexit, so that even if the user forgets
        // about calling shutdown() explicitly, we still perform a nice
        // cleanup.
        if (!_exitHookRegistered) {
            _exitHookRegistered = (atexit(shutdown) == 0) ? 1 : 0;
        }
        // Done.
        _sdlInitialized = true;
        return true;
    } while (false);

    // Once we get to this point, an error has occured.
    return false;
}


void shutdown()
{
    if (!_sdlInitialized)
        return;
    // Shutdown happens in reverse order.
    Sound_Quit();
    SDL_Quit();
    _sdlInitialized = false;
}


bool isInitialized() {
    return _sdlInitialized;
}


} // namespace audio

} // namespace annot8

