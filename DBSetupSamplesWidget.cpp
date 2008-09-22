//
// DBSetupSamplesWidget.cpp
// $Id$
//


#include "DBSetupSamplesWidget.h"
#include "DBController.h"
#include "FileNameDelegate.h"
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>
#include <QHeaderView>
#include <QFile>
#include <QFileDialog>


using namespace std;


namespace hiwi {


DBSetupSamplesWidget::DBSetupSamplesWidget(QWidget *parent) :
    QWizardPage(parent)
{
    _ui.setupUi(this);

    setTitle(tr("Samples setup"));
    setSubTitle(tr("Please fill in a sound file for each participant."));

    QSqlRelationalTableModel *model = new QSqlRelationalTableModel();
    model->setTable("samples");
    model->setRelation(0, QSqlRelation("pinfo", "id", "name_info"));
    model->select();
    model->setHeaderData(0, Qt::Horizontal, tr("Participant"));
    model->setHeaderData(1, Qt::Horizontal, tr("Sound file"));
    _ui.tvSamples->setModel(model);
    _ui.tvSamples->setItemDelegate(new QSqlRelationalDelegate(_ui.tvSamples));
    _ui.tvSamples->setItemDelegateForColumn(
        model->fieldIndex("filename"),
        new FileNameDelegate(this,
                             tr("Wave files (*.wav);;"
                                "MP3 files (*.mp3);;"
                                "OGG files (*.ogg)"))
    );
    _ui.tvSamples->verticalHeader()->setVisible(false);
    _ui.tvSamples->resizeColumnsToContents();

    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
            SIGNAL(completeChanged()));
}


bool DBSetupSamplesWidget::validatePage() const
{
    static_cast<QSqlRelationalTableModel *>(_ui.tvSamples->model())->submitAll();
    return true;
}


bool DBSetupSamplesWidget::isComplete() const
{
    // Assure that every sound file exists.
    QSqlRelationalTableModel *model =
        static_cast<QSqlRelationalTableModel *>(_ui.tvSamples->model());
    const int fiFileName = model->fieldIndex("filename");
    for (int row = 0; row < model->rowCount(); row++) {
        QModelIndex index = model->index(row, fiFileName);
        if (!QFile::exists(model->data(index).toString()))
            return false;
    }

    return true;
}


void DBSetupSamplesWidget::initializePage()
{
    static_cast<QSqlRelationalTableModel *>(_ui.tvSamples->model())->select();
}


} // namespace hiwi
