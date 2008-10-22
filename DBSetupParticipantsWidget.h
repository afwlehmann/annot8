//
// DBSetupParticipantsWidget.h
// $Id$
//
// Copyright 2008 by Alexander Lehmann <lehmanna@in.tum.de>
//
// This file is part of annot8.
// 
// annot8 is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
// 
// annot8 is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License along with
// annot8.  If not, see <http://www.gnu.org/licenses/>.
//


#ifndef __DBSETUPPARTICIPANTSWIDGET_H__
#define __DBSETUPPARTICIPANTSWIDGET_H__


#include <QWizardPage>
#include "ui_DBSetupParticipantsWidget.h"


namespace annot8 {


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


} // namespace annot8


#endif
