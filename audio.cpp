//
// $Id$
//


#include "audio.h"
#include <SDL/SDL.h>
#include <SDL/SDL_sound.h>
#include <cstdlib>


namespace hiwi {

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

} // namespace hiwi

