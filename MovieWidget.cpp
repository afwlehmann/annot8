//
// MovieWidget.cpp
// $Id$
//


#include "MovieWidget.h"
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
    _initialized(false)
{
    // A small sanity check...
    assert(_movie->firstFrame >= 0);
    assert(_movie->firstFrame < _movie->lastFrame);
    assert(_movie->numDigits > 0);
}


MovieWidget::~MovieWidget()
{
    // Let go of all allocated images.
    foreach (QImage *image, _images)
        delete image;
    _images.clear();
    _imagesFifo.clear();

    // Don't forget the movie itself.
    delete _movie;
    _movie = 0;
}


void MovieWidget::loadFrame(float pos)
{
    const int startIndex = (int)(_movie->firstFrame + (_numFrames - 1) * pos);
    const int endIndex = qMin(startIndex + MAX_IMAGES_FIFO_SIZE - 1,
                              _movie->lastFrame);

    assert(startIndex >= _movie->firstFrame && startIndex <= _movie->lastFrame);

    std::stringstream ss(std::ios_base::app | std::ios_base::ate | std::ios_base::out);
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

            // Now fetch the desired frame.
            ss.str(_movie->prefix);
            ss << std::setfill('0') << std::setw(_movie->numDigits) << i
               << _movie->suffix;
            QImage *newImage = new QImage(QString::fromStdString(ss.str()));
            if (newImage->isNull()) {
                delete newImage;
                throw std::runtime_error("Couldn't load '" + ss.str() + "'.");
            }

            // Store the pointer to the image and enqueue the image's index in
            // the fifo.
            _images[i] = newImage;
            _imagesFifo.enqueue(i);
        }
    }
}


void MovieWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    const int index = _movie->firstFrame + (_numFrames - 1) * _position;
    if (_images.contains(index)) {
        p.drawImage(rect(), *_images[index]);
    } else {
        p.fillRect(rect(), QBrush(Qt::black, Qt::DiagCrossPattern));
    }
}


void MovieWidget::setPosition(float position)
{
    assert(position >= 0 && position <= 1);
    _position = position;
    loadFrame(_position);
    update();
}


void MovieWidget::initialize()
{
    loadFrame(0);
}


} // namespace hiwi
