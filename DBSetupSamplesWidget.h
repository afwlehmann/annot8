//
// DBSetupSamplesWidget.h
// $Id$
//


#ifndef __DBSETUPSAMPLESWIDGET_H__
#define __DBSETUPSAMPLESWIDGET_H__


#include <QWizardPage>
#include "ui_DBSetupSamplesWidget.h"


namespace hiwi {


class DBSetupSamplesWidget : public QWizardPage
{
    Q_OBJECT

public:
    DBSetupSamplesWidget(QWidget *parent);


    virtual bool validatePage() const;


    virtual bool isComplete() const;


    virtual void initializePage();


private:
    Ui::DBSetupSamplesWidget _ui;
};


} // namespace hiwi


#endif
