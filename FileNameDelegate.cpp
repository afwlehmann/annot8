//
// FileNameDelegate.cpp
// $Id$
//


#include "FileNameDelegate.h"
#include "FileNameWidget.h"


namespace hiwi {

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


} // namespace hiwi
