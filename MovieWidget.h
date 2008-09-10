//
// MovieWidget.h
// $Id$
//


#ifndef __MOVIEWIDGET_H__
#define __MOVIEWIDGET_H__


#include "Movie.h"
#include <QWidget>
#include <QHash>
#include <QQueue>
#include <QImage>


namespace hiwi
{


class MovieWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a MovieWidget for the given movie.
     */
    MovieWidget(Movie *movie);


    /**
     * Destructs an instance of MovieWidget.
     */
    virtual ~MovieWidget();


    /**
     * Get the movie's description.
     */
    inline std::string getDescription() const { return _movie->description; }


    /**
     * Get the movie's current position.
     */
    inline float getPosition() const { return _position; }


    inline int getFirstFrame() const { return _movie->firstFrame; }


    /**
     * Get the movie's number of frames.
     */
    inline int getNumFrames() const { return _numFrames; }


    /**
     * Initializes this widget in terms of prefetching the first frames.
     */
    void initialize();


public slots:
    /**
     * Sets the associated movie's position to the given value.
     * @param  position     a real value within [0,1].
     */
    void setPosition(float position);


protected:
    /**
     * Event-handler for paint-events.
     */
    virtual void paintEvent(QPaintEvent *);


    /**
     * Loads the frame specified by the given position within [0,1]. Does some
     * prefetching, too.
     * @param  pos          the position of the frame
     */
    void loadFrame(float pos);


private:
    const Movie *       _movie;
    const int           _numFrames;

    QHash<int, QImage*> _images;
    QQueue<int>         _imagesFifo;

    float               _position;

    bool                _initialized;
};


} // namespace hiwi

#endif
