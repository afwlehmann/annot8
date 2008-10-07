//
// MainWindow.cpp
// $Id$
//


#include "MainWindow.h"
#include "DBController.h"
#include "MovieWidget.h"
#include "Participant.h"
#include "PlaybackThread.h"
#include "Samples.h"
#include "AboutDlg.h"
#include "Attributes.h"

#include <stdexcept>
#include <cmath>
#include <cassert>

#include <QMessageBox>
#include <QListWidgetItem>
#include <QProgressDialog>
#include <QKeyEvent>


// The zoom factor to be used for zoom-in and -out.
#define ZOOM_FACTOR 1.5f


using namespace hiwi::audio;
using namespace std;


namespace hiwi {


MainWindow::MainWindow(int participantID, bool takeAlong) :
    _participant(0),
    _pbThread(0),
    _playing(false),
    _flipping(false),
    _takeAlong(takeAlong)
{
    _ui.setupUi(this);
    _ui.twMovies->removeTab(0);

    setupMovies();
    setupParticipants(participantID);
    setupSamples();

    loadAnnotations(false);
}


MainWindow::~MainWindow()
{
    // Delete the movies.
    for (vector<Movie *>::iterator it = _movies.begin(); it != _movies.end(); ++it)
        delete *it;
    _movies.clear();

    // Delete the selected participant.
    if (_participant) {
        delete _participant;
        _participant = 0;
    }

    // Annihilate the playback thread ;-)
    if (_pbThread) {
        _pbThread->setPlaybackState(PlaybackThread::Pause);
        while (_pbThread->isRunning()) {
            // Wait for the thread to terminate.
        }
        delete _pbThread;
        _pbThread = 0;
    }
}


void MainWindow::setupMovies()
{
    // Get the available movies.
    _movies = DBController::instance()->getAvailableMovies();
    if (_movies.empty())
        throw runtime_error("There are no movies to annotate.");

    // Display a progress dialog because prefetching might take a while...
    QProgressDialog progDlg(tr("Initializing the available movies..."),
                            QString::null, 0, _movies.size());
    progDlg.setValue(0);
    progDlg.show();

    // Create one tab per movie and make sure that all movies have the exact
    // same length.
    int lengthCheck = -1;
    for (vector<Movie *>::const_iterator it = _movies.begin();
        it != _movies.end(); it++)
    {
        int length = (*it)->lastFrame - (*it)->firstFrame + 1;
        if (it == _movies.begin())
            lengthCheck = length;
        else if (length != lengthCheck)
            throw runtime_error("All movies must have equal length!");

        MovieWidget *mw = new MovieWidget(*it);
        _ui.twMovies->addTab(mw, QString::fromStdString(mw->getDescription()));
        mw->initialize();
        progDlg.setValue(progDlg.value() + 1);
    }

    // Update the range of the current-frame horizontal slider.
    _ui.hsCurrentFrame->setRange(0, lengthCheck);
    _ui.spbCurrentFrame->setRange(0, lengthCheck);
}


void MainWindow::setupParticipants(int participantID)
{
    // Get the participants.
    vector<Participant *> participants =
        DBController::instance()->getParticipants();
    if (participants.empty())
        throw runtime_error("There are no participants.");

    // Create one list widget item per participant. Also store the participant
    // that was selected for annotation while deleting the others.
    for (vector<Participant *>::const_iterator it = participants.begin();
        it != participants.end(); it++)
    {
        Participant *p = *it;
        if (p->id == participantID) {
            _participant = p;
        } else {
            QListWidgetItem *lwi = new QListWidgetItem(
                    QString("%1, %2 (%3)")
                    .arg(QString::fromStdString(p->name))
                    .arg(QString::fromStdString(p->firstName))
                    .arg(p->number)
            );
            lwi->setData(Qt::UserRole, p->id);
            lwi->setFlags(lwi->flags() | Qt::ItemIsUserCheckable);
            lwi->setCheckState(Qt::Unchecked);
            _ui.lwReceivers->addItem(lwi);
            delete p;
        }
    }
    participants.clear();

    // Assure that the _participant has been set.
    assert(_participant);

    // Update the corresponding fields of the ui.
    _ui.leName->setText(QString::fromStdString(_participant->name));
    _ui.leFirstName->setText(QString::fromStdString(_participant->firstName));
    _ui.leNumber->setText(QString::number(_participant->number));
}


void MainWindow::setupSamples()
{
    // Display a progress dialog because this might take a while...
    QProgressDialog progDlg(tr("Initializing the samples..."), QString::null,
                            0, 2);
    progDlg.setValue(0);
    progDlg.show();
    // Get the samples that are associated with the participant.
    _samples = DBController::instance()->getSamplesForParticipant(_participant);
    if (!_samples)
        throw std::runtime_error("No samples stored for the given participant.");

    progDlg.setValue(1);

    // Initialize the samples preview as well as the corresponding slider.
    _ui.spvCanvas->setSamples(_samples->samplesAsFloat, _samples->numSamples);
    _ui.spvCanvas->setMarkerVisible(true);
    _ui.hsbSamples->setRange(0, 0);

    progDlg.setValue(2);

    // Create and connect to the neccessary playback thread.
    _pbThread = new PlaybackThread(_samples);
    connect(_pbThread,
            SIGNAL(playbackPosChanged(float)),
            SLOT(xx_on_pbThread_playbackPosChanged(float)));
    connect(_pbThread,
            SIGNAL(finished()),
            SLOT(xx_on_pbThread_finished()));
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDlg(this).exec();
}


void MainWindow::on_actionQuit_triggered()
{
    // Don't argue!
    close();
}


void MainWindow::on_twMovies_currentChanged(int index)
{
    // index < 0 seemingly happens during Qt's initialization?!
    if (index < 0)
        return;

    // Assure that the newly selected movie's position gets updated as well,
    // i.e. delegate this task to the horizontal slider as (almost) everybody
    // else does.
    on_hsCurrentFrame_valueChanged(currentFrame());
}


void MainWindow::on_hsCurrentFrame_valueChanged(int value)
{
    // Since the associated signal is already emitted during Qt's `startup' we
    // must introduce a little safety check at this point:
    if (!_participant)
        return;

    // Go to the desired frame.
    goToFrame(value);

    if (!_flipping) {
        // Adapt the samples position.
        _pbThread->setPlaybackPos(positionForValue(value));
        // Update the annotations.
        loadAnnotations();
    }
}


void MainWindow::on_tbPrev_clicked()
{
    // Delegate the task to the current frame slider.
    if (currentFrame() > 0)
        _ui.hsCurrentFrame->setValue(currentFrame() - 1);
}


void MainWindow::on_tbNext_clicked()
{
    if (currentFrame() >= _ui.hsCurrentFrame->maximum())
        return;

    // Block the current-frame slider and spinbox signals in order to suppress
    // unwanted event handling.
    _ui.hsCurrentFrame->blockSignals(true);
    _ui.spbCurrentFrame->blockSignals(true);

    // Go to the desired frame.
    const int nextFrame = currentFrame() + 1;
    goToFrame(nextFrame);

    // Adapt the current-frame slider and the spinbox.
    _ui.hsCurrentFrame->setValue(nextFrame);
    _ui.spbCurrentFrame->setValue(nextFrame);

    // Adapt the samples position.
    _pbThread->setPlaybackPos(positionForValue(nextFrame));

    // Take along the annotations iff desired.
    loadAnnotations(_takeAlong);

    // Unblock the current-frame slider and spinbox signals again.
    _ui.hsCurrentFrame->blockSignals(false);
    _ui.spbCurrentFrame->blockSignals(false);
}


void MainWindow::on_pbReset_clicked()
{
    // (Re)load the corresponding annotations.
    loadAnnotations();
}


void MainWindow::on_pbSave_clicked()
{
    // First determine the ids of the checked receivers.
    vector<int> selectedReceiverIDs;
    for (int i = 0; i < _ui.lwReceivers->count(); i++) {
        QListWidgetItem *lwi = _ui.lwReceivers->item(i);
        if (lwi->checkState() == Qt::Checked)
            selectedReceiverIDs.push_back(lwi->data(Qt::UserRole).toInt());
    }

    // Then determine the attributes.
    Attributes attributes;
    attributes.speaking = _ui.cbSpeaking->isChecked();
    attributes.laughing = _ui.cbLaughing->isChecked();

    // Store those ids in conjunction with the `laughing' attribute.
    DBController::instance()->storeAnnotation(
            currentTimestamp(),
            _participant->id,
            selectedReceiverIDs,
            attributes
    );
}


void MainWindow::on_pbSaveAndContinue_clicked()
{
    // Let "The Others" do the rest ;-)
    on_pbSave_clicked();
    on_tbNext_clicked();
    on_tbSyncSamples_clicked();
}


float MainWindow::positionForValue(int value) const
{
    return (float)value / (float)_ui.hsCurrentFrame->maximum();
}


float MainWindow::currentPosition() const
{
    MovieWidget *mw = static_cast<MovieWidget *>(_ui.twMovies->currentWidget());
    return mw->getPosition();
}


int MainWindow::currentTimestamp() const
{
    MovieWidget *mw = static_cast<MovieWidget *>(_ui.twMovies->currentWidget());
    return mw->getTimestamp();
}


int MainWindow::currentFrame() const
{
    return _ui.hsCurrentFrame->value();
}


void MainWindow::on_tbZoomIn_clicked()
{
    // Compute and set the new minimum and maximum.
    float width = _ui.spvCanvas->max() - _ui.spvCanvas->min();
    float middle = _ui.spvCanvas->min() + width / 2.0;
    float newMin = middle - width / (2.0 * ZOOM_FACTOR);
    float newMax = middle + width / (2.0 * ZOOM_FACTOR);
    if (newMin >= newMax)
        return;
    _ui.spvCanvas->setMinMax(newMin, newMax);
    // Update the scrollbar.
    updateScrollBarRange();
    // Finally update the widget.
    _ui.spvCanvas->update();
}


void MainWindow::on_tbZoomOut_clicked()
{
    // Compute and set the new minimum and maximum.
    float width = _ui.spvCanvas->max() - _ui.spvCanvas->min();
    float newMin = std::max<float>(0.0, _ui.spvCanvas->min() - width / 4.0 * ZOOM_FACTOR);
    float newMax = std::min<float>(1.0, _ui.spvCanvas->max() + width / 4.0 * ZOOM_FACTOR);
    _ui.spvCanvas->setMinMax(newMin, newMax);
    // Update the scrollbar.
    updateScrollBarRange();
    // Finally update the widget.
    _ui.spvCanvas->update();
}


void MainWindow::updateScrollBarRange()
{
    int steps = (int)ceilf(1.0 / (_ui.spvCanvas->max() - _ui.spvCanvas->min())) - 1;
    _ui.hsbSamples->blockSignals(true);
    _ui.hsbSamples->setValue(min<int>(steps, _ui.hsbSamples->value() + 1));
    _ui.hsbSamples->setRange(0, steps);
    _ui.hsbSamples->blockSignals(false);
}


void MainWindow::on_hsbSamples_valueChanged(int value)
{
    float window = _ui.spvCanvas->max() - _ui.spvCanvas->min();
    float newMin = value * window;
    float newMax = newMin + window;
    if (newMax > 1.0) {
        newMax = 1.0;
        newMin = newMax - window;
    }
    _ui.spvCanvas->setMinMax(newMin, newMax);
}


void MainWindow::on_pbPlay_clicked()
{
    if (_playing) {
        _pbThread->setPlaybackState(PlaybackThread::Pause);
        _ui.pbPlay->setText(tr("Play"));
        enableUI(false);
    } else {
        _pbThread->setPlaybackState(PlaybackThread::Play);
        _ui.pbPlay->setText(tr("Pause"));
        enableUI(true);
    }

    _playing = !_playing;
}


void MainWindow::enableUI(bool playing)
{
    _ui.twMovies->setEnabled(!playing);
    _ui.tbPrev->setEnabled(!playing);
    _ui.tbNext->setEnabled(!playing);
    _ui.tbSyncMovie->setEnabled(!playing);
    _ui.hsCurrentFrame->setEnabled(!playing);
    _ui.pbFlipbook->setEnabled(!playing);
    _ui.spbCurrentFrame->setEnabled(!playing);
    _ui.pbReset->setEnabled(!playing);
    _ui.pbSave->setEnabled(!playing);
    _ui.pbSaveAndContinue->setEnabled(!playing);
    _ui.tbSyncSamples->setEnabled(!playing);
    _ui.groupBoxReceivers->setEnabled(!playing);
    _ui.groupBoxAdditionalInfo->setEnabled(!playing);
}


void MainWindow::on_spvCanvas_posClicked(float pos)
{
    _pbThread->setPlaybackPos(pos);
}


void MainWindow::xx_on_pbThread_playbackPosChanged(float pos)
{
    _ui.spvCanvas->setMarkerPos(pos);
    if (_flipping)
        _ui.hsCurrentFrame->setValue(pos * _ui.hsCurrentFrame->maximum());
}


void MainWindow::xx_on_pbThread_finished()
{
    _ui.pbPlay->setText(tr("Play"));
    enableUI(false);
    _playing = false;
}


void MainWindow::on_tbSyncSamples_clicked()
{
    float window = _ui.spvCanvas->max() - _ui.spvCanvas->min();
    float newMin = max<float>(0.0, _ui.spvCanvas->markerPos() - window / 2.0);
    float newMax = newMin + window;
    if (newMax > 1.0) {
        newMax = 1.0;
        newMin = newMax - window;
    }
    _ui.spvCanvas->setMinMax(newMin, newMax);

    _pbThread->setPlaybackPos(currentPosition());
}


void MainWindow::on_tbSyncMovie_clicked()
{
    // Delegate this task to the current-frame slider.
    _ui.hsCurrentFrame->setValue(
            _pbThread->playbackPos() * _ui.hsCurrentFrame->maximum()
    );
}


void MainWindow::on_pbFlipbook_clicked()
{
    if (_flipping) {
        _ui.pbFlipbook->setText(tr("Flipbook"));
        _pbThread->setPlaybackState(PlaybackThread::Pause);
        // Since during flipping no annotation retrieval is done, the
        // annotations for the current frame have to be retrieved now.
        loadAnnotations(false);
        enableUI(false);
        _ui.pbPlay->setEnabled(true);
    } else {
        _ui.pbFlipbook->setText(tr("Still image"));
        _pbThread->setPlaybackState(PlaybackThread::Play);
        // During flipping no annotation retrieval is done, so clear all
        // possible annotations at this point.
        clearAnnotations();
        enableUI(true);
        // Because enableUI(true) implicitly disables the flipbook button, the
        // button's `enabled' state has to be manually reset to true.
        _ui.pbFlipbook->setEnabled(true);
        _ui.pbPlay->setEnabled(false);
    }

    _flipping = !_flipping;
}


void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    ev->ignore();

    switch (ev->key()) {
    case Qt::Key_F1:
        on_tbSyncSamples_clicked();
        ev->accept();
        break;

    case Qt::Key_F2:
        on_pbPlay_clicked();
        ev->accept();
        break;

    case Qt::Key_F3:
        on_tbPrev_clicked();
        ev->accept();
        break;

    case Qt::Key_F4:
        on_tbNext_clicked();
        ev->accept();
        break;

    case Qt::Key_F5:
        update();
        ev->accept();
        break;

    case Qt::Key_F6:
        on_pbSaveAndContinue_clicked();
        ev->accept();
        break;
    }

    QMainWindow::keyPressEvent(ev);
}


void MainWindow::clearAnnotations()
{
    for (int i = 0; i < _ui.lwReceivers->count(); i++)
        _ui.lwReceivers->item(i)->setCheckState(Qt::Unchecked);

    _ui.cbSpeaking->setChecked(false);
    _ui.cbLaughing->setChecked(false);
}


void MainWindow::loadAnnotations(bool takeAlong)
{
    vector<int> selectedReceiverIDs;
    Attributes attributes;

    // First get the annotations for this frame.
    const bool hasAnnotations = DBController::instance()->getAnnotation(
            currentTimestamp(),
            _participant->id,
            &selectedReceiverIDs,
            &attributes
    );

    // If no annotations have been stored for the current frame and the
    // takeAlong flag is set, simply return and leave everything untouched.
    if (takeAlong && !hasAnnotations)
        return;

    // Reset the checked/unchecked state of the receivers.
    for (int i = 0; i < _ui.lwReceivers->count(); i++) {
        QListWidgetItem *lwi = _ui.lwReceivers->item(i);
        lwi->setCheckState(Qt::Unchecked);
        for (vector<int>::const_iterator it = selectedReceiverIDs.begin();
            it != selectedReceiverIDs.end(); it++)
        {
            if (lwi->data(Qt::UserRole).toInt() == *it) {
                lwi->setCheckState(Qt::Checked);
                break;
            }
        }
    }

    // Adapt the check/unchecked state of the `laughing' checkbox.
    _ui.cbSpeaking->setChecked(attributes.speaking);
    _ui.cbLaughing->setChecked(attributes.laughing);
}


void MainWindow::goToFrame(int frame)
{
    MovieWidget *mw = static_cast<MovieWidget *>(_ui.twMovies->currentWidget());
    mw->setPosition(positionForValue(frame));
}


} // namespace hiwi
