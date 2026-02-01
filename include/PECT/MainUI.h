#ifndef PECT_MAINUI_H_
#define PECT_MAINUI_H_

#include <charconv>

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/notebook.h>
#include <wx/sizer.h>

#include "PECT/ContentFile.h"
#include "PECT/ContentLoader.h"

#define ID_FILE_NEW           100
#define ID_FILE_OPEN          101
#define ID_FILE_SAVE          102

#define ID_ATLAS_ADDIMAGE     103
#define ID_ATLAS_ADDFONT      104

#define ID_TEXTURELIST        105
#define ID_FONTLIST           106
#define ID_TEXTURELIST_REMOVE 107
#define ID_FONTLIST_REMOVE    108

namespace PECT
{
    class DrawPanel : public wxPanel
    {
    private:
        ContentFile& m_ContentFile;
        AtlasInt m_AtlasPageIndex;
    public:
        DrawPanel(wxWindow*, ContentFile&, AtlasInt) noexcept;

        void OnPaint(wxPaintEvent&) noexcept;
        void OnKeyDown(wxKeyEvent&) noexcept;
    };

    class MainUI : public wxFrame
    {
    private:
        wxDataViewListCtrl* m_TextureList;
        wxDataViewListCtrl* m_FontList;
        wxNotebook* m_Notebook;
        ContentFile m_ContentFile;
    public:
        MainUI();
    private:
        void UpdateAtlasPages();

        void OnMenuNew(wxCommandEvent&);
        void OnMenuOpen(wxCommandEvent&);
        void OnMenuSave(wxCommandEvent&);
        void OnAtlasAddImage(wxCommandEvent&);
        void OnAtlasAddFont(wxCommandEvent&);
        void OnTextureListContext(wxDataViewEvent&);
        void OnFontListContext(wxDataViewEvent&);
        void OnContextRemoveTexture(wxCommandEvent&);
        void OnContextRemoveFont(wxCommandEvent&);
    };
}

#endif