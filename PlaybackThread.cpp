//
// $Id$
//


#include "PlaybackThread.h"
#include "SamplesPreviewCanvas.h"


using namespace hiwi::audio;
using namespace hiwi::internal;


namespace hiwi {


PlaybackThread::PlaybackThread(const double *samples, size_t nSamples,
                               unsigned int sampleFreq, QObject *parent) :
    QThread(parent)
{
    _sound = new Sound(samples, nSamples, sampleFreq);
};


PlaybackThread::~PlaybackThread()
{
    _mutex.lock();
    if (_sound) {
        _sound->setPlaybackState(Sound::Pause);
        delete _sound;
        _sound = NULL;
    }
    _mutex.unlock();
}


void PlaybackThread::run()
{
    while (true) {
        _mutex.lock();
        if (!_sound ||
            _sound->playbackPos() >= 1 ||
            _sound->playbackState() != Sound::Play) {
            _mutex.unlock();
            break;
        }
        emit playbackPosChanged(_sound->playbackPos());
        _mutex.unlock();
        // 100ms gives a refresh rate of 10Hz.
        msleep(100);
    }
    // Qt emits the QThread::finished() signal right after run() has terminated.
}


void PlaybackThread::setPlaybackState(Sound::PlaybackState state)
{
    _mutex.lock();
    _sound->setPlaybackState(state);
    _mutex.unlock();
    // Start the thread if the state is Sound::Play and the thread isn't
    // running already.
    if (state == Sound::Play && !isRunning())
        start();
}


void PlaybackThread::setPlaybackPos(float pos)
{
    _mutex.lock();
    _sound->setPlaybackPos(pos);
    emit playbackPosChanged(_sound->playbackPos());
    _mutex.unlock();
}


} // namespace hiwi
