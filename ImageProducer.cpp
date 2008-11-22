//
// ImageProducer.cpp
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


#include "ImageProducer.h"
#include "Movie.h"
#include <QImage>
#include <sstream>
#include <iomanip>
#include <stdexcept>


namespace annot8 {


ImageProducer* ImageProducer::_instance = 0;
int ImageProducer::_instanceCounter = 0;
QMutex ImageProducer::_instanceMutex;


ImageProducer* ImageProducer::acquireInstance()
{
    _instanceMutex.lock();
    if (!_instance) {
        _instance = new ImageProducer();
        _instance->start();
    }
    _instanceCounter++;
    _instanceMutex.unlock();
    return _instance;
}


void ImageProducer::releaseInstance()
{
    _instanceMutex.lock();
    _instanceCounter--;
    if (!_instanceCounter) {
        _instance->_active = false;
        _instance->_sema.release();
        _instance->wait();
        delete _instance;
        _instance = 0;
    }
    _instanceMutex.unlock();
}


ImageProducer::ImageProducer() :
    _active(true)
{
}


ImageProducer::~ImageProducer()
{
    // Delete possibly remaining jobs.
    _jobsMutex.lock();
    while (!_jobs.empty())
        delete _jobs.dequeue();
    _jobsMutex.unlock();
}


void ImageProducer::run()
{
    try {

        while (_active) {
            // Wait until there's work to do...
            _sema.acquire();

            // Upon deletion of the thread we may have woken up without any work
            // being left, so we must check the _active flag once again.
            if (!_active)
                break;

            // Dequeue
            _jobsMutex.lock();
            JobInfo *ji = _jobs.dequeue();
            _jobsMutex.unlock();

            ji->mutex->lock();
            {
                // Construct the image's filename.
                std::ostringstream os(
                        std::ios_base::ate | std::ios_base::app |
                        std::ios_base::out
                );
                os << ji->movie->prefix
                   << std::setfill('0') << std::setw(ji->movie->numDigits)
                   << ji->frame
                   << ji->movie->suffix;
                // Load and check the image.
                QImage *newImage = new QImage(QString::fromStdString(os.str()));
                if (!newImage) {
                    throw std::runtime_error("Not enough memory while loading "
                            "'" + os.str() + "'!");
                }
                if (newImage->isNull()) {
                    delete newImage;
                    newImage = 0;
                    throw std::runtime_error("Couldn't load '" + os.str() + "'!");
                }
                // Store the image.
                ji->images->insert(ji->frame, newImage);
                ji->imagesFifo->enqueue(ji->frame);
            }
            ji->mutex->unlock();

            // Get rid of the job's remains.
            delete ji;
        }

    } catch (std::exception &ex) {
        emit loadingFailed(QString::fromStdString(ex.what()));
    }
}


void ImageProducer::retrieve(const Movie *movie,
                             int frame,
                             QMutex *mutex,
                             QHash<int, QImage *> *images,
                             QQueue<int> *imagesFifo)
{
    // Create a new job.
    JobInfo *ji = new JobInfo;
    ji->movie = movie;
    ji->frame = frame;
    ji->mutex = mutex;
    ji->images = images;
    ji->imagesFifo = imagesFifo;

    // Then enqueue it.
    _jobsMutex.lock();
    _jobs.enqueue(ji);
    _jobsMutex.unlock();

    // And commence the work.
    _sema.release();
}


} // namespace annot8
