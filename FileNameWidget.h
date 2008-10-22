//
// FileNameWidget.h
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


#ifndef __FILENAMEWIDGET_H__
#define __FILENAMEWIDGET_H__


#include "ui_FileNameWidget.h"


namespace annot8 {


class FileNameWidget : public QWidget
{
    Q_OBJECT

public:
    FileNameWidget(QWidget *parent = 0, const QString &filter = QString());


    void setFileName(const QString &newFileName);


    QString fileName() const;


signals:
    void fileNameChanged(const QString &);


protected slots:
    virtual void on_tbGetFileName_clicked();


private:
    Ui::FileNameWidget _ui;
    const QString        _filter;
};


} // namespace annot8


#endif
