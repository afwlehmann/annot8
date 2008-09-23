//
// DBSetupMoviesWidget.cpp
// $Id$
//


#include "DBSetupMoviesWidget.h"
#include <QSqlTableModel>
#include <QHeaderView>
#include <QSqlRecord>
#include <set>


namespace hiwi {


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

    // TODO

    return true;
}


void DBSetupMoviesWidget::on_tbAddMovie_clicked()
{
    QSqlTableModel *model =
        static_cast<QSqlTableModel *>(_ui.tvMovies->model());

    model->insertRow(model->rowCount());
    QModelIndex index = model->index(model->rowCount() - 1, 1);
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


} // namespace hiwi
