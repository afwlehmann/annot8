//
// $Id$
//


#ifndef __PLAYBACKTHREAD_H__
#define __PLAYBACKTHREAD_H__


#include "Sound.h"
#include <QThread>
#include <QMutex>


namespace hiwi {


class PlaybackThread : public QThread
{
    Q_OBJECT

public:
    /**
     * Constructs an instance of PlaybackThread for the given samples and
     * sample frequency.
     * @param  samples          a pointer to the samples
     * @param  nSamples         the total number of samples
     * @param  sampleFreq       the sample frequency
     * @param  previewCanvas    a pointer to a SamplesPreviewCanvas to be able
     *                          to update the position marker
     * @param  parent           a pointer to the parent object
     */
    PlaybackThread(const double *samples, size_t nSamples,
                   unsigned int sampleFreq, QObject *parent = 0);


    /**
     * Destructs an instance of PlaybackThread, thus stopping the playback
     * and freeing all allocated memory.
     */
    virtual ~PlaybackThread();


    /**
     * Sets the playback state of this thread to the given state.
     * @param  state            the new playback state
     */
    void setPlaybackState(audio::Sound::PlaybackState state);


    /**
     * Returns the playback state of this thread.
     */
    inline audio::Sound::PlaybackState playbackState() const;


    /**
     * Sets the playback position to the given value.
     * @param  pos              a real value within [0,1]
     */
    void setPlaybackPos(float pos);


    /**
     * Returns the playback position of this thread.
     */
    inline float playbackPos() const;


signals:
    /**
     * This signal is emitted every 100ms at best.
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


    audio::Sound                   *_sound;
    QMutex                         _mutex;
};


// Inlines
audio::Sound::PlaybackState PlaybackThread::playbackState() const
{
    return _sound->playbackState();
}


float PlaybackThread::playbackPos() const
{
    return _sound->playbackPos();
}


} // namespace hiwi


#endif
