//
// DBSetupParticipantsWidget.cpp
// $Id$
//


#include "DBSetupParticipantsWidget.h"
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QHeaderView>
#include <set>


namespace hiwi {


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

    // Assure that each participant has a name.
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

    model->insertRow(model->rowCount());
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


} // namespace hiwi
