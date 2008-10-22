//
// FileNameDelegate.cpp
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


#include "FileNameDelegate.h"
#include "FileNameWidget.h"


namespace annot8 {

FileNameDelegate::FileNameDelegate(QObject *parent, const QString &filter) :
    QItemDelegate(parent),
    _filter(filter)
{
}


QWidget* FileNameDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex&) const
{
    FileNameWidget *fw = new FileNameWidget(parent, _filter);
    fw->setGeometry(option.rect);
    fw->setFont(option.font);

    return fw;
}


void FileNameDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
{
    FileNameWidget *fw = static_cast<FileNameWidget *>(editor);
    fw->setFileName(index.model()->data(index).toString());
}


void FileNameDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
    FileNameWidget *fw = static_cast<FileNameWidget *>(editor);
    model->setData(index, fw->fileName());
}


} // namespace annot8
