//
// MovieWidget.cpp
// $Id$
//


#include "MovieWidget.h"
#include "ImageProducer.h"
#include <QPainter>
#include <cassert>
#include <sstream>
#include <iomanip>
#include <stdexcept>


#define MAX_IMAGES_FIFO_SIZE    10


namespace hiwi
{


MovieWidget::MovieWidget(Movie *movie) :
    _movie(movie),
    _numFrames(_movie->lastFrame - _movie->firstFrame + 1),
    _position(0.0),
    _imageProducer(0)
{
    // A small sanity check...
    assert(_movie->firstFrame >= 0);
    assert(_movie->firstFrame < _movie->lastFrame);
    assert(_movie->numDigits > 0);

    // Acquire access on the image producer thread.
    _imageProducer = ImageProducer::acquireInstance();
}


MovieWidget::~MovieWidget()
{
    // Let go of all allocated images.
    foreach (QImage *image, _images)
        delete image;
    _images.clear();
    _imagesFifo.clear();

    // The movie ptr ist owned by the MainWindow, so all we do is set
    // _movie to 0.
    _movie = 0;

    // We're also done with the ImageProducer thread.
    ImageProducer::releaseInstance();
}


void MovieWidget::loadFrame(float pos)
{
    const int startIndex = (int)(_movie->firstFrame + (_numFrames - 1) * pos);
    const int endIndex = qMin(startIndex + MAX_IMAGES_FIFO_SIZE - 1,
                              _movie->lastFrame);

#ifdef DEBUG
    assert(startIndex >= _movie->firstFrame && startIndex <= _movie->lastFrame);
#endif

    _mutex.lock();
    for (int i = startIndex; i <= endIndex; i++) {
        // Load the frame only when neccessary
        if (!_images.contains(i)) {
            // See if there's still enough space according to the fifo.
            if (_imagesFifo.size() >= MAX_IMAGES_FIFO_SIZE) {
                // No, there isn't enough space. Remove the `oldest' frame.
                // However, if the dequeued index lies within the range
                // [startIndex, endIndex], give it a second chance.
                int oldestIndex = _imagesFifo.dequeue();
                while (oldestIndex >= startIndex && oldestIndex <= endIndex) {
                    _imagesFifo.enqueue(oldestIndex);
                    oldestIndex = _imagesFifo.dequeue();
                }
                delete _images.take(oldestIndex);
            }
            // Now delegate the retrieval of the desired frame.
            _imageProducer->retrieve(_movie, i, &_mutex, &_images, &_imagesFifo);
        }
    }
    _mutex.unlock();
}


void MovieWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    const int index = _movie->firstFrame + (_numFrames - 1) * _position;
    _mutex.lock();
    QImage *im = _images.value(index);
    _mutex.unlock();

    if (im) {
        p.drawImage(rect(), *_images[index]);
    } else {
        p.fillRect(rect(), QBrush(Qt::black, Qt::DiagCrossPattern));
    }
}


void MovieWidget::setPosition(float position)
{
#ifdef DEBUG
    assert(position >= 0 && position <= 1);
#endif

    _position = position;
    loadFrame(_position);
    update();
}


void MovieWidget::initialize()
{
    loadFrame(0);
}


} // namespace hiwi
