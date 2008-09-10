//
// ChooseParticipantDlg.h
// $Id$
//


#ifndef __CHOOSEPARTICIPANTDLG_H__
#define __CHOOSEPARTICIPANTDLG_H__


#include "ui_ChooseParticipantDlg.h"


namespace hiwi {


class ChooseParticipantDlg : public QDialog
{
    // Q_OBJECT isn't needed as long as there are not signals or slots involved.

public:
    ChooseParticipantDlg(QWidget *parent = 0);


    /**
     * Return the ID of the chosen participant or 0 iff none was selected.
     */
    int getParticipantID() const;


private:
    Ui::ChooseParticipantDlg _ui;
};



} // namespace hiwi


#endif
