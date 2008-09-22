//
// FileNameWidget.h
// $Id$
//


#ifndef __FILENAMEWIDGET_H__
#define __FILENAMEWIDGET_H__


#include "ui_FileNameWidget.h"


namespace hiwi {


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


} // namespace hiwi


#endif
