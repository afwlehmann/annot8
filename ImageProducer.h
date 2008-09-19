//
// ImageProducer.h
// $Id$
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
    static ImageProducer* acquireInstance();
    static void releaseInstance();


    virtual ~ImageProducer();


    void retrieve(const Movie *movie,
                  int frame,
                  QMutex *mutex,
                  QHash<int, QImage *> *images,
                  QQueue<int> *imageFifo);


protected:
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
