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

    
    /**
     * Returns whether the form is completed and the next-button can be
     * displayed.
     */
    virtual bool isComplete() const;


    /**
     * Stores all entered data in the database. Assures that all provided files
     * really exist.
     */
    virtual bool validatePage();


protected slots:
    /**
     * Reloads the participants information from the database.
     */
    void on_tbReload_clicked();



private:
    Ui::DBSetupSamplesWidget _ui;
};


} // namespace hiwi


#endif
