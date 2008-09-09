//
// $Id$
//


#ifndef __SAMPLESPREVIEWWIDGET_H__
#define __SAMPLESPREVIEWWIDGET_H__


#include <QWidget>


// Forward declarations
class QPushButton;
class QScrollBar;


namespace hiwi {


// Forward declarations
class PlaybackThread;
namespace audio { class Sound; }
namespace internal { class SamplesPreviewCanvas; }


class SamplesPreviewWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructs a new SamplesPreviewWidget.
     */
    SamplesPreviewWidget(QWidget *parent = 0);


    /**
     * Constructs a new SamplesPreviewWidget for the given samples.
     * @param  samples                  a pointer to the samples
     * @param  nSamples                 the number of samples
     * @param  sampleFreq               the sample frequency
     * @param  parent                   a pointer to the parent widget
     */
    SamplesPreviewWidget(const double *samples, size_t nSamples,
                         unsigned int sampleFreq, QWidget *parent = 0);



    /**
     * Destructs an instance of SamplesPreviewWidget.
     */
    virtual ~SamplesPreviewWidget();


    /**
     * Sets the samples that should be displayed. Creates a Sound object
     * if possible.
     * @param  samples                  a pointer to the samples
     * @param  nSamples                 the number of samples
     * @param  sampleFreq               the sample frequency
     * @return                          true iff a Sound object has been created
     */
    bool setSamples(const double *samples, size_t nSamples,
                    unsigned int sampleFreq);


protected slots:
    /**
     * Event-handler for the "Play" button.
     */
    void on_pbPlay_clicked();


    /**
     * Event-handler for the "Pause" button.
     */
    void on_pbPause_clicked();


    /**
     Event-handler for the "Rewind" button.
     */
    void on_pbRewind_clicked();


    /**
     * Event-handler for the "+" button.
     */
    void on_pbZoomIn_clicked();


    /**
     * Event-handler for the "-" button.
     */
    void on_pbZoomOut_clicked();


    /**
     * Event-handler for the positional scrollbar.
     */
    void on_sbPosition_valueChanged(int value);


    /**
     * Event-handler for the samples visualization canvas.
     */
    void on_svCanvas_posClicked(float pos);
    
    
    /**
     * Event-handler for the playback thread.
     */
    void on_playbackThread_playbackPosChanged(float pos);


protected:
    /**
     * Handles change-events such as QEvent::EnabledChanged.
     */
    virtual void changeEvent(QEvent *ev);


private:
    /**
     * Sets up the whole user interface, i.e. all child widgets and layouts.
     */
    void setupUi();


    /**
     * Adapts the scrollbar's range to the visualized samples.
     */
    void updateScrollBarRange();


    internal::SamplesPreviewCanvas *_svCanvas;
    QScrollBar                     *_sbPosition;
    QPushButton                    *_pbPlay;
    QPushButton                    *_pbPause;
    QPushButton                    *_pbRewind;
    PlaybackThread                 *_playbackThread;
};


} // namespace hiwi


#endif

