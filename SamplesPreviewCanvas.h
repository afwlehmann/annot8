//
// $Id$
//


#ifndef __SAMPLESPREVIEWCANVAS_H__
#define __SAMPLESPREVIEWCANVAS_H__


#include <QWidget>


namespace hiwi {

namespace internal {


class SamplesPreviewCanvas : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a new SamplesPreviewCanvas.
     */
    SamplesPreviewCanvas(QWidget *parent = 0);


    /**
     * Constructs a new SamplesPreviewWidget for the given samples.
     * @param  samples                  a pointer to the samples
     * @param  nSamples                 the number of samples
     * @param  parent                   a pointer to the parent widget
     */
    SamplesPreviewCanvas(const double *samples, size_t nSamples,
                                     QWidget *parent);


    /**
     * Destructs an instance of SamplesPreviewCanvas and frees all formerly
     * allocated memory.
     */
    virtual ~SamplesPreviewCanvas();


    /**
     * Sets the samples that should be displayed. Makes a local copy of the
     * given samples.
     * @param  samples                  a pointer to the samples
     * @param  nSamples                 the number of samples
     */
    void setSamples(const double *samples, size_t nSamples);


    /**
     * Returns the minimum of the displayed area mapped to [0,1].
     */
    inline float min() const { return _tMin; }


    /**
     * Returns the maximum of the displayed area mapped to [0,1].
     */
    inline float max() const { return _tMax; }


    /**
     * Returns whether the marker is currently visible or not.
     */
    inline bool markerVisible() const { return _markerVisible; }


    /**
     * Returns the number of samples.
     */
    inline size_t nrOfSamples() const { return _nSamples; }


signals:
    /**
     * This signal is emitted every time the user clicks somewhere in the
     * image. The position is mapped to [0,1].
     */
    void posClicked(float pos);


public slots:
    /**
     * Sets the minimum of the displayed area mapped to [0,1].
     * @param  newMin                   a real value within [0,1]
     */
    void setMin(float newMin);


    /**
     * Sets the maximum of the displayed area mapped to [0,1].
     * @param  newMax                   a real value within [0,1]
     */
    void setMax(float newMax);


    /**
     * Sets the minimum and maximum of the displayed area mapped to [0,1].
     * @param  newMin                   a real value within [0,1]
     * @param  newMax                   a real value within [0,1]
     */
    void setMinMax(float newMin, float newMax);


    /**
     * Sets the new marker position.
     * @param  newPos                   a real value within [0,1]
     */
    void setMarkerPos(float newPos);


    /**
     * Changes the visibility of the marker.
     */
    void setMarkerVisible(bool visible);

    
protected:
    /**
     * Returns a size hint for the layout.
     */
    virtual QSize sizeHint() const;


    /**
     * Event-handler for paint events.
     */
    virtual void paintEvent(QPaintEvent *ev);


    /**
     * Event-handler for mouse-release events.
     */
    virtual void mouseReleaseEvent(QMouseEvent *ev);


    /**
     * Repaints the marker region if neccessary.
     */
    void updateMarker();


private:
    // Forbid copy constructor and operator=.
    SamplesPreviewCanvas(const SamplesPreviewCanvas&);
    SamplesPreviewCanvas& operator=(const SamplesPreviewCanvas&);


    double       *_samples;
    size_t       _nSamples;
    float        _tMin;
    float        _tMax;
    float        _markerPos;
    bool         _markerVisible;
};


} // namespace internal

} // namespace hiwi


#endif

