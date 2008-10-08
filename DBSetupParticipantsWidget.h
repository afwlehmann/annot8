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


    /**
     * Returns whether the form is completed and the next-button can be
     * displayed.
     */
    virtual bool isComplete() const;


protected slots:
    /**
     * Event-handler for the `Add participant'-button. Adds a participant ;-)
     */
    void on_tbAddParticipant_clicked();


    /**
     * Event-handler for the `Remove selected'-button. Removes all selected
     * participants.
     */
    void on_tbRemoveSelected_clicked();


private:
    Ui::DBSetupParticipantsWidget _ui;
};


} // namespace hiwi


#endif
