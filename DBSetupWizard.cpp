//
// DBSetupWizard.cpp
// $Id$
//


#include "DBSetupWizard.h"
#include "DBSetupMoviesWidget.h"
#include "DBSetupParticipantsWidget.h"
#include "DBSetupSamplesWidget.h"


namespace hiwi {


DBSetupWizard::DBSetupWizard(QWidget *parent) : QWizard(parent)
{
    setWindowTitle(tr("Database setup wizard"));

    addPage(new DBSetupParticipantsWidget(this));
    addPage(new DBSetupSamplesWidget(this));
    addPage(new DBSetupMoviesWidget(this));

    setMinimumSize(800, 500);

#ifdef Q_WS_MAC
    setOptions((options() | QWizard::CancelButtonOnLeft)
               & ~QWizard::NoCancelButton);
#endif
}


} // namespace hiwi
