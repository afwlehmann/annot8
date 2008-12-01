//
// FileNameWidget.cpp
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


#include "FileNameWidget.h"
#include <QFileDialog>
#include <QFile>


namespace annot8 {


FileNameWidget::FileNameWidget(QWidget *parent, const QString &filter) :
    QWidget(parent),
    _filter(filter)
{
    _ui.setupUi(this);
    _ui.leFileName->setFrame(false);
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
        QFileDialog::getOpenFileName(
                0, tr("Choose file"), _ui.leFileName->text(), _filter
        );

    if (!newFileName.isEmpty() && QFile::exists(newFileName)) {
        _ui.leFileName->setText(newFileName);
        emit fileNameChanged(newFileName);
    }
}


} // namespace annot8
