//
// ChooseParticipantDlg.cpp
// $Id$
//


#include "ChooseParticipantDlg.h"
#include "DBController.h"
#include "Participant.h"
#include <vector>
#include <stdexcept>
#include <QListWidgetItem>


namespace hiwi {


ChooseParticipantDlg::ChooseParticipantDlg(QWidget *parent) : QDialog(parent)
{
    _ui.setupUi(this);
    connect(_ui.lwParticipants, SIGNAL(itemActivated(QListWidgetItem *)),
            SLOT(accept()));

    // Get the available participants.
    std::vector<Participant *> participants =
        DBController::instance()->getParticipants();
    if (participants.empty())
        throw std::runtime_error("There are no participants.");

    for (std::vector<Participant *>::const_iterator it = participants.begin();
        it != participants.end(); it++)
    {
        // Create a new list widget item with the data from the participant.
        Participant *p = *it;
        QListWidgetItem *lwi = new QListWidgetItem(
                QString("%1, %2 (%3)")
                .arg(QString::fromStdString(p->name))
                .arg(QString::fromStdString(p->firstName))
                .arg(p->number)
        );
        lwi->setData(Qt::UserRole, p->id);
        _ui.lwParticipants->addItem(lwi);
        // No further need of the participant, so free the allocated memory.
        delete p;
    }

    _ui.lwParticipants->setCurrentRow(0);
    raise();
}


int ChooseParticipantDlg::getParticipantID() const
{
    QListWidgetItem *lwi = _ui.lwParticipants->currentItem();
    return lwi ? lwi->data(Qt::UserRole).toInt() : 0;
}


} // namespace hiwi
