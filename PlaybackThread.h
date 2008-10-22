//
// PlaybackThread.h
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


#ifndef __PLAYBACKTHREAD_H__
#define __PLAYBACKTHREAD_H__


#include "Samples.h"
#include <QThread>
#include <QMutex>


namespace hiwi {

namespace audio {


class PlaybackThread : public QThread
{
    Q_OBJECT

public:
    typedef enum { Play, Pause } PlaybackState;
    enum { DefaultFrequency = 22050 };


    /**
     * Constructs an instance of PlaybackThread for the given samples and opens
     * the audio device.
     * @param  samples          a pointer to a Samples object
     * @param  parent           a pointer to the parent object
     * @throw                   audio::AudioException
     */
    PlaybackThread(Samples *samples, QObject *parent = 0);


    /**
     * Destructs an instance of PlaybackThread, thus stopping the playback.
     * Closes the audio device.
     */
    virtual ~PlaybackThread();


    /**
     * Sets the playback state of this thread to the given state.
     * @param  state            the new playback state
     */
    void setPlaybackState(PlaybackState state);


    /**
     * Returns the playback state of this thread.
     */
    inline PlaybackState playbackState() const;


    /**
     * Returns the playback position of this thread.
     */
    float playbackPos() const;


    /**
     * Sets the playback position to the given value.
     * @param  pos              a real value within [0,1]
     */
    void setPlaybackPos(float pos);


signals:
    /**
     * This signal is emitted every time the playback position changed.
     * During playback this happens at a rate of about 8Hz at best.
     */
    void playbackPosChanged(float pos);


protected:
    /**
     * The thread's main loop.
     */
    virtual void run();


private:
    // Forbid copy constructor and operator=.
    PlaybackThread(const PlaybackThread &);
    PlaybackThread& operator=(const PlaybackThread &);


    /**
     * Callback function for SDL.
     */
    static void pbCallback(void *user, Uint8 *buf, int size);


    Samples         *_samples;
    // The following mutex is responsible for all the succeeding variables:
    QMutex          _mutex;
    SDL_AudioSpec   *_audioSpec;
    size_t          _pos;
    PlaybackState   _state;
};


} // namespace audio

} // namespace hiwi


#endif
