//
// DBSetupSamplesWidget.h
// $Id$
//


#ifndef __DBSETUPSAMPLESWIDGET_H__
#define __DBSETUPSAMPLESWIDGET_H__


#include <QWizardPage>
#include "ui_DBSetupSamplesWidget.h"


// Forward declaration
class QSqlRecord;


namespace hiwi {


class DBSetupSamplesWidget : public QWizardPage
{
    Q_OBJECT

public:
    DBSetupSamplesWidget(QWidget *parent);

    
    virtual bool isComplete() const;


    virtual bool validatePage();


protected slots:
    void on_tbReload_clicked();



private:
    Ui::DBSetupSamplesWidget _ui;
};


} // namespace hiwi


#endif
