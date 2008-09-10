//
// MainWindow.cpp
// $Id$
//


#include "MainWindow.h"
#include "DBController.h"
#include "MovieWidget.h"
#include "Participant.h"
#include <stdexcept>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QProgressDialog>


namespace hiwi {


MainWindow::MainWindow(int participantID) :
    _participant(0)
{
    _ui.setupUi(this);
    _ui.twMovies->removeTab(0);

    // Get the movies.
    std::vector<Movie *> movies = DBController::instance()->getAvailableMovies();
    if (movies.empty())
        throw std::runtime_error("There are no movies to annotate.");

    // Display a progress dialog because prefetching might take a while...
    QProgressDialog progDlg(tr("Initializing the movies..."), QString::null,
                            0, movies.size());
    progDlg.setValue(0);
    progDlg.show();

    // Create one tab per movie and make sure that all movies have the exact
    // same length.
    int lengthCheck;
    for (std::vector<Movie *>::const_iterator it = movies.begin();
        it != movies.end(); it++)
    {
        int length = (*it)->lastFrame - (*it)->firstFrame + 1;
        if (it == movies.begin())
            lengthCheck = length;
        else if (length != lengthCheck)
            throw std::runtime_error("All movies must have equal length!");

        MovieWidget *mw = new MovieWidget(*it);
        _ui.twMovies->addTab(mw, QString::fromStdString(mw->getDescription()));
        mw->initialize();
        progDlg.setValue(progDlg.value() + 1);
    }

    // Get the participants.
    std::vector<Participant *> participants =
        DBController::instance()->getParticipants();
    if (participants.empty())
        throw std::runtime_error("There are no participants.");

    // Create one list widget item per participant. Also store the participant
    // that was selected for annotation while deleting the others.
    for (std::vector<Participant *>::const_iterator it = participants.begin();
        it != participants.end(); it++)
    {
        Participant *p = *it;
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

        if (p->id == participantID)
            _participant = p;
        else
            delete p;
    }
    participants.clear();

    // Assure that the _participant has been set.
    assert(_participant);
    _ui.leName->setText(QString::fromStdString(_participant->name));
    _ui.leFirstName->setText(QString::fromStdString(_participant->firstName));
    _ui.leNumber->setText(QString::number(_participant->number));

    // Initialize the current-frame slider with the first movie's number of
    // frames.
    _ui.hsCurrentFrame->setRange(0,
        static_cast<MovieWidget *>(_ui.twMovies->widget(0))->getNumFrames() - 1);

    // Delegate the retrieval of the annotations for the initial frame.
    on_pbReset_clicked();
}


MainWindow::~MainWindow()
{
    if (_participant) {
        delete _participant;
        _participant = 0;
    }
}


void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this, tr("Unavailable"), tr("Not yet implemented."));
}


void MainWindow::on_actionPreferences_triggered()
{
    QMessageBox::information(this, tr("Unavailable"), tr("Not yet implemented."));
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

    // Update the current-frame range according to the number of frames of the
    // currently selected movie.
    setRange(0,
             static_cast<MovieWidget *>(_ui.twMovies->widget(index))->getNumFrames() - 1);

    // Assure that the newly selected movie's position gets updated as well,
    // i.e. delegate this task to the horizontal slider as everybody else does.
    on_hsCurrentFrame_valueChanged(_ui.hsCurrentFrame->value());
}


void MainWindow::on_hsCurrentFrame_valueChanged(int value)
{
    // Since the associated is emitted already during Qt's `startup' we must
    // introduce a little safety check at this point:
    if (!_participant)
        return;

    // Give the user a chance to save his/her data if applicable.
    if (isWindowModified() && QMessageBox::Yes != QMessageBox::question(this,
            tr("Attention"), tr("This frame has been modified. If you continue,"
            " all modifications will be lost. Do you still want to continue?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
    {
        return;
    }

    // Set the new movie position according to the slider's value.
    MovieWidget *mw = static_cast<MovieWidget *>(_ui.twMovies->currentWidget());
    mw->setPosition(positionForValue(value));

    // Resetting the windows `modified' flag isn't neccessary at this point
    // as it is done by on_pbReset_clicked instead.

    // Retrieve the new frame's annotations.
    on_pbReset_clicked();
}


void MainWindow::on_tbPrev_clicked()
{
    // Delegate the task to the current frame slider.
    if (_ui.hsCurrentFrame->value() > 0)
        _ui.hsCurrentFrame->setValue(_ui.hsCurrentFrame->value() - 1);
}


void MainWindow::on_tbNext_clicked()
{
    // Delegate the task to the current frame slider.
    if (_ui.hsCurrentFrame->value() < _ui.hsCurrentFrame->maximum())
        _ui.hsCurrentFrame->setValue(_ui.hsCurrentFrame->value() + 1);
}


void MainWindow::on_lwReceivers_itemClicked(QListWidgetItem *)
{
     setWindowModified(true);
}


void MainWindow::on_pbReset_clicked()
{
    std::vector<int> selectedReceiverIDs;
    bool laughing;

    // First get the annotations for this frame.
    DBController::instance()->getAnnotation(
            positionForValue(_ui.hsCurrentFrame->value()),
            _participant->id,
            &selectedReceiverIDs,
            &laughing
    );

    // Reset the checked/unchecked state of the receivers.
    for (int i = 0; i < _ui.lwReceivers->count(); i++) {
        QListWidgetItem *lwi = _ui.lwReceivers->item(i);
        lwi->setCheckState(Qt::Unchecked);
        for (std::vector<int>::const_iterator it = selectedReceiverIDs.begin();
            it != selectedReceiverIDs.end(); it++)
        {
            if (lwi->data(Qt::UserRole).toInt() == *it) {
                lwi->setCheckState(Qt::Checked);
                break;
            }
        }
    }

    // Adapt the check/unchecked state of the `laughing' checkbox.
    _ui.cbLaughing->setChecked(laughing);

    // Reset the `window modified' flag.
    setWindowModified(false);
}


void MainWindow::on_pbSave_clicked()
{
    // First determine the ids of the checked receivers.
    std::vector<int> selectedReceiverIDs;
    for (int i = 0; i < _ui.lwReceivers->count(); i++) {
        QListWidgetItem *lwi = _ui.lwReceivers->item(i);
        if (lwi->checkState() == Qt::Checked)
            selectedReceiverIDs.push_back(lwi->data(Qt::UserRole).toInt());
    }

    // Then store those ids in conjunction with the `laughing' attribute.
    DBController::instance()->storeAnnotation(
            positionForValue(_ui.hsCurrentFrame->value()),
            _participant->id,
            selectedReceiverIDs,
            _ui.cbLaughing->isChecked()
    );

    // Reset the `window modified' flag.
    setWindowModified(false);
}


void MainWindow::on_pbSaveAndContinue_clicked()
{
    // Let "The Others" do the rest ;-)
    on_pbSave_clicked();
    on_tbNext_clicked();
}


float MainWindow::positionForValue(int value) const
{
    return (float)value / (float)_ui.hsCurrentFrame->maximum();
}


void MainWindow::setRange(int min, int max)
{
    _ui.hsCurrentFrame->setRange(min, max);
    _ui.spbCurrentFrame->setRange(min, max);
}


} // namespace hiwi
