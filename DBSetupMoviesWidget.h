//
// DBSetupMoviesWidget.h
// $Id$
//


#ifndef __DBSETUPMOVIESWIDGET_H__
#define __DBSETUPMOVIESWIDGET_H__


#include <QWizardPage>
#include "ui_DBSetupMoviesWidget.h"


namespace hiwi {


class DBSetupMoviesWidget : public QWizardPage
{
    Q_OBJECT

public:
    DBSetupMoviesWidget(QWidget *parent);


    virtual bool validatePage() const;


    virtual bool isComplete() const;


    virtual void initializePage();


protected slots:
    void on_tbAddMovie_clicked();


    void on_tbRemoveSelected_clicked();


private:
    Ui::DBSetupMoviesWidget _ui;
};


} // namespace hiwi


#endif
