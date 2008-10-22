//
// ChooseParticipantDlg.cpp
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


#include "ChooseParticipantDlg.h"
#include "DBController.h"
#include "Participant.h"
#include <vector>
#include <stdexcept>
#include <QListWidgetItem>


namespace annot8 {


ChooseParticipantDlg::ChooseParticipantDlg(QWidget *parent) : QDialog(parent)
{
    _ui.setupUi(this);
    connect(_ui.lwParticipants, SIGNAL(itemActivated(QListWidgetItem *)),
            SLOT(accept()));

    // Get the available participants.
    std::vector<Participant *> participants =
        DBController::instance()->getParticipants(true);
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


} // namespace annot8
