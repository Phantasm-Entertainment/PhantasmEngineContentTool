#include "PECT/MainUI.h"

namespace PECT
{
    MainUI::MainUI() : wxFrame(NULL, wxID_ANY, "Phantasm Engine Content Tool", wxDefaultPosition, wxSize(1280, 720))
    {
        CreateStatusBar();
        SetStatusText("Ready.");

        wxMenuBar* menuBar = new wxMenuBar;

        wxMenu* fileMenu = new wxMenu;
        fileMenu->Append(ID_FILE_NEW, _T("&New"));
        fileMenu->Append(ID_FILE_OPEN, _T("&Open"));
        fileMenu->Append(ID_FILE_SAVE, _T("&Save"));

        wxMenu* atlasMenu = new wxMenu;
        atlasMenu->Append(ID_ATLAS_ADDIMAGE, _T("&Add image"));
        atlasMenu->Append(ID_ATLAS_ADDFONT, _T("&Add font"));
        
        menuBar->Append(fileMenu, _T("&File"));
        menuBar->Append(atlasMenu, _T("&Atlas"));
        SetMenuBar(menuBar);

        m_TextureList = new wxDataViewListCtrl(this, ID_TEXTURELIST, wxDefaultPosition, wxDefaultSize);
        m_TextureList->SetMinSize(wxSize(100, 100));
        wxDataViewTextRenderer* tr = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
        wxDataViewColumn* texturesColumn = new wxDataViewColumn("Textures", tr, 0);
        m_TextureList->AppendColumn(texturesColumn);

        m_FontList = new wxDataViewListCtrl(this, ID_FONTLIST, wxDefaultPosition, wxDefaultSize);
        m_FontList->SetMinSize(wxSize(100, 100));
        tr = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_INERT);
        wxDataViewColumn* fontsColumn = new wxDataViewColumn("Fonts", tr, 0);
        m_FontList->AppendColumn(fontsColumn);
        
        m_Notebook = new wxNotebook(this, wxID_ANY);

        wxFlexGridSizer* sizer = new wxFlexGridSizer(1, 3, 0, 0);
        sizer->AddGrowableCol(2, 1);
        sizer->AddGrowableRow(0, 2);
        sizer->Add(m_TextureList, 1, wxEXPAND | wxSOUTH, 0);
        sizer->Add(m_FontList, 1, wxEXPAND | wxSOUTH, 0);
        sizer->Add(m_Notebook, 1, wxEXPAND | wxALL, 0);
        
        Bind(wxEVT_MENU, &MainUI::OnMenuNew, this, ID_FILE_NEW);
        Bind(wxEVT_MENU, &MainUI::OnMenuOpen, this, ID_FILE_OPEN);
        Bind(wxEVT_MENU, &MainUI::OnMenuSave, this, ID_FILE_SAVE);
        Bind(wxEVT_MENU, &MainUI::OnAtlasAddImage, this, ID_ATLAS_ADDIMAGE);
        Bind(wxEVT_MENU, &MainUI::OnAtlasAddFont, this, ID_ATLAS_ADDFONT);
        Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &MainUI::OnTextureListContext, this, ID_TEXTURELIST);
        Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &MainUI::OnFontListContext, this, ID_FONTLIST);

        SetSizer(sizer);
        CenterOnScreen();
        Maximize();
    }

    void MainUI::UpdateAtlasPages()
    {
        auto pages = m_ContentFile->GetPages();

        for (std::size_t i = 0; i < pages.size(); ++i)
        {
            if (m_Notebook->GetPageCount() == i)
            {
                wxScrolledWindow* scrollWin = new wxScrolledWindow(m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
                scrollWin->SetSizer(new wxBoxSizer(wxVERTICAL));
                scrollWin->SetScrollRate(5, 5);
                scrollWin->Scroll(wxPoint(0, 0));
                DrawPanel* panel = new DrawPanel(scrollWin, pages[i]);
                scrollWin->GetSizer()->Add(panel, 1, wxEXPAND | wxALL, 3);
                m_Notebook->AddPage(scrollWin, "Page " + std::to_string(i + 1));
            }
        }

        m_Notebook->Refresh();
    }

    void MainUI::OnMenuNew(wxCommandEvent&)
    {
        if (m_ContentFile)
        {
            // TODO: save it
        }
        
        m_Notebook->DeleteAllPages();
        m_ContentFile = std::make_shared<ContentFile>();
        UpdateAtlasPages();
        SetStatusText("Created new content file.");
    }

    void MainUI::OnMenuOpen(wxCommandEvent&)
    {
        if (m_ContentFile)
        {
            wxMessageBox("TODO: OnMenuOpen save before open");
            return;
        }

        wxFileDialog dialog(this, _T("Open content file"), wxEmptyString, wxEmptyString, _T("Phantasm Engine Content File (*.pecf)|*.pecf"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (dialog.ShowModal() == wxID_CANCEL)
        {
            return;
        }

        std::shared_ptr<ContentFile> contentFile;

        try
        {
            contentFile = ContentFile::LoadFromFile(dialog.GetPath().ToStdString());
        }
        catch (const std::string& e)
        {
            wxMessageBox("Error: " + e, "PECT", 5L, this);
            return;
        }
        
        m_Notebook->DeleteAllPages();
        m_ContentFile = contentFile;
        m_TextureList->DeleteAllItems();
        m_FontList->DeleteAllItems();

        for (const auto& pages : m_ContentFile->GetPages())
        {
            for (const auto& texture : pages->GetPageTextures())
            {
                if (!texture->IsFont)
                {
                    wxVector<wxVariant> list;
                    list.push_back(texture->Name);
                    m_TextureList->AppendItem(list);
                }
            }
        }

        for (const auto& font : m_ContentFile->GetFontEntries())
        {
            wxVector<wxVariant> list;
            list.push_back(font.Name);
            m_FontList->AppendItem(list);
        }

        UpdateAtlasPages();
    }

    void MainUI::OnMenuSave(wxCommandEvent&)
    {
        if (!m_ContentFile)
        {
            return;
        }

        wxFileDialog dialog(this, _T("Save content file"), wxEmptyString, wxEmptyString, _T("Phantasm Engine Content File (*.pecf)|*.pecf"), wxFD_SAVE);

        if (dialog.ShowModal() == wxID_CANCEL)
        {
            return;
        }

        std::string path = dialog.GetPath().ToStdString();

        try
        {
            m_ContentFile->SaveToFile(path);
            SetStatusText("Saved content file to '" + path + "'");
        }
        catch(const std::string& e)
        {
            wxMessageBox("Error: " + e, "PECT", 5L, this);
        }
    }

    void MainUI::OnAtlasAddImage(wxCommandEvent&)
    {
        if (!m_ContentFile)
        {
            wxMessageBox(_T("Please create a content file first."), _T("PECT"), 5L, this);
            return;
        }

        wxFileDialog dialog(this, _T("Open image file"), wxEmptyString, wxEmptyString, _T("Image file (*.png)|*.png"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (dialog.ShowModal() == wxID_CANCEL)
        {
            return;
        }

        std::string path = dialog.GetPath().ToStdString();
        wxTextEntryDialog textDialog(this, _T("Enter texture name:"), _T("test"));

        if (textDialog.ShowModal() == wxID_CANCEL)
        {
            return;
        }

        // IF PNG

        std::size_t w, h;
        std::shared_ptr<std::uint8_t[]> data = ContentLoader::LoadPNG(path, &w, &h);

        if (w > 2048 || h > 2048)
        {
            wxMessageBox(_T("Image too large."), _T("Error"), 5L, this);
            return;
        }

        m_ContentFile->AddTexture(textDialog.GetValue().ToStdString(), static_cast<std::uint16_t>(w), static_cast<std::uint16_t>(h), data);
        wxVector<wxVariant> list;
        list.push_back(textDialog.GetValue().ToStdString());
        m_TextureList->AppendItem(list);
        UpdateAtlasPages();
    }

    void MainUI::OnAtlasAddFont(wxCommandEvent&)
    {
        if (!m_ContentFile)
        {
            wxMessageBox(_T("Please create a content file first."), _T("PECT"), 5L, this);
            return;
        }

        wxFileDialog dialog(this, _T("Open font file"), wxEmptyString, wxEmptyString, _T("Font file (*.ttf)|*.ttf"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

        if (dialog.ShowModal() == wxID_CANCEL)
        {
            return;
        }

        wxTextEntryDialog textDialog(this, _T("Enter texture name:"), _T("test"));

        if (textDialog.ShowModal() == wxID_CANCEL)
        {
            return;
        }

        wxTextEntryDialog sizeDialog(this, _T("Enter font size:"), _T("test"));

        if (sizeDialog.ShowModal() == wxID_CANCEL)
        {
            return;
        }

        std::string sizeText = sizeDialog.GetValue().ToStdString();
        std::uint16_t size;
        auto [ptr, ec] = std::from_chars(sizeText.data(), sizeText.data() + sizeText.size(), size);

        if (ec != std::errc())
        {
            wxMessageBox(_T("Invalid font size."), _T("Error"), 5L, this);
            return;
        }

        std::shared_ptr<FontData> fontData;

        try
        {
            fontData = ContentLoader::LoadFont(dialog.GetPath().ToStdString(), size);
        }
        catch(const std::string& e)
        {
            wxMessageBox("Error when loading font: " + e, _T("Error"), 5L, this);
            return;
        }
        
        m_ContentFile->AddFont(textDialog.GetValue().ToStdString(), fontData);
        wxVector<wxVariant> list;
        list.push_back(textDialog.GetValue().ToStdString());
        m_FontList->AppendItem(list);
        UpdateAtlasPages();
    }

    void MainUI::OnTextureListContext(wxDataViewEvent& e)
    {
        wxMenu menu;
        menu.Append(ID_TEXTURELIST_REMOVE, _T("Remove"));
        menu.Bind(wxEVT_MENU, &MainUI::OnContextRemoveTexture, this);
        PopupMenu(&menu);
    }

    void MainUI::OnFontListContext(wxDataViewEvent& e)
    {
        wxMenu menu;
        menu.Append(ID_FONTLIST_REMOVE, _T("Remove"));
        menu.Bind(wxEVT_MENU, &MainUI::OnContextRemoveFont, this);
        PopupMenu(&menu);
    }

    void MainUI::OnContextRemoveTexture(wxCommandEvent& e)
    {
        int row = m_TextureList->GetSelectedRow();

        if (row == wxNOT_FOUND)
        {
            return;
        }

        wxVariant variant;
        m_TextureList->GetValue(variant, row, 0);

        if (!m_ContentFile->RemoveTexture(variant.GetString().ToStdString()))
        {
            wxMessageBox("Couldn't delete that item.");
            return;
        }

        m_TextureList->DeleteItem(row);
        m_Notebook->Refresh();
    }

    void MainUI::OnContextRemoveFont(wxCommandEvent& e)
    {
        int row = m_FontList->GetSelectedRow();

        if (row == wxNOT_FOUND)
        {
            return;
        }

        wxVariant variant;
        m_FontList->GetValue(variant, row, 0);

        if (!m_ContentFile->RemoveFont(variant.GetString().ToStdString()))
        {
            wxMessageBox("Couldn't delete that item.");
            return;
        }

        m_FontList->DeleteItem(row);
        m_Notebook->Refresh();
    }

    DrawPanel::DrawPanel(wxWindow* w, std::shared_ptr<AtlasPage> p) :
    wxPanel(w, wxID_ANY, wxDefaultPosition, wxSize(4096, 4096)), m_Page(p)
    {
        Bind(wxEVT_PAINT, &DrawPanel::OnPaint, this);
        SetMinSize(wxSize(4096, 4096));
        SetDoubleBuffered(true);
    }

    void DrawPanel::OnPaint(wxPaintEvent&)
    {
        wxPaintDC dc(this);
        dc.SetBrush(wxBrush(wxColour(255, 0, 255)));
        dc.DrawRectangle(0, 0, m_Page->GetWidth(), m_Page->GetHeight());

        auto& textures = m_Page->GetPageTextures();

        for (auto& texture : textures)
        {
            if (texture->Width != 0 && texture->Height != 0)
            {
                dc.DrawBitmap(texture->Bitmap, texture->X, texture->Y);
            }
        }
    }
}