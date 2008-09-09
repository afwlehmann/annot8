//
// $Id$
//


#ifndef __SOUND_H__
#define __SOUND_H__


// Get rid of SDL's "main" macro if it hasn't been defined beforehand.
#ifdef main
#  include <SDL/SDL_stdinc.h>
#else
#  include <SDL/SDL_stdinc.h>
#  undef main
#endif


// Forward declaration
struct SDL_AudioSpec;


namespace hiwi {

namespace audio {


/**
 * Convenience class for the playback of 16-bit mono sounds.
 */
class Sound
{
public:
    typedef enum { Play, Pause } PlaybackState;


    // SDL supports only a limited range of sample frequencies for playback,
    // hence:
    static const unsigned int MinSampleFreq = 11025;
    static const unsigned int MaxSampleFreq = 44100;


    /**
     * Constructs a new instance of Sound for the given samples (in floating
     * point format) and sample frequency.
     * Note that the Sound object doesn't take ownership of the given pointer.
     * @param  samples              a pointer to the raw samples
     * @param  len                  length of the the samples buffer
     * @param  sampleFreq           the desired playback frequency
     */
    Sound(const double *samples, size_t len, unsigned int sampleFreq);


    /**
     * Destructs an instance of Sound and frees all formerly allocated memory.
     */
    virtual ~Sound();


    /**
     * Sets the playback state of the Sound object.
     * @param  state                a PlayState
     */
    void setPlaybackState(PlaybackState state);


    /**
     * Returns the playback state of the Sound object.
     */
    inline PlaybackState playbackState() const { return _state; }


    /**
     * Sets the playback position.
     * @param  pos                  the new pos [0,1]
     */
    void setPlaybackPos(double pos);


    /**
     * Returns the current playback position.
     * @return                      a value within [0,1]
     */
    double playbackPos() const;


private:
    // Forbid copy constructor and operator=.
    Sound(const Sound &);
    Sound& operator=(const Sound &);


    /**
     * Callback function for SDL.
     */
    static void pbCallback(void *user, Uint8 *buf, int size);


    short              *_samples;
    const size_t       _len;
    const unsigned int _sampleFreq;
    size_t             _pos;
    PlaybackState      _state;
    SDL_AudioSpec      *_audioSpec;
};


} // namespace audio

} // namespace hiwi


#endif
