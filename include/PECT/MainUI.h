#ifndef PECT_MAINUI_H_
#define PECT_MAINUI_H_

#include <charconv>

#include <wx/wx.h>
#include <wx/notebook.h>

#define ID_FILE_NEW       100
#define ID_FILE_OPEN      101
#define ID_FILE_SAVE      102

#define ID_ATLAS_ADDIMAGE 103
#define ID_ATLAS_ADDFONT  104

#include "PECT/ContentFile.h"
#include "PECT/ContentLoader.h"

namespace PECT
{
    class DrawPanel : public wxPanel
    {
    private:
        std::shared_ptr<AtlasPage> m_Page;
    public:
        inline DrawPanel(wxWindow*, std::shared_ptr<AtlasPage>);

        void OnPaint(wxPaintEvent&);
        void OnKeyDown(wxKeyEvent&);
    };

    class MainUI : public wxFrame
    {
    private:
        wxNotebook* m_Notebook;
        std::shared_ptr<ContentFile> m_ContentFile;
    public:
        MainUI();
    private:
        void UpdateAtlasPages();

        void OnMenuNew(wxCommandEvent&);
        void OnMenuOpen(wxCommandEvent&);
        void OnMenuSave(wxCommandEvent&);
        void OnAtlasAddImage(wxCommandEvent&);
        void OnAtlasAddFont(wxCommandEvent&);
    };
}

#endif