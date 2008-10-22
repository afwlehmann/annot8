//
// DBSetupMoviesWidget.cpp
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


#include "DBSetupMoviesWidget.h"
#include <QSqlTableModel>
#include <QHeaderView>
#include <QSqlRecord>
#include <set>


namespace annot8 {


DBSetupMoviesWidget::DBSetupMoviesWidget(QWidget *parent) :
    QWizardPage(parent)
{
    _ui.setupUi(this);

    setTitle(tr("Movies setup"));
    setSubTitle(tr("Please define the available movies, where the "
            "movies' filenames will be assembled by appending the prefix, "
            "the neccessary number of digits and eventually the suffix. If, "
            "for example, a movie's files were named 'abc_0001.jpg' through "
            "'abc_2000.jpg' and you wanted to use frames 50 up to and including "
            "995, you'd use the prefix 'abc_', the suffix '.jpg', 4 digits, "
            "the first frame 50 and the last frame 995."));

    QSqlTableModel *model = new QSqlTableModel(_ui.tvMovies);
    model->setTable("movies");
    model->setEditStrategy(QSqlTableModel::OnFieldChange);
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("Prefix"));
    model->setHeaderData(1, Qt::Horizontal, tr("Suffix"));
    model->setHeaderData(2, Qt::Horizontal, tr("First #"));
    model->setHeaderData(3, Qt::Horizontal, tr("Last #"));
    model->setHeaderData(4, Qt::Horizontal, tr("ms/frame"));
    model->setHeaderData(5, Qt::Horizontal, tr("Digits"));
    model->setHeaderData(6, Qt::Horizontal, tr("Description"));
    _ui.tvMovies->setModel(model);
    _ui.tvMovies->verticalHeader()->setVisible(false);
    _ui.tvMovies->resizeColumnsToContents();

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            SIGNAL(completeChanged()));
}


bool DBSetupMoviesWidget::isComplete() const
{
    QSqlTableModel *model =
        static_cast<QSqlTableModel *>(_ui.tvMovies->model());

    // Assure that any movies exist at all.
    if (model->rowCount() <= 0)
        return false;
    
    // TODO

    return true;
}


void DBSetupMoviesWidget::on_tbAddMovie_clicked()
{
    QSqlTableModel *model =
        static_cast<QSqlTableModel *>(_ui.tvMovies->model());

    model->insertRow(model->rowCount());
    QModelIndex index = model->index(model->rowCount() - 1, 0);
    _ui.tvMovies->setCurrentIndex(index);
    _ui.tvMovies->edit(index);

    emit completeChanged();
}


void DBSetupMoviesWidget::on_tbRemoveSelected_clicked()
{
    // Create an ordered set of the rows that are to be removed.
    std::set<int> selectedRows;
    foreach (const QModelIndex &index,
             _ui.tvMovies->selectionModel()->selectedIndexes())
    {
        selectedRows.insert(index.row());
    }

    // Reverse traversal of the ordered set because this way the numbers of the
    // rows that are to be removed are guaranteed to stay the same.
    QSqlTableModel *model = static_cast<QSqlTableModel *>(_ui.tvMovies->model());
    for (std::set<int>::reverse_iterator rIt = selectedRows.rbegin();
         rIt != selectedRows.rend(); rIt++)
    {
        model->removeRow(*rIt);
    }

    emit completeChanged();
}


} // namespace annot8
