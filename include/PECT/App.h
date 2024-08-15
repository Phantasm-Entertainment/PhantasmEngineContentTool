#ifndef PECT_APP_H_
#define PECT_APP_H_

#include <wx/wx.h>

#include "PECT/MainUI.h"

namespace PECT
{
    class App : public wxApp
    {
    public:
        bool OnInit() override;
    };
}

#endif