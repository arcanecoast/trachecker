#include <GUI/DialogBatch.h>
#include <GUI/WindowMain.h>
#include <GUI/Application.h>

enum 
{
    ID_OPEN = 2000,
    ID_CANCEL,

    ID_FILES_LIST
};

wxBEGIN_EVENT_TABLE(BatchDialog, wxDialog)
    EVT_BUTTON(ID_OPEN,                    BatchDialog::OnOpenClick)
    EVT_BUTTON(ID_CANCEL,                  BatchDialog::OnCloseClick)
    EVT_LIST_ITEM_ACTIVATED(ID_FILES_LIST, BatchDialog::OnListDoubleClick)
wxEND_EVENT_TABLE()

BatchDialog::BatchDialog(MainWindow *parent, const wxString& directory) :
    wxDialog(parent, wxID_ANY, "Batch checking [" + directory + "]",
        wxDefaultPosition, wxSize(600, 400),
        wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU | wxRESIZE_BORDER),
    m_directory(directory)
{
    m_listFiles = new wxListCtrl(this, ID_FILES_LIST, wxDefaultPosition, wxDefaultSize, wxLC_VRULES | wxLC_HRULES | wxLC_SINGLE_SEL | wxLC_REPORT);
    m_listFiles->InsertColumn(0, _("Path"), wxLIST_FORMAT_LEFT, 170);
    m_listFiles->InsertColumn(1, _("Error type"), wxLIST_FORMAT_LEFT, 275);
    m_listFiles->InsertColumn(2, _("Anchor"), wxLIST_FORMAT_LEFT, 145);

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(m_listFiles, 1, wxEXPAND | wxALL, 2);

    wxBoxSizer *buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(new wxButton(this, ID_OPEN, _("Open..."), wxDefaultPosition, wxSize(100, 25)), 0, wxALIGN_RIGHT | wxRIGHT, 3);
    buttonsSizer->Add(new wxButton(this, ID_CANCEL, _("Close"), wxDefaultPosition, wxSize(100, 25)), 0, wxALIGN_RIGHT | wxRIGHT, 2);
    mainSizer->Add(buttonsSizer, 0, wxALIGN_RIGHT | wxALL, 2);

    SetSizer(mainSizer);
}

void BatchDialog::AddFileInfo(const wxString& Path, const wxString& ErrorType, const wxString& Anchor)
{
    long itm = m_listFiles->InsertItem(0, Path);
    m_listFiles->SetItem(itm, 1, ErrorType);
    m_listFiles->SetItem(itm, 2, Anchor);
}

void BatchDialog::OpenFile(const wxString& Path)
{
    MainWindow *wndMain = dynamic_cast<MainWindow *>(this->GetParent());

    if (wxGetApp().IsFileModified()) {
        if (wxMessageBox(_("Current file has been modified. Your changes will be lost!\nDo you want to continue?"),
            _("Warning"), wxCENTRE | wxYES_NO | wxICON_WARNING, wndMain) == wxNO) {
            return;
        }
    }

    wndMain->SetFocus();

    wxGetApp().UpdateCurrentFilePath(m_directory + '\\' + Path);
    wndMain->ReadContentFromFile(wxGetApp().GetCurrentFilePath());

    MainWindow_UpdateStatus(wndMain, false);

    wndMain->RecheckFile();
}

void BatchDialog::ClearFilesList()
{
    m_listFiles->DeleteAllItems();
}

void BatchDialog::OnCloseClick(wxCommandEvent& event)
{
    Close();
}

void BatchDialog::OnOpenClick(wxCommandEvent& event)
{
    long item = m_listFiles->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1) {
        OpenFile(m_listFiles->GetItemText(item, 0));
    }
}

void BatchDialog::OnListDoubleClick(wxListEvent& event)
{
    long item = m_listFiles->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (item != -1) {
        OpenFile(m_listFiles->GetItemText(item, 0));
    }
}