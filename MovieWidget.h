//
// MovieWidget.h
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


#ifndef __MOVIEWIDGET_H__
#define __MOVIEWIDGET_H__


#include "Movie.h"
#include <QWidget>
#include <QMutex>
#include <QHash>
#include <QQueue>
#include <QImage>


namespace hiwi
{


// Forward declaration
class ImageProducer;


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


    /**
     * Get the movie's current position in milliseconds.
     */
    inline int getTimestamp() const
    {
        return (int)(_position * _numFrames * _movie->msPerFrame);
    }


    /**
     * Get the movie's total number of frames.
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
    float               _position;

    ImageProducer       *_imageProducer;
    QMutex              _mutex;
    QHash<int, QImage*> _images;
    QQueue<int>         _imagesFifo;
};


} // namespace hiwi

#endif
