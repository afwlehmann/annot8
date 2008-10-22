//
// ImageProducer.h
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


#ifndef __IMAGEPRODUCER_H__
#define __IMAGEPRODUCER_H__


#include <QThread>
#include <QSemaphore>
#include <QMutex>
#include <QHash>
#include <QQueue>


// Forward declaration
class QImage;


namespace hiwi {


// Forward declaration
struct Movie;


class ImageProducer : public QThread
{
    Q_OBJECT

public:
    /**
     * Acquires the instance of the image producer, thus increasing the
     * internal counter. Creates the instance if neccessary.
     */
    static ImageProducer* acquireInstance();


    /**
     * Releases the instance of the image producer, i.e. decrements the
     * internal counter. Once this counter reaches zero, the instance will be
     * deleted.
     */
    static void releaseInstance();


    virtual ~ImageProducer();


    /**
     * Creates a new job for the given movie and requested frame. A pointer to
     * the frame will be stored in the given images hash while the critical
     * section is protected by using the given mutex.
     * @param  movie            a pointer to a Movie
     * @param  frame            the number of the frame that should be retrieved
     * @param  images           a pointer to the images hash
     * @param  imageFifo        a pointer to the images fifo
     */
    void retrieve(const Movie *movie,
                  int frame,
                  QMutex *mutex,
                  QHash<int, QImage *> *images,
                  QQueue<int> *imageFifo);


protected:
    /**
     * The thread's main loop.
     */
    virtual void run();


private:
    // Singleton stuff...
    ImageProducer();
    ImageProducer(const ImageProducer &);
    ImageProducer & operator= (const ImageProducer &);


    static ImageProducer *_instance;
    static QMutex        _instanceMutex;
    static int           _instanceCounter;


    struct JobInfo {
        const Movie          *movie;
        int                  frame;
        QMutex               *mutex;
        QHash<int, QImage *> *images;
        QQueue<int>          *imagesFifo;
    };


    QSemaphore        _sema;
    QMutex            _jobsMutex;
    QQueue<JobInfo *> _jobs;
    bool              _active;
};


} // namespace hiwi


#endif
