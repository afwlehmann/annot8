//
// FileNameWidget.cpp
// $Id$
//


#include "FileNameWidget.h"
#include <QFileDialog>
#include <QFile>


namespace hiwi {


FileNameWidget::FileNameWidget(QWidget *parent, const QString &filter) :
    QWidget(parent),
    _filter(filter)
{
    _ui.setupUi(this);
}


void FileNameWidget::setFileName(const QString &newFileName)
{
    if (QFile::exists(newFileName))
        _ui.leFileName->setText(newFileName);

    emit fileNameChanged(newFileName);
}


QString FileNameWidget::fileName() const
{
    return _ui.leFileName->text();
}


void FileNameWidget::on_tbGetFileName_clicked()
{
    const QString newFileName =
        QFileDialog::getOpenFileName(0, tr("Choose file"), "", _filter);

    if (!newFileName.isEmpty() && QFile::exists(newFileName)) {
        _ui.leFileName->setText(newFileName);
        emit fileNameChanged(newFileName);
    }
}


} // namespace hiwi
