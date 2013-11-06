#include <fstream>

#include <wx/encconv.h>
#include <wx/fileconf.h>

#include "window_main.h"
#include "application.h"
#include "dialog_about.h"
#include "dialog_batch.h"

#include "binary_code_splash_png.h"

#include "cppbg/tra/HighLevelParser.h"

using cppbg_tra::HighLevelParser;
using std::ifstream;

enum
{
	ID_FILE_NEW	= wxID_HIGHEST + 1,
	ID_FILE_OPEN,
	ID_FILE_RELOAD,
	ID_FILE_SAVE,
	ID_FILE_SAVE_AS,

	ID_EDIT_UNDO,
	ID_EDIT_REDO,
	ID_EDIT_CUT,
	ID_EDIT_COPY,
	ID_EDIT_PASTE,
	ID_EDIT_SELECTALL,

	ID_SEARCH_FIND,
	ID_SEARCH_FINDNEXT,
	ID_SEARCH_FINDPREV,
	ID_SEARCH_REPLACE,

	ID_CHECK_DOCHECK,
	ID_CHECK_GOTOERROR,
	ID_CHECK_AUTORECHK,
	ID_CHECK_BATCH,

	ID_EDITOR,

    TIMER_MISTAKE_HIGHLIGHT
};

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_STC_CHANGE(ID_EDITOR,   MainWindow::EditorChange)
	EVT_STC_UPDATEUI(ID_EDITOR, MainWindow::EditorStateUpdate)

	EVT_MENU(ID_FILE_NEW,     MainWindow::MenuFileNew)
	EVT_MENU(ID_FILE_OPEN,    MainWindow::MenuFileOpen)
	EVT_MENU(ID_FILE_RELOAD,  MainWindow::MenuFileReload)
	EVT_MENU(ID_FILE_SAVE,    MainWindow::MenuFileSave)
	EVT_MENU(ID_FILE_SAVE_AS, MainWindow::MenuFileSaveAs)
	EVT_MENU(wxID_EXIT,       MainWindow::MenuFileExit)

	EVT_MENU(ID_EDIT_UNDO,      MainWindow::MenuEditUndo)
	EVT_MENU(ID_EDIT_REDO,      MainWindow::MenuEditRedo)
	EVT_MENU(ID_EDIT_CUT,       MainWindow::MenuEditCut)
	EVT_MENU(ID_EDIT_COPY,      MainWindow::MenuEditCopy)
	EVT_MENU(ID_EDIT_PASTE,     MainWindow::MenuEditPaste)
	EVT_MENU(ID_EDIT_SELECTALL, MainWindow::MenuEditSelectAll)

	EVT_MENU(ID_SEARCH_FIND,     MainWindow::MenuSearchFind)
	EVT_MENU(ID_SEARCH_FINDNEXT, MainWindow::MenuSearchFindNext)
	EVT_MENU(ID_SEARCH_FINDPREV, MainWindow::MenuSearchFindPrev)
	EVT_MENU(ID_SEARCH_REPLACE,  MainWindow::MenuSearchReplace)

    EVT_MENU(ID_CHECK_DOCHECK,   MainWindow::MenuCheckDoRecheck)
    EVT_MENU(ID_CHECK_GOTOERROR, MainWindow::MenuCheckGotoError)
    EVT_MENU(ID_CHECK_BATCH,     MainWindow::MenuCheckBatch)

    EVT_MENU(wxID_ABOUT, MainWindow::MenuHelpAbout)

	EVT_FIND(wxID_ANY,             MainWindow::EditorFind)
	EVT_FIND_NEXT(wxID_ANY,        MainWindow::EditorFind)
	EVT_FIND_CLOSE(wxID_ANY,       MainWindow::EditorSearchClosed)
	EVT_FIND_REPLACE(wxID_ANY,     MainWindow::EditorReplace)
    EVT_FIND_REPLACE_ALL(wxID_ANY, MainWindow::EditorReplaceAll)

    EVT_TIMER(TIMER_MISTAKE_HIGHLIGHT, MainWindow::DoMistakeHighlight)
wxEND_EVENT_TABLE()

void MainWindow_UpdateStatus(MainWindow* window, const bool &are_changes_made)
{
    window->UpdateChangesStatus(are_changes_made);
    window->UpdateCaretStatus();
    window->UpdateFileStatus();
}

MainWindow::MainWindow() : wxFrame(0, wxID_ANY, _("TRA Checker"), wxDefaultPosition, wxSize(620, 440)),
       m_errorsHighlightingTimer(this, TIMER_MISTAKE_HIGHLIGHT), m_dialogBatch(0),
       m_dialogSearch(0), m_latestTranslationError(0)
{
    wxIcon icon("ICON_APP", wxICON_DEFAULT_TYPE, 16, 16);
    SetIcon(icon);

	wxAcceleratorEntry entries[19];
	entries[0].Set(wxACCEL_CTRL, (int)'O', ID_FILE_OPEN);
	entries[1].Set(wxACCEL_CTRL, (int)'R', ID_FILE_RELOAD);
	entries[2].Set(wxACCEL_CTRL, (int)'S', ID_FILE_SAVE);
	entries[3].Set(wxACCEL_CTRL | wxACCEL_ALT, (int)'S', ID_FILE_SAVE_AS);
	entries[4].Set(wxACCEL_NORMAL, WXK_ESCAPE, wxID_EXIT);
	entries[5].Set(wxACCEL_CTRL, (int)'Z', ID_EDIT_UNDO);
	entries[6].Set(wxACCEL_CTRL, (int)'Y', ID_EDIT_REDO);
	entries[7].Set(wxACCEL_CTRL, (int)'X', ID_EDIT_CUT);
	entries[8].Set(wxACCEL_CTRL, (int)'C', ID_EDIT_COPY);
	entries[9].Set(wxACCEL_CTRL, (int)'V', ID_EDIT_PASTE);
	entries[10].Set(wxACCEL_CTRL, (int)'A', ID_EDIT_SELECTALL);
	entries[11].Set(wxACCEL_CTRL, (int)'F', ID_SEARCH_FIND);
	entries[12].Set(wxACCEL_NORMAL, WXK_F3, ID_SEARCH_FINDNEXT);
	entries[13].Set(wxACCEL_NORMAL, WXK_F2, ID_SEARCH_FINDPREV);
	entries[14].Set(wxACCEL_CTRL, (int)'H', ID_SEARCH_REPLACE);
	entries[15].Set(wxACCEL_CTRL, (int)'T', ID_CHECK_DOCHECK);
	entries[16].Set(wxACCEL_CTRL, (int)'G', ID_CHECK_GOTOERROR);
	entries[17].Set(wxACCEL_CTRL, (int)'B', ID_CHECK_BATCH);
	entries[18].Set(wxACCEL_NORMAL, WXK_F1, wxID_ABOUT);

    wxAcceleratorTable accel(19, entries);

	m_textEditor = new wxStyledTextCtrl(this, ID_EDITOR); 
	m_textEditor->SetWrapMode(wxSTC_WRAP_WORD);
	m_textEditor->StyleSetFontAttr(wxSTC_STYLE_DEFAULT, 10, "Courier New", false, false, false);
	m_textEditor->StyleSetFontAttr(wxSTC_STYLE_LINENUMBER, 10, "Courier New", true, false, false);
	m_textEditor->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColor(0, 196, 0));

	m_textEditor->SetCaretLineVisible(true);
	m_textEditor->SetCaretLineBackground(wxColor(255, 255, 220));

	m_textEditor->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END);
	m_textEditor->SetMarginType(0, wxSTC_MARGIN_NUMBER);
	m_textEditor->SetMarginWidth(0, m_textEditor->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));

	m_textEditor->SetAcceleratorTable(accel);
	SetAcceleratorTable(accel);

	wxBoxSizer *MainSizer = new wxBoxSizer(wxHORIZONTAL);
	MainSizer->Add(m_textEditor, 1, wxEXPAND);
	SetSizer(MainSizer);

	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_FILE_NEW, _("&New"));
	menuFile->AppendSeparator();
	menuFile->Append(ID_FILE_OPEN, _("&Open...\tCtrl-O"));
	menuFile->Append(ID_FILE_RELOAD, _("Re&load\tCtrl-R"));
    menuFile->AppendSeparator();
	menuFile->Append(ID_FILE_SAVE, _("&Save changes\tCtrl-S"));
	menuFile->Append(ID_FILE_SAVE_AS, _("S&ave as...\tCtrl+Alt-S"));
    menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT, _("E&xit"));

	wxMenu *menuEdit = new wxMenu;
	menuEdit->Append(ID_EDIT_UNDO, _("&Undo\tCtrl-Z"));
	menuEdit->Append(ID_EDIT_REDO, _("&Redo\tCtrl-Y"));
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_EDIT_CUT, _("&Cut\tCtrl-X"));
	menuEdit->Append(ID_EDIT_COPY, _("C&opy\tCtrl-C"));
	menuEdit->Append(ID_EDIT_PASTE, _("&Paste\tCtrl-V"));
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_EDIT_SELECTALL, _("&Select all\tCtrl-A"));

	wxMenu *menuSearch = new wxMenu;
	menuSearch->Append(ID_SEARCH_FIND, _("&Find...\tCtrl-F"));
	menuSearch->AppendSeparator();
	menuSearch->Append(ID_SEARCH_FINDNEXT, _("Find &next\tF3"));
	menuSearch->Append(ID_SEARCH_FINDPREV, _("Find &prev\tF2"));
	menuSearch->AppendSeparator();
	menuSearch->Append(ID_SEARCH_REPLACE, _("&Replace\tCtrl-H"));

	wxMenu *menuChecking = new wxMenu;
	menuChecking->Append(ID_CHECK_DOCHECK, _("&Check content\tCtrl-T"));
	menuChecking->Append(ID_CHECK_GOTOERROR, _("Go to &mistake\tCtrl-G"));
	menuChecking->AppendSeparator();
	m_autoRecheckFlag = menuChecking->Append(ID_CHECK_AUTORECHK, _("Automatic recheck on save"), wxEmptyString, wxITEM_CHECK);
	menuChecking->AppendSeparator();
	menuChecking->Append(ID_CHECK_BATCH, _("&Batch checking\tCtrl-B"));

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, _("&About...\tF1"));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuEdit, _("&Edit"));
    menuBar->Append(menuSearch, _("&Search"));
    menuBar->Append(menuChecking, _("&Checking"));
    menuBar->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar);

    CreateStatusBar(4, wxSTB_DEFAULT_STYLE);

	int status_widths[] = {125, 10, 150, 700};
	SetStatusWidths(4, status_widths);

	SetStatusBarPane(-1);

    //-- Loading configuration

    wxFileConfig config("TRA Check", "Aldark", "tracheck.ini", "tracheck.ini", wxCONFIG_USE_LOCAL_FILE);

    bool bAutoRecheck, bMaximized;
    int iWidth, iHeight;
    config.Read("AutoRecheckOnSave", &bAutoRecheck, false);
    config.Read("Width", &iWidth, 620);
    config.Read("Height", &iHeight, 440);
    config.Read("Maximized", &bMaximized, false);

    SetSize(iWidth, iHeight);
    Maximize(bMaximized);
    m_autoRecheckFlag->Check(bAutoRecheck);

    m_textEditor->StyleSetFontEncoding(wxSTC_STYLE_DEFAULT, wxLocale::GetSystemEncoding()); 

    UpdateCaretStatus();
    UpdateFileStatus();
}

MainWindow::~MainWindow()
{
    //-- Saving configuration
    
    wxFileConfig config("TRA Check", "Aldark", "tracheck.ini", "tracheck.ini", wxCONFIG_USE_LOCAL_FILE);
    config.Write("AutoRecheckOnSave", m_autoRecheckFlag->IsChecked());
    if (!IsMaximized()) {
        config.Write("Width", GetSize().GetWidth());
        config.Write("Height", GetSize().GetHeight());
    }
    config.Write("Maximized", IsMaximized());
}

void MainWindow::UpdateCaretStatus()
{
	wxString status;

	int line_start = m_textEditor->GetLineEndPosition(m_textEditor->GetCurrentLine() - 1);
	line_start = (line_start == -1) ? -1 : line_start + 1;

    status.Printf(_("[%i:%i]: %i"),
		m_textEditor->GetCurrentLine() + 1,
		m_textEditor->GetCurrentPos() - line_start,
		m_textEditor->GetTextLength());

	SetStatusText(status, 0);
}

void MainWindow::UpdateChangesStatus(const bool &ChangesMade)
{
	static bool CurrentState = false;
	if (CurrentState == ChangesMade) return;

	if (ChangesMade) {
		SetStatusText("*", 1);
	} else {
		SetStatusText(wxEmptyString, 1);
	}

	CurrentState = ChangesMade;
}

void MainWindow::UpdateFileStatus()
{
	if (wxGetApp().GetCurrentFilePath().empty()) {
		SetStatusText(_("File wasn't neither saved nor checked"), 3);
        SetStatusText("", 2);
	} else {
		SetStatusText(_("Current file: ") + wxGetApp().GetCurrentFilePath(), 3);
        SetStatusText("", 2);
	}
}

void MainWindow::RecheckFile()
{
    wxDateTime now(wxDateTime::Now());
    const wxString& time = now.Format("%x %X");

    const wxString& text = m_textEditor->GetText();
    const wxString& filename = wxString::Format("test_tra_file#%i.tra", abs(rand()));

    m_textEditor->SaveFile(filename);

    HighLevelParser parser;
    try {
        parser.LoadFromFile((const char *)filename.c_str());
        SetStatusText(wxString::Format(_("No errors, entries - %i! Last check on %s"),
            parser.Size(), time.utf8_str()), 3);

        m_latestTranslationError.reset(0);

        SetStatusText("", 2);
    } catch (TranslationException& e) {
        SetStatusText(wxString::Format(_("%s. Last check on %s"),
            _(e.GetHint().c_str()), time.utf8_str()), 3);

        m_latestTranslationError.reset(new TranslationException(e));

        if (e.GetIndexOfBadLine() != -1) {
            SetStatusText(wxString::Format(_("Error at %i:%i"), e.GetIndexOfBadLine()+1,
                e.GetIndexOfBadCharacterInLine()), 2);

            GoToMistake();
        } else {
            SetStatusText("", 2);
        }
    }
    
    remove(filename);

    wxGetApp().SetFileChecked(true);
}

void MainWindow::GoToMistake()
{
    if (m_latestTranslationError.get()) {
        if (m_errorsHighlightingTimer.IsRunning()) m_errorsHighlightingTimer.Stop();

        m_textEditor->GotoLine(m_latestTranslationError->GetIndexOfBadLine());
        m_textEditor->EnsureVisible(m_latestTranslationError->GetIndexOfBadLine());
        m_textEditor->Home();
        for (int i = m_latestTranslationError->GetIndexOfBadCharacterInLine() - 1; i > 0; --i) {
            m_textEditor->CharRight();
        }
        m_textEditor->SetCaretLineBackground(wxColor(255, 220, 220));

        m_errorsHighlightingTimer.Start(500, false);
    }
}

void MainWindow::DoMistakeHighlight(wxTimerEvent &event)
{
    static int runsCount = 0;

    if (runsCount & 1) {
        m_textEditor->SetCaretLineBackground(wxColor(255, 220, 220));
    } else {
        m_textEditor->SetCaretLineBackground(wxColor(255, 255, 220));
    }

    if (runsCount == 4) {
        event.GetTimer().Stop();
        runsCount = 0;
    } else {
        ++runsCount;
    }
}

bool MainWindow::LoadFile(const wxString& path)
{
    string line, text;

    ifstream input(reinterpret_cast<const wchar_t*>(path.wc_str()));
    if (input.is_open()) {
        while (getline(input, line)) {
            text += line + "\n";
        }

        input.close();
    } else {
        wxMessageBox(_("Unable to load request file."), _("Error"), wxOK_DEFAULT | wxOK | wxICON_ERROR, this);
        return false;
    }

    m_textEditor->SetText(text);
    m_textEditor->GotoPos(0);

    return true;
}

// Search/Replace dialog releated code
//

void MainWindow::EditorFind(wxFindDialogEvent& event)
{
	int offset;

	if (event.GetFlags() & wxFR_DOWN) {
		m_textEditor->SetAnchor(m_textEditor->GetSelectionEnd());
		m_textEditor->SearchAnchor();
		offset = m_textEditor->SearchNext(event.GetFlags(), event.GetFindString());
	} else {
		m_textEditor->SetAnchor(m_textEditor->GetSelectionStart());
		m_textEditor->SearchAnchor();
		offset = m_textEditor->SearchPrev(event.GetFlags(), event.GetFindString());
	}

	if (offset == -1) {
		wxMessageBox(_("No matches found!"), _("Search finished"), wxOK_DEFAULT | wxOK, event.GetDialog());
	} else {
        m_textEditor->GotoPos(offset);

        for (int i = m_currentSearchData.GetFindString().Length(); i > 0; --i) {
            m_textEditor->CharRightExtend();
        }
	}
}

void MainWindow::EditorReplace(wxFindDialogEvent& event)
{
	int offset;

	m_textEditor->SetAnchor(m_textEditor->GetSelectionStart());
	m_textEditor->SearchAnchor();
	offset = m_textEditor->SearchNext(event.GetFlags(), event.GetFindString());

	if (offset == -1) {
		wxMessageBox(_("No matches found!"), _("Search finished"), wxOK_DEFAULT | wxOK, event.GetDialog());
	} else {
		m_textEditor->GotoPos(offset);
        for (int i = m_currentSearchData.GetFindString().Length(); i > 0; --i) m_textEditor->CharRightExtend();
        m_textEditor->ReplaceSelection(event.GetReplaceString());
        m_textEditor->GotoPos(offset);
        for (int i = event.GetReplaceString().Length(); i > 0; --i) m_textEditor->CharRightExtend();
	}
}

void MainWindow::EditorReplaceAll(wxFindDialogEvent& event)
{
	int offset = 0, count = 0;

	while (offset != -1 || offset >= m_textEditor->GetTextLength()) {
		offset = m_textEditor->FindText(offset, m_textEditor->GetTextLength(), event.GetFindString(), event.GetFlags());

		if (offset != -1) {
            m_textEditor->GotoPos(offset);
            for (int i = m_currentSearchData.GetFindString().Length(); i > 0; --i) {
                m_textEditor->CharRightExtend();
                offset = m_textEditor->GetCurrentPos();
            }

            m_textEditor->ReplaceSelection(event.GetReplaceString());
			++count;
		}
	}

	if (count == 0) {
		wxMessageBox(_("No matches found!"), _("Replace finished"), wxOK_DEFAULT | wxOK, event.GetDialog());
    } else {
		wxMessageBox(wxString::Format(_("Replaced count: %i"), count), _("Replace finished"), wxOK_DEFAULT | wxOK, event.GetDialog());
	}
}

void MainWindow::EditorSearchClosed(wxFindDialogEvent& event)
{
	m_dialogSearch.reset(0);
}

// Main text editor events handling
//

void MainWindow::EditorChange(wxStyledTextEvent &event)
{
	UpdateChangesStatus(true);
}

void MainWindow::EditorStateUpdate(wxStyledTextEvent &event)
{
	UpdateCaretStatus();
}

// Menus commands
//

void MainWindow::MenuFileNew(wxCommandEvent &event)
{
	m_textEditor->ClearAll();

	wxGetApp().UpdateCurrentFilePath(wxEmptyString);

    MainWindow_UpdateStatus(this, false);
    wxGetApp().SetFileModified(false);
}

void MainWindow::MenuFileOpen(wxCommandEvent &event)
{
	wxFileDialog FileDialog(this, _("Open translation file"), "", "", _("WeiDU TRA (*.tra)|*.tra|All files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (FileDialog.ShowModal() == wxID_CANCEL) return;

	wxGetApp().UpdateCurrentFilePath(FileDialog.GetPath());
	LoadFile(wxGetApp().GetCurrentFilePath());

	MainWindow_UpdateStatus(this, false);
    wxGetApp().SetFileModified(false);
    wxGetApp().SetFileChecked(false);
}

void MainWindow::MenuFileReload(wxCommandEvent &event)
{
    if (!wxFile::Exists(wxGetApp().GetCurrentFilePath())) return;

	LoadFile(wxGetApp().GetCurrentFilePath());

    MainWindow_UpdateStatus(this, false);
    wxGetApp().SetFileModified(false);
    wxGetApp().SetFileChecked(false);
}

void MainWindow::MenuFileSave(wxCommandEvent &event)
{
	if (wxGetApp().GetCurrentFilePath().empty()) {
		MenuFileSaveAs(event);
	} else {
		m_textEditor->SaveFile(wxGetApp().GetCurrentFilePath());

		UpdateChangesStatus(false);
		UpdateCaretStatus();
        wxGetApp().SetFileModified(false);

        if (m_autoRecheckFlag->IsChecked()) {
            RecheckFile();
        }
	}
}

void MainWindow::MenuFileSaveAs(wxCommandEvent &event)
{
	wxFileDialog FileDialog(this, _("Save translation file"), "", "", _("WeiDU TRA (*.tra)|*.tra|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (FileDialog.ShowModal() == wxID_CANCEL) return;

	wxGetApp().UpdateCurrentFilePath(FileDialog.GetPath());
	m_textEditor->SaveFile(wxGetApp().GetCurrentFilePath());

    MainWindow_UpdateStatus(this, false);
    wxGetApp().SetFileModified(false);

    if (m_autoRecheckFlag->IsChecked()) RecheckFile();
}

void MainWindow::MenuFileExit(wxCommandEvent &event)
{
	Close();
}

void MainWindow::MenuEditUndo(wxCommandEvent &event)
{
	m_textEditor->Undo();
}

void MainWindow::MenuEditRedo(wxCommandEvent &event)
{
	m_textEditor->Redo();
}

void MainWindow::MenuEditCut(wxCommandEvent &event)
{
	m_textEditor->Cut();
}

void MainWindow::MenuEditCopy(wxCommandEvent &event)
{
	m_textEditor->Copy();
}

void MainWindow::MenuEditPaste(wxCommandEvent &event)
{
	m_textEditor->Paste();
}

void MainWindow::MenuEditSelectAll(wxCommandEvent &event)
{
	m_textEditor->SelectAll();
}

void MainWindow::MenuSearchFind(wxCommandEvent &event)
{
	m_dialogSearch.reset(0);

	if (!m_textEditor->GetSelectedText().empty()) {
		m_currentSearchData.SetFindString(m_textEditor->GetSelectedText());
	}
    
    m_currentSearchData.SetFlags(wxFR_DOWN);

	m_dialogSearch.reset(new wxFindReplaceDialog(this, &m_currentSearchData, _("Search dialog")));
	m_dialogSearch->Show();
}

void MainWindow::MenuSearchFindNext(wxCommandEvent &event)
{
	if (m_currentSearchData.GetFindString().empty()) return;

	m_textEditor->SetAnchor(m_textEditor->GetSelectionEnd());
	m_textEditor->SearchAnchor();
	int offset = m_textEditor->SearchNext(m_currentSearchData.GetFlags(), m_currentSearchData.GetFindString());

	if (offset == -1) {
		wxWindow *wnd = m_dialogSearch.get() ? m_dialogSearch.get() : (wxWindow *)this;
		wxMessageBox(_("No matches found!"), _("Search finished"), wxOK_DEFAULT | wxOK, wnd);
	} else {
		m_textEditor->GotoPos(offset);
        for (int i = m_currentSearchData.GetFindString().Length(); i > 0; --i) m_textEditor->CharRightExtend();
	}
}

void MainWindow::MenuSearchFindPrev(wxCommandEvent &event)
{
	if (m_currentSearchData.GetFindString().empty()) return;

	m_textEditor->SetAnchor(m_textEditor->GetSelectionStart());
	m_textEditor->SearchAnchor();
	int offset = m_textEditor->SearchPrev(m_currentSearchData.GetFlags(), m_currentSearchData.GetFindString());

	if (offset == -1) {
		wxWindow *wnd = m_dialogSearch.get() ? m_dialogSearch.get() : (wxWindow *)this;
		wxMessageBox(_("No matches found!"), _("Search finished"), wxOK_DEFAULT | wxOK, wnd);
	} else {
		m_textEditor->GotoPos(offset);
        for (int i = m_currentSearchData.GetFindString().Length(); i > 0; --i) {
            m_textEditor->CharRightExtend();
        }
	}
}

void MainWindow::MenuSearchReplace(wxCommandEvent &event)
{
	m_dialogSearch.reset(0);

	if (!m_textEditor->GetSelectedText().empty()) {
		m_currentSearchData.SetFindString(m_textEditor->GetSelectedText());
	}

	m_dialogSearch.reset(new wxFindReplaceDialog(this, &m_currentSearchData, _("Replace dialog"), wxFR_REPLACEDIALOG));
	m_dialogSearch->Show();
}

void MainWindow::MenuCheckDoRecheck(wxCommandEvent &event)
{
    RecheckFile();
}

void MainWindow::MenuCheckGotoError(wxCommandEvent &event)
{
    if (wxGetApp().IsFileChecked() == true) {
        GoToMistake();
    } else {
        RecheckFile();
    }
}

void MainWindow::MenuCheckBatch(wxCommandEvent &event)
{
    if (m_dialogBatch.get()) {
        if (m_dialogBatch->IsVisible()) {
            m_dialogBatch->SetFocus();
            return;
        } else {
            m_dialogBatch.reset(0);
        }
    }

    wxFileDialog FileDialog(this, _("Select translation files"), "", "", _("WeiDU TRA (*.tra)|*.tra|All files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
	if (FileDialog.ShowModal() == wxID_CANCEL) return;

    wxArrayString paths, names;
    FileDialog.GetPaths(paths);
    FileDialog.GetFilenames(names);

    m_dialogBatch.reset(new BatchDialog(this, FileDialog.GetDirectory()));

    HighLevelParser parser;

    int count = paths.Count();
    auto_ptr<wxProgressDialog> dlgProgress(new wxProgressDialog(_("Batch checking"),
        _T("Initializing..."), count, this,
        wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME));
    dlgProgress->Show();

    for (int i = 0; i < count; ++i) {
        dlgProgress->Update(i, wxString::Format(_("File: %s"), paths[i]));

        try {
            parser.LoadFromFile((const char *)paths[i].c_str());
            m_dialogBatch->AddFileInfo(names[i], _("No errors!"), "");
        } catch (TranslationException& e) {
            wxString anchor = wxString::Format(_("Error at %i:%i"), e.GetIndexOfBadLine()+1,
                e.GetIndexOfBadCharacterInLine());

            m_dialogBatch->AddFileInfo(names[i], _(e.GetHint()), anchor);
        }
    }

    m_dialogBatch->Show();
}

void MainWindow::MenuHelpAbout(wxCommandEvent &event)
{
    wxMemoryInputStream splash_raw_image(splash_template_png, sizeof(splash_template_png));
    wxBitmap splash_image(wxImage(splash_raw_image, wxBITMAP_TYPE_PNG, -1), -1);

    auto_ptr<AboutDialog> wndAbout(new AboutDialog(this, splash_image));
    wndAbout->ShowModal();
}