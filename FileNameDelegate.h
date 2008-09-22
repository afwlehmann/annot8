//
// FileNameDelegate.h
// $Id$


#ifndef __FILENAMEDELEGATE_H__
#define __FILENAMEDELEGATE_H__


#include <QItemDelegate>


namespace hiwi {


class FileNameDelegate : public QItemDelegate
{
public:
    FileNameDelegate(QObject *parent = 0, const QString &filter = QString());


    virtual QWidget *createEditor(QWidget *parent,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const;


    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;


    virtual void setModelData(QWidget *editor, QAbstractItemModel *model,
                              const QModelIndex &index) const;


private:
    const QString   _filter;
};


} // namespace hiwi


#endif
