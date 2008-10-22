//
// DBSetupSamplesWidget.h
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


#ifndef __DBSETUPSAMPLESWIDGET_H__
#define __DBSETUPSAMPLESWIDGET_H__


#include <QWizardPage>
#include "ui_DBSetupSamplesWidget.h"


// Forward declaration
class QSqlRecord;


namespace annot8 {


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


} // namespace annot8


#endif
