//
// DBSetupParticipantsWidget.h
// $Id$
//


#ifndef __DBSETUPPARTICIPANTSWIDGET_H__
#define __DBSETUPPARTICIPANTSWIDGET_H__


#include <QWizardPage>
#include "ui_DBSetupParticipantsWidget.h"


namespace hiwi {


class DBSetupParticipantsWidget : public QWizardPage
{
    Q_OBJECT

public:
    DBSetupParticipantsWidget(QWidget *parent);


    virtual bool isComplete() const;


protected slots:
    void on_tbAddParticipant_clicked();


    void on_tbRemoveSelected_clicked();


private:
    Ui::DBSetupParticipantsWidget _ui;
};


} // namespace hiwi


#endif
