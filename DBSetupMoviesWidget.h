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


    /**
     * Returns whether the form is completed and the next-button can be
     * displayed.
     */
    virtual bool isComplete() const;


protected slots:
    /**
     * Event-handler for the `Add movie'-button. Adds a movie ;-)
     */
    void on_tbAddMovie_clicked();


    /**
     * Event-handler for the `Remove selected'-button. Removes all selected
     * movies.s
     */
    void on_tbRemoveSelected_clicked();


private:
    Ui::DBSetupMoviesWidget _ui;
};


} // namespace hiwi


#endif
