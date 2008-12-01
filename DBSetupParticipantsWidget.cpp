//
// DBSetupParticipantsWidget.cpp
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


#include "DBSetupParticipantsWidget.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QHeaderView>
#include <set>


namespace annot8 {


DBSetupParticipantsWidget::DBSetupParticipantsWidget(QWidget *parent) :
    QWizardPage(parent)
{
    _ui.setupUi(this);

    setTitle(tr("Participants setup"));
    setSubTitle(tr("Please fill in the list of participants."));

    QSqlTableModel *model = new QSqlTableModel(_ui.tvParticipants);
    model->setTable("participants");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();
    model->setHeaderData(1, Qt::Horizontal, tr("Name"));
    model->setHeaderData(2, Qt::Horizontal, tr("First name"));
    model->setHeaderData(3, Qt::Horizontal, tr("Number"));
    _ui.tvParticipants->setModel(model);
    _ui.tvParticipants->hideColumn(0);
    _ui.tvParticipants->verticalHeader()->setVisible(false);
    _ui.tvParticipants->resizeColumnsToContents();

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            SIGNAL(completeChanged()));
}


bool DBSetupParticipantsWidget::isComplete() const
{
    QSqlTableModel *model =
        static_cast<QSqlTableModel *>(_ui.tvParticipants->model());

    // Assure that participants exist at all.
    if (model->rowCount() <= 0)
        return false;

    // Assure that each participant at least has a name.
    const int fiName = model->fieldIndex("name");
    for (int row = 0; row < model->rowCount(); row++) {
        QModelIndex index(model->index(row, fiName));
        if (model->data(index).toString().isEmpty())
            return false;
    }

    return true;
}


void DBSetupParticipantsWidget::on_tbAddParticipant_clicked()
{
    QSqlTableModel *model =
        static_cast<QSqlTableModel *>(_ui.tvParticipants->model());

    QSqlRecord r = model->record();
    r.setValue("name", "Frog");
    r.setValue("first_name", "Kermit");
    model->insertRecord(-1, r);

    QModelIndex index = model->index(model->rowCount() - 1, 1);
    _ui.tvParticipants->setCurrentIndex(index);
    _ui.tvParticipants->edit(index);

    emit completeChanged();
}


void DBSetupParticipantsWidget::on_tbRemoveSelected_clicked()
{
    // Create an ordered set of the rows that are to be removed.
    std::set<int> selectedRows;
    foreach (const QModelIndex &index,
             _ui.tvParticipants->selectionModel()->selectedIndexes())
    {
        selectedRows.insert(index.row());
    }

    // Reverse traversal of the ordered set because this way the numbers of the
    // rows that are to be removed are guaranteed to stay the same.
    QSqlTableModel *model = static_cast<QSqlTableModel *>(_ui.tvParticipants->model());
    for (std::set<int>::reverse_iterator rIt = selectedRows.rbegin();
         rIt != selectedRows.rend(); rIt++)
    {
        model->removeRow(*rIt);
    }

    emit completeChanged();
}


} // namespace annot8
