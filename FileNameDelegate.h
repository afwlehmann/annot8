//
// FileNameDelegate.h
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


#ifndef __FILENAMEDELEGATE_H__
#define __FILENAMEDELEGATE_H__


#include <QItemDelegate>


namespace hiwi {


class FileNameDelegate : public QItemDelegate
{
public:
    FileNameDelegate(QObject *parent = 0, const QString &filter = QString());


    /**
     * See Qt's documentation for detailed explanations.
     */
    virtual QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;


    /**
     * See Qt's documentation for detailed explanations.
     */
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;


    /**
     * See Qt's documentation for detailed explanations.
     */
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
                              const QModelIndex &index) const;


private:
    const QString   _filter;
};


} // namespace hiwi


#endif
