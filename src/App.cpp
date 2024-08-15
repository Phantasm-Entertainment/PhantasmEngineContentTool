#include "PECT/App.h"

namespace PECT
{
    bool App::OnInit()
    {
        MainUI* mainui = new MainUI;
        mainui->Show();
        return true;
    }
}

wxIMPLEMENT_APP(PECT::App);