#include <fstream>

#include <wx/encconv.h>
#include <wx/fileconf.h>

#include "window_main.h"
#include "application.h"
#include "dialog_about.h"
#include "dialog_batch.h"

#include "cppbg/tlk_v1/TalkTableEntry.h"
#include "cppbg/tlk_v1/TalkTableFile.h"
#include "cppbg/tra/WeiDUModTranslation.h"

using cppbg_tra::WeiDUModTranslation;
using cppbg_tlk_v1::TalkTable;
using cppbg_tlk_v1::TalkTableEntry;
using std::ifstream;
using std::ofstream;
using std::ios_base;

enum
{
	ID_FILE_NEW	= wxID_HIGHEST + 1,
	ID_FILE_OPEN,
	ID_FILE_RELOAD,
	ID_FILE_SAVE,
    ID_FILE_SAVE_AS_UTF8,
    ID_FILE_SAVE_AS_CP1251,

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

    ID_UTILS_TO_TLK,
    ID_UTILS_FROM_TLK,

	ID_EDITOR,

    TIMER_MISTAKE_HIGHLIGHT
};

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
	EVT_STC_CHANGE(ID_EDITOR,   MainWindow::EditorChange)
	EVT_STC_UPDATEUI(ID_EDITOR, MainWindow::EditorStateUpdate)

	EVT_MENU(ID_FILE_NEW, MainWindow::MenuFileNew)
	EVT_MENU(ID_FILE_OPEN, MainWindow::MenuFileOpen)
	EVT_MENU(ID_FILE_RELOAD, MainWindow::MenuFileReload)
	EVT_MENU(ID_FILE_SAVE, MainWindow::MenuFileSave)
    EVT_MENU(ID_FILE_SAVE_AS_UTF8, MainWindow::MenuFileSaveAsUTF8)
    EVT_MENU(ID_FILE_SAVE_AS_CP1251, MainWindow::MenuFileSaveAsCP1251)
	EVT_MENU(wxID_EXIT, MainWindow::MenuFileExit)

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

    EVT_MENU(ID_CHECK_DOCHECK, MainWindow::MenuCheckDoRecheck)
    EVT_MENU(ID_CHECK_GOTOERROR, MainWindow::MenuCheckGotoError)
    EVT_MENU(ID_CHECK_BATCH, MainWindow::MenuCheckBatch)

    EVT_MENU(ID_UTILS_FROM_TLK, MainWindow::MenuUtilsFromTlk)
    EVT_MENU(ID_UTILS_TO_TLK, MainWindow::MenuUtilsToTlk)

    EVT_MENU(wxID_ABOUT, MainWindow::MenuHelpAbout)

	EVT_FIND(wxID_ANY,             MainWindow::EditorFind)
	EVT_FIND_NEXT(wxID_ANY,        MainWindow::EditorFind)
	EVT_FIND_CLOSE(wxID_ANY,       MainWindow::EditorSearchClosed)
	EVT_FIND_REPLACE(wxID_ANY,     MainWindow::EditorReplace)
    EVT_FIND_REPLACE_ALL(wxID_ANY, MainWindow::EditorReplaceAll)

    EVT_TIMER(TIMER_MISTAKE_HIGHLIGHT, MainWindow::DoMistakeHighlight)
wxEND_EVENT_TABLE()

const int STATUSBAR_PANE_CARET = 0;
const int STATUSBAR_PANE_CODEPAGE = 1;
const int STATUSBAR_PANE_ISMODIFIED = 2;
const int STATUSBAR_PANE_ERROR_MESSAGE = 3;
const int STATUSBAR_PANE_FILEPATH = 4;

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
    menuFile->Append(ID_FILE_SAVE, _("&Save changes\tCtrl-S"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_FILE_SAVE_AS_UTF8, _("S&ave as UTF-8 file..."));
    menuFile->Append(ID_FILE_SAVE_AS_CP1251, _("S&ave as CP1251 file..."));
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

	wxMenu *menuCheck = new wxMenu;
    menuCheck->Append(ID_CHECK_DOCHECK, _("&Check content\tCtrl-T"));
    menuCheck->Append(ID_CHECK_GOTOERROR, _("Go to &mistake\tCtrl-G"));
    menuCheck->AppendSeparator();
    m_autoRecheckFlag = menuCheck->Append(ID_CHECK_AUTORECHK, _("Automatic recheck on save"), wxEmptyString, wxITEM_CHECK);
    menuCheck->AppendSeparator();
    menuCheck->Append(ID_CHECK_BATCH, _("&Batch check\tCtrl-B"));

    wxMenu *menuUtils = new wxMenu;
    menuUtils->Append(ID_UTILS_FROM_TLK, _("Import text &from BG:EE style .TLK"));
    menuUtils->Append(ID_UTILS_TO_TLK, _("&Export text of BG:EE style .TLK"));

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, _("&About...\tF1"));

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuEdit, _("&Edit"));
    menuBar->Append(menuSearch, _("&Search"));
    menuBar->Append(menuCheck, _("&Check"));
    menuBar->Append(menuUtils, _("&Utils"));
    menuBar->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar);

    CreateStatusBar(5, wxSTB_DEFAULT_STYLE);

	int status_widths[] = {120, 75, 10, 150, 550};
	SetStatusWidths(5, status_widths);

	SetStatusBarPane(-1);

    //-- Loading configuration

    wxFileConfig config("TRA Check", "Aldark", "tracheck.ini", "tracheck.ini", wxCONFIG_USE_LOCAL_FILE);

    bool bAutoRecheck, bMaximized;
    int iWidth, iHeight;
    wxString defaultCodepage;
    config.Read("AutoRecheckOnSave", &bAutoRecheck, false);
    config.Read("Width", &iWidth, 620);
    config.Read("Height", &iHeight, 440);
    config.Read("Maximized", &bMaximized, false);
    config.Read("DefaultCodepage", &defaultCodepage, "UTF-8");

    wxGetApp().SetDefaultCodepage(defaultCodepage);
    wxGetApp().SetCurrentCodepage(defaultCodepage);

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
    config.Write("DefaultCodepage", wxGetApp().GetDefaultCodepage());
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

	SetStatusText(status, STATUSBAR_PANE_CARET);
}

void MainWindow::UpdateChangesStatus(const bool &ChangesMade)
{
	static bool CurrentState = false;
	if (CurrentState == ChangesMade) return;

	if (ChangesMade) {
		SetStatusText("*", STATUSBAR_PANE_ISMODIFIED);
	} else {
		SetStatusText(wxEmptyString, STATUSBAR_PANE_ISMODIFIED);
	}

	CurrentState = ChangesMade;
}

void MainWindow::UpdateFileStatus()
{
    SetStatusText("", STATUSBAR_PANE_ERROR_MESSAGE);

	if (wxGetApp().GetCurrentFilePath().empty()) {
        SetStatusText(_("File wasn't neither saved nor checked"), STATUSBAR_PANE_FILEPATH);
	} else {
		SetStatusText(_("Current file: ") + wxGetApp().GetCurrentFilePath(), STATUSBAR_PANE_FILEPATH);
    }

    SetStatusText(wxGetApp().GetCurrentCodepage(), STATUSBAR_PANE_CODEPAGE);
}

wxString MainWindow::CreateTemporaryFileWithTextEditorContent()
{
    m_textEditor->SetEOLMode(wxSTC_EOL_CRLF);
    wxString text = m_textEditor->GetTextRaw();

    if (text.empty() && !m_textEditor->GetText().empty()) {
        wxMessageBox(_("Unable to check the file."), _("Warning"), wxOK_DEFAULT | wxOK | wxICON_WARNING, this);

        return "";
    }

    string filename = wxString::Format("test_tra_file#%i.tra", abs(rand()));

    ofstream output(filename.c_str(), ios_base::binary | ios_base::out);
    if (!output.is_open()) {
        wxMessageBox(_("Unable to create temporary file."), _("Error"), wxOK_DEFAULT | wxOK | wxICON_ERROR, this);

        return "";
    } else {
        output.write(text.c_str(), text.length());
        output.close();
    }

    return filename;
}

bool MainWindow::RecheckFile()
{
    const wxString& time = wxDateTime::Now().Format("%x %X");

    string filename = CreateTemporaryFileWithTextEditorContent();

    bool hasNoErrors = true;

    WeiDUModTranslation parser;
    try {
        parser.LoadFromFile(filename.c_str());
        SetStatusText(wxString::Format(_("No errors, entries - %i! Last check on %s"),
            parser.Size(), time.utf8_str()), STATUSBAR_PANE_FILEPATH);

        m_latestTranslationError.reset(0);

        SetStatusText("", STATUSBAR_PANE_ERROR_MESSAGE);
    } catch (TranslationException& e) {
        hasNoErrors = false;

        SetStatusText(wxString::Format(_("%s. Last check on %s"),
            _(e.GetHint().c_str()), time.utf8_str()), STATUSBAR_PANE_FILEPATH);

        m_latestTranslationError.reset(new TranslationException(e));

        if (e.GetIndexOfBadLine() != -1) {
            SetStatusText(wxString::Format(_("Error at %i:%i"), e.GetIndexOfBadLine()+1,
                e.GetIndexOfBadCharacterInLine()), STATUSBAR_PANE_ERROR_MESSAGE);

            GoToMistake();
        } else {
            SetStatusText("", STATUSBAR_PANE_ERROR_MESSAGE);
        }
    }
    
    remove(filename.c_str());

    wxGetApp().SetFileChecked(true);

    return hasNoErrors;
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

bool MainWindow::ReadFromFile(const wxString& path)
{
    wxConvAuto::SetFallbackEncoding(wxFONTENCODING_CP1251);

    m_textEditor->LoadFile(path);
    m_textEditor->GotoPos(0);

    wxGetApp().SetCurrentCodepage(wxGetApp().GetDefaultCodepage());

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
	ReadFromFile(wxGetApp().GetCurrentFilePath());

	MainWindow_UpdateStatus(this, false);
    wxGetApp().SetFileModified(false);
    wxGetApp().SetFileChecked(false);
}

void MainWindow::MenuFileReload(wxCommandEvent &event)
{
    if (!wxFile::Exists(wxGetApp().GetCurrentFilePath())) return;

	ReadFromFile(wxGetApp().GetCurrentFilePath());

    MainWindow_UpdateStatus(this, false);
    wxGetApp().SetFileModified(false);
    wxGetApp().SetFileChecked(false);
}

void MainWindow::MenuFileSave(wxCommandEvent &event)
{
	if (wxGetApp().GetCurrentFilePath().empty()) {
        MenuFileSaveAsUTF8(event);
	} else {
        WriteToFile(wxGetApp().GetCurrentFilePath(), wxGetApp().GetCurrentCodepage());

		UpdateChangesStatus(false);
		UpdateCaretStatus();
        wxGetApp().SetFileModified(false);

        if (m_autoRecheckFlag->IsChecked()) {
            RecheckFile();
        }
	}
}

void MainWindow::MenuFileSaveAsUTF8(wxCommandEvent &event)
{
    SaveAs("UTF-8");
}
void MainWindow::MenuFileSaveAsCP1251(wxCommandEvent &event)
{
    SaveAs("CP1251");
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

    WeiDUModTranslation parser;

    int count = paths.Count();
    auto_ptr<wxProgressDialog> dlgProgress(new wxProgressDialog(_("Batch checking"),
        _T("Initializing..."), count, this,
        wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME));
    dlgProgress->Show();

    for (int i = 0; i < count; ++i) {
        dlgProgress->Update(i, wxString::Format(_("File: %s"), paths[i]));

        try {
            parser.LoadFromFile(static_cast<const char *>(paths[i].c_str()));
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
    int splashRawImageDataSize;
    auto_ptr<char> splashRawImageData(::wxLoadUserResource(L"SPLASH_ABOUT", L"RC_DATA", &splashRawImageDataSize)); //?
    wxMemoryInputStream splashRawImageStream(splashRawImageData.get(),splashRawImageDataSize);
    wxBitmap splash_image(wxImage(splashRawImageStream, wxBITMAP_TYPE_PNG, -1), -1);

    auto_ptr<AboutDialog> wndAbout(new AboutDialog(this, splash_image));
    wndAbout->ShowModal();
}

void MainWindow::WriteToFile(const wxString& path, const wxString& encoding)
{
    m_textEditor->SetEOLMode(wxSTC_EOL_CRLF);
    string text = m_textEditor->GetTextRaw();

    if (encoding == "UTF-8") {
        // Do nothing
    } else if (encoding == "CP1251") {
        text = wxString::FromUTF8(text.c_str());

        if (text.empty() && !m_textEditor->GetText().empty()) {
            wxMessageBox(_("Unable to convert text to CP1251. File wasn't saved!"), _("Error"), wxOK_DEFAULT | wxOK | wxICON_ERROR, this);

            return;
        }
    }

    ofstream out(static_cast<const char*>(path.c_str()), ios_base::binary | ios_base::out);

    if (!out.good()) {
        return;
    }

    out.write(text.c_str(), text.length());
    out.close();
}

void MainWindow::SaveAs(const wxString& codepage)
{
    wxFileDialog FileDialog(this, _("Save translation file in ") + codepage + _(" codepage"), "", "", _("WeiDU TRA (*.tra)|*.tra|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (FileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    wxGetApp().SetCurrentCodepage(codepage);

    wxGetApp().UpdateCurrentFilePath(FileDialog.GetPath());
    WriteToFile(wxGetApp().GetCurrentFilePath(), codepage);

    MainWindow_UpdateStatus(this, false);
    wxGetApp().SetFileModified(false);

    if (m_autoRecheckFlag->IsChecked()) {
        RecheckFile();
    }
}

string EscapeTranslationText(const string& text)
{
    string boundaryString;

    bool hasTilda = text.find('~') != text.npos,
        hasPercent = text.find('%') != text.npos,
        hasQuote = text.find('"') != text.npos;

    if (!hasTilda) {
        boundaryString = '~';
    } else if (!hasQuote) {
        boundaryString = '"';
    } else if (!hasPercent) {
        boundaryString = '%';
    } else {
        boundaryString = "~~~~~";
    }

    return boundaryString + text + boundaryString;
}

wxString TLKEntryToString(const TalkTableEntry& entry, const size_t& index)
{
    wxString result = wxString::Format("@%i = %s", index, EscapeTranslationText(entry.GetText()));

    if (!entry.GetSoundResRef().empty()) {
        result += " [" + entry.GetSoundResRef() + "]";
    }

    return result;
}

void MainWindow::MenuUtilsToTlk(wxCommandEvent &event)
{
    // TODO: Give other options than just BG:EE

    wxFileDialog FileDialog(this, _("Export translation to .TLK"), "", "", _("Talktable file (*.tlk)|*.tlk|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (FileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    TalkTable dialogTlk;

    if (!RecheckFile()) {
        // User must fix errors before updating dialog.tlk

        return;
    }

    string filename = CreateTemporaryFileWithTextEditorContent();

    WeiDUModTranslation traFile;
    traFile.LoadFromFile(filename.c_str());

    remove(filename.c_str());

    for (WeiDUModTranslation::iterator currentEntry = traFile.Begin(), entryEnd = traFile.End(); currentEntry != entryEnd; ++currentEntry) {
        int index = currentEntry->first;

        TalkTableEntry newEntry(7, 0, 0, currentEntry->second->GetMainSound(), currentEntry->second->GetMainText());

        dialogTlk.GetItems().push_back(newEntry);
    }

    dialogTlk.SaveToFile(FileDialog.GetPath());
}

void MainWindow::MenuUtilsFromTlk(wxCommandEvent &event)
{
    // TODO: Give other options than just BG:EE

    wxFileDialog FileDialog(this, _("Import translation from .TLK"), "", "", _("Talktable file (*.tlk)|*.tlk|All files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (FileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    m_textEditor->ClearAll();

    TalkTable dialogTlk;
    dialogTlk.LoadFromFile(FileDialog.GetPath());

    for (size_t currentEntryIndex = 0, entryEnd = dialogTlk.GetItems().size(); currentEntryIndex != entryEnd; ++currentEntryIndex) {
        wxString oneLine = TLKEntryToString(dialogTlk.GetItems()[currentEntryIndex], currentEntryIndex) + '\n';
        oneLine = wxString::FromUTF8(oneLine.c_str());

        m_textEditor->AddText(oneLine);
    }
}
