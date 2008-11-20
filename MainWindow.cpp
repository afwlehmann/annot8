//
// MainWindow.cpp
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


#include "MainWindow.h"
#include "DBController.h"
#include "MovieWidget.h"
#include "Participant.h"
#include "PlaybackThread.h"
#include "Samples.h"
#include "AboutDlg.h"

#include <stdexcept>
#include <cmath>
#include <cassert>
#include <ctime>

#include <QMessageBox>
#include <QListWidgetItem>
#include <QProgressDialog>
#include <QKeyEvent>


// The zoom factor to be used for zoom-in and -out.
#define ZOOM_FACTOR 1.5f


using namespace annot8::audio;
using namespace std;


namespace annot8 {


MainWindow::MainWindow(int participantID, bool takeAlong) :
    _participant(0),
    _pbThread(0),
    _playing(false),
    _flipping(false),
    _takeAlong(takeAlong),
    _lastSelectedMovieIdxPos(0)
{
    _ui.setupUi(this);
    _ui.twMovies->removeTab(0);

    connect(_ui.hsCurrentFrame, SIGNAL(valueChanged(int)),
            SLOT(xx_on_hsCurrentFrame_valueChanged(int)));
    connect(_ui.actionClear_annotations,
            SIGNAL(triggered(bool)), SLOT(clearAnnotations()));
    connect(_ui.actionCopy_annotations,
            SIGNAL(triggered(bool)), SLOT(copyAnnotations()));
    connect(_ui.actionPaste_annotations,
            SIGNAL(triggered(bool)), SLOT(pasteAnnotations()));

    _lastSelectedMovieIdx[0] = 0;
    _lastSelectedMovieIdx[1] = 0;

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

    // Remember the selected movie.
    _lastSelectedMovieIdxPos = (_lastSelectedMovieIdxPos + 1) % 2;
    _lastSelectedMovieIdx[_lastSelectedMovieIdxPos] = index;

    // Assure that the newly selected movie's position gets updated as well,
    // i.e. delegate this task to the horizontal slider as (almost) everybody
    // else does. However, don't mess with the annotations.
    xx_on_hsCurrentFrame_valueChanged(currentFrame(), false);
}


void MainWindow::xx_on_hsCurrentFrame_valueChanged(int value, bool ldAnnotations)
{
    // Since the associated signal is already emitted during Qt's `startup' we
    // must introduce a little safety check at this point:
    if (!_participant)
        return;

    // Go to the desired frame.
    setCurrentFrame(value);

    // Update the annotations.
    if (ldAnnotations)
        loadAnnotations();

    if (!_flipping) {
        // Adapt the samples position.
        _pbThread->setPlaybackPos(positionForValue(value));
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
    setCurrentFrame(nextFrame);

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
    saveAnnotations();
    // Reflect the changes by adapting the groupbox labels.
    adaptGroupBoxLabels(false);
}


void MainWindow::on_pbSaveAndContinue_clicked()
{
    // Let "The Others" do the rest ;-)
    on_pbSave_clicked();
    on_tbNext_clicked();
    on_tbSyncSamples_clicked();
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
    _ui.hsCurrentFrame->setRange(0, lengthCheck - 1);
    _ui.spbCurrentFrame->setRange(0, lengthCheck - 1);
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


void MainWindow::setCurrentFrame(int frame)
{
    MovieWidget *mw = static_cast<MovieWidget *>(_ui.twMovies->currentWidget());
    mw->setPosition(positionForValue(frame));
}


void MainWindow::updateScrollBarRange()
{
    int steps = (int)ceilf(1.0 / (_ui.spvCanvas->max() - _ui.spvCanvas->min())) - 1;
    _ui.hsbSamples->blockSignals(true);
    _ui.hsbSamples->setValue(min<int>(steps, _ui.hsbSamples->value() + 1));
    _ui.hsbSamples->setRange(0, steps);
    _ui.hsbSamples->blockSignals(false);
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


void MainWindow::adaptGroupBoxLabels(bool takeAlong) {
    if (takeAlong) {
        _ui.groupBoxReceivers->setTitle(tr("Receivers (proposed)"));
        _ui.groupBoxAdditionalInfo->setTitle(tr("Additional information (proposed)"));
    } else {
        _ui.groupBoxReceivers->setTitle(tr("Receivers"));
        _ui.groupBoxAdditionalInfo->setTitle(tr("Additional information"));
    }
}


void MainWindow::clearAnnotations()
{
    for (int i = 0; i < _ui.lwReceivers->count(); i++)
        _ui.lwReceivers->item(i)->setCheckState(Qt::Unchecked);

    _ui.cbSpeaking->setChecked(false);
    _ui.cbLaughing->setChecked(false);
}


void MainWindow::saveAnnotations()
{
    // Retrieve the current annotations
    vector<int> selectedReceiverIDs;
    Attributes attributes;
    getCurrentAnnotations(&selectedReceiverIDs, &attributes);

    // Store everything in the database.
    DBController::instance()->storeAnnotation(
            currentTimestamp(),
            _participant->id,
            selectedReceiverIDs,
            attributes
    );

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
    if (takeAlong && !hasAnnotations) {
        adaptGroupBoxLabels(true);
        return;
    } else
        adaptGroupBoxLabels(false);

    // Update the user interface.
    setCurrentAnnotations(selectedReceiverIDs, attributes);
}


void MainWindow::copyAnnotations()
{
    getCurrentAnnotations(&_clipboard.receiverIDs, &_clipboard.attributes);
}


void MainWindow::pasteAnnotations()
{
    setCurrentAnnotations(_clipboard.receiverIDs, _clipboard.attributes);
}


void MainWindow::getCurrentAnnotations(vector<int> *receiverIDs,
                                       Attributes *attributes)
{
    receiverIDs->clear();
    for (int i = 0; i < _ui.lwReceivers->count(); i++) {
        QListWidgetItem *lwi = _ui.lwReceivers->item(i);
        if (lwi->checkState() == Qt::Checked)
            receiverIDs->push_back(lwi->data(Qt::UserRole).toInt());
    }
    *attributes = Attributes(_ui.cbSpeaking->isChecked(),
                             _ui.cbLaughing->isChecked());
}


void MainWindow::setCurrentAnnotations(const vector<int> &receiverIDs,
                                       const Attributes &attributes)
{
    // Reset the checked/unchecked state of the receivers.
    for (int i = 0; i < _ui.lwReceivers->count(); i++) {
        QListWidgetItem *lwi = _ui.lwReceivers->item(i);
        lwi->setCheckState(Qt::Unchecked);
        for (vector<int>::const_iterator it = receiverIDs.begin();
            it != receiverIDs.end(); it++)
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

    // Adapt the groupbox labels.
    adaptGroupBoxLabels(false);
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


void MainWindow::on_spvCanvas_posClicked(float pos)
{
    _pbThread->setPlaybackPos(pos);
}


void MainWindow::xx_on_pbThread_playbackPosChanged(float pos)
{
    // Track the marker while in playback or flipbook mode.
    if (pos > _ui.spvCanvas->max() && (_flipping || _playing)) {
        _ui.hsbSamples->setValue(
                qMin(_ui.hsbSamples->value() + 1, _ui.hsbSamples->maximum()));
    }
    // Update the marker.
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


void MainWindow::selectPreviousMovie()
{
    int newIndex = _ui.twMovies->currentIndex() - 1;
    if (newIndex < 0)
        newIndex = _ui.twMovies->count() - 1;
    _ui.twMovies->setCurrentIndex(newIndex);
}


void MainWindow::selectNextMovie()
{
    int newIndex = (_ui.twMovies->currentIndex() + 1) % _ui.twMovies->count();
    _ui.twMovies->setCurrentIndex(newIndex);
}


void MainWindow::swapMovies()
{
    int newIndex = _lastSelectedMovieIdx[(_lastSelectedMovieIdxPos + 1) % 2];
    _ui.twMovies->setCurrentIndex(newIndex);
}


void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    QMainWindow::keyPressEvent(ev);

    // During Flipbook-mode no keypresses shall be allowed.
    if (_flipping)
        return;

    // Also, during sound playback only pausing the playback shall be allowed.
    if (_playing && ev->key() != Qt::Key_F2)
        return;

    switch (ev->key()) {
    case Qt::Key_F1:
        on_tbSyncSamples_clicked();
        break;

    case Qt::Key_F2: // If you change this, change the if-clause above as well.
        on_pbPlay_clicked();
        break;

    case Qt::Key_F3:
        on_tbPrev_clicked();
        break;

    case Qt::Key_F4:
        on_tbNext_clicked();
        break;

    case Qt::Key_F5:
        update();
        break;

    case Qt::Key_F6:
        on_pbSaveAndContinue_clicked();
        break;

    case Qt::Key_F7:
        // Play sound for exactly two seconds.
        {
            on_pbPlay_clicked();
            time_t startTime = time(NULL);
            while (time(NULL) < startTime + 2) {
                // Wait
            }
            on_pbPlay_clicked();
        }
        break;

    case Qt::Key_F8:
        swapMovies();
        break;

    case Qt::Key_F9:
        selectPreviousMovie();
        break;

    case Qt::Key_F10:
        selectNextMovie();
        break;
    }
}


} // namespace annot8
