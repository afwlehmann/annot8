//
// AboutDlg.h
// $Id$
//


#ifndef __ABOUTDLG_H__
#define __ABOUTDLG_H__


#include "ui_AboutDlg.h"


namespace hiwi {


class AboutDlg : public QDialog
{
public:
    AboutDlg(QWidget *parent = 0) : QDialog(parent)
    {
        _ui.setupUi(this);
        raise();
    }


private:
    Ui::AboutDlg _ui;
};



} // namespace hiwi


#endif
