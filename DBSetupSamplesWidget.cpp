//
// DBSetupSamplesWidget.cpp
// $Id$
//


#include "DBSetupSamplesWidget.h"
#include "DBController.h"
#include "FileNameDelegate.h"
#include <QHeaderView>
#include <QFile>
#include <QMessageBox>
#include <map>


using namespace std;


namespace hiwi {


DBSetupSamplesWidget::DBSetupSamplesWidget(QWidget *parent) :
    QWizardPage(parent)
{
    _ui.setupUi(this);

    _ui.twSamples->setItemDelegateForColumn(
            1,
            new FileNameDelegate(this,
                                 tr("Wave files (*.wav);;"
                                    "MP3 files (*.mp3);;"
                                    "OGG files (*.ogg)"))
    );
    _ui.twSamples->verticalHeader()->setVisible(false);
    _ui.twSamples->horizontalHeader()->stretchLastSection();
    on_tbRefresh_clicked();

    setTitle(tr("Samples setup"));
    setSubTitle(tr("Please fill in a sound file for each participant."));

    connect(_ui.twSamples, SIGNAL(cellChanged(int, int)),
            SIGNAL(completeChanged()));
}


bool DBSetupSamplesWidget::isComplete() const
{
    // Check that at least one sample has been given.
    for (int row = 0; row < _ui.twSamples->rowCount(); row++) {
        const QString fileName =
            _ui.twSamples->item(row, 1)->data(Qt::DisplayRole).toString();

        if (!fileName.isEmpty())
            return true;
    }

    return false;
}

    
bool DBSetupSamplesWidget::validatePage()
{
    map<int, string> samples;

    // Check that every given filename names an existing file.
    for (int row = 0; row < _ui.twSamples->rowCount(); row++) {
        const QString fileName =
            _ui.twSamples->item(row, 1)->data(Qt::DisplayRole).toString();

        if (fileName.isEmpty()) {
            continue;
        } else if (!QFile::exists(fileName)) {
            // The file doesn't exist -- complain!
            _ui.twSamples->setCurrentCell(row, 1);
            QMessageBox::information(this,
                    tr("Missing file"),
                    tr("The file '%1' doesn't exist.").arg(fileName));
            return false;
        }

        const int participantID =
            _ui.twSamples->item(row, 0)->data(Qt::UserRole).toInt();
        samples[participantID] = fileName.toStdString();
    }

    // Once everything is alright, go ahead and save the data.
    DBController::instance()->storeAvailableSamples(samples);

    return true;
}


void DBSetupSamplesWidget::on_tbRefresh_clicked()
{
    // Block the table widget's signals in order to avoid interference
    // with isComplete() because the table widget's cellChanged-signal
    // might be emitted during refresh.
    _ui.twSamples->blockSignals(true);

    // Get the available participants info.
    map<int, string> pinfo;
    DBController::instance()->getParticipantsInfo(&pinfo);

    // Get the available samples.
    map<int, string> samples;
    DBController::instance()->getAvailableSamples(&samples);

    // Fill the table widget with the data.
    _ui.twSamples->clearContents(); // clear() would destroy the headers, too.
    _ui.twSamples->setRowCount(0);  // Explictly neccessary.
    _ui.twSamples->setSortingEnabled(false);
    map<int, string>::const_iterator it;
    for (it = pinfo.begin(); it != pinfo.end(); it++) {
        QTableWidgetItem *twiParticipant =
            new QTableWidgetItem(QString::fromStdString(it->second));
        twiParticipant->setData(Qt::UserRole, it->first);
        twiParticipant->setFlags(Qt::NoItemFlags);

        QTableWidgetItem *twiFile =
            new QTableWidgetItem(QString::fromStdString(samples[it->first]));
        //twiFile->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable);

        _ui.twSamples->insertRow(0);
        _ui.twSamples->setItem(0, 0, twiParticipant);
        _ui.twSamples->setItem(0, 1, twiFile);
    }
    _ui.twSamples->setSortingEnabled(true);
    _ui.twSamples->resizeColumnsToContents();

    // Reactive the table widget's signal again.
    _ui.twSamples->blockSignals(false);
    emit completeChanged();
}


} // namespace hiwi
