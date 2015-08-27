#include <wx/file.h>
#include <wx/mstream.h>
#include <wx/progdlg.h>

#include <GUI/WindowMain.h>
#include <GUI/Application.h>
#include <GUI/DialogAbout.h>
#include <GUI/DialogBatch.h>
#include <GUI/DialogCompareFolders.h>

#include <cppbg/tlk_v1/TalkTableEntry.h>
#include <cppbg/tlk_v1/TalkTableFile.h>
#include <cppbg/tra/WeiDUModTranslation.h>

#include <Utilities/WeiDUTranslationUtilities.h>
#include <Utilities/TranslationFileActions.h>
#include <cppbg/tra/TranslationException.h>

using cppbg_tra::WeiDUModTranslation;
using cppbg_tra::TranslationException;
using cppbg_tlk_v1::TalkTable;
using cppbg_tlk_v1::TalkTableEntry;

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
	ID_CHECK_AUTORECHECK,
    ID_CHECK_BATCH,

    ID_UTILS_TO_TLK,
    ID_UTILS_FROM_TLK,
	ID_UTILS_COMPARE_FOLDERS,

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
	EVT_MENU(ID_UTILS_COMPARE_FOLDERS, MainWindow::MenuUtilsCompareFolders)

    EVT_MENU(wxID_ABOUT, MainWindow::MenuHelpAbout)

	EVT_FIND(wxID_ANY,             MainWindow::EditorFind)
	EVT_FIND_NEXT(wxID_ANY,        MainWindow::EditorFind)
	EVT_FIND_CLOSE(wxID_ANY,       MainWindow::EditorSearchClosed)
	EVT_FIND_REPLACE(wxID_ANY,     MainWindow::EditorReplace)
    EVT_FIND_REPLACE_ALL(wxID_ANY, MainWindow::EditorReplaceAll)

    EVT_TIMER(TIMER_MISTAKE_HIGHLIGHT, MainWindow::OnTimerHighlightError)
wxEND_EVENT_TABLE()

const int MainWindow::STATUSBAR_PANE_CARET = 0;
const int MainWindow::STATUSBAR_PANE_CODEPAGE = 1;
const int MainWindow::STATUSBAR_PANE_ISMODIFIED = 2;
const int MainWindow::STATUSBAR_PANE_ERROR_MESSAGE = 3;
const int MainWindow::STATUSBAR_PANE_FILEPATH = 4;

const wxColor MainWindow::EDITOR_ACTIVE_LINE_BACKGROUND = wxColor(255, 255, 220);
const wxColor MainWindow::EDITOR_ERROR_LINE_BACKGROUND = wxColor(255, 220, 220);

MainWindow::MainWindow() : wxFrame(0, wxID_ANY, _("TRA Checker"), wxDefaultPosition, wxSize(620, 440)),
       m_timerErrorHighlight(this, TIMER_MISTAKE_HIGHLIGHT), m_dialogBatch(0), m_dialogSearch(0)
{
    InitializeMenubar();
    InitializeAccelerators();

    wxIcon icon("ICON_APP", wxICON_DEFAULT_TYPE, 16, 16);
    this->SetIcon(icon);

	m_textEditor = new wxStyledTextCtrl(this, ID_EDITOR); 
	m_textEditor->SetWrapMode(wxSTC_WRAP_WORD);
	m_textEditor->StyleSetFontAttr(wxSTC_STYLE_DEFAULT, 10, "Courier New", false, false, false);
	m_textEditor->StyleSetFontAttr(wxSTC_STYLE_LINENUMBER, 10, "Courier New", true, false, false);
	m_textEditor->StyleSetForeground(wxSTC_STYLE_LINENUMBER, wxColor(0, 196, 0));

	m_textEditor->SetCaretLineVisible(true);
    m_textEditor->SetCaretLineBackground(MainWindow::EDITOR_ACTIVE_LINE_BACKGROUND);

	m_textEditor->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END);
	m_textEditor->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    m_textEditor->SetMarginWidth(0, m_textEditor->TextWidth(wxSTC_STYLE_LINENUMBER, wxT("_99999")));

    m_textEditor->StyleSetFontEncoding(wxSTC_STYLE_DEFAULT, wxLocale::GetSystemEncoding());

	wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
	mainSizer->Add(m_textEditor, 1, wxEXPAND);
	this->SetSizer(mainSizer);

    // Configure status bar

    this->CreateStatusBar(5, wxSTB_DEFAULT_STYLE);

	int statusWidths[] = {120, 75, 10, 150, 550};
    this->SetStatusWidths(5, statusWidths);

    this->SetStatusBarPane(-1);

    this->RefreshAllStatusBarInfo();
}

MainWindow::~MainWindow()
{
    // Update config with window changes

    wxGetApp().CurrentConfiguration().SetAutoRecheckOnSave(this->GetAutoRecheck());
    wxGetApp().CurrentConfiguration().SetMaximized(this->IsMaximized());

    if (!this->IsMaximized()) {
        wxGetApp().CurrentConfiguration().SetWidth(this->GetSize().GetWidth());
        wxGetApp().CurrentConfiguration().SetHeight(this->GetSize().GetHeight());
    }
}

void MainWindow::RefreshAllStatusBarInfo()
{
    this->RefreshStatusBarChangesMarker();
    this->RefreshStatusBarCaretInfo();
    this->RefreshStatusBarFileInfo();
}

void MainWindow::RefreshStatusBarCaretInfo()
{
	int lineFirstSymbolPos = m_textEditor->GetLineEndPosition(m_textEditor->GetCurrentLine() - 1);
	lineFirstSymbolPos = (lineFirstSymbolPos == -1) ? -1 : lineFirstSymbolPos + 1;

    SetStatusText(wxString::Format(
        _("[%i:%i]: %i"),
        m_textEditor->GetCurrentLine() + 1,
        m_textEditor->GetCurrentPos() - lineFirstSymbolPos,
        m_textEditor->GetTextLength()
    ), STATUSBAR_PANE_CARET);
}

void MainWindow::RefreshStatusBarChangesMarker()
{
    if (CurrentTranslationFileInfo().GetModified()) {
        SetStatusText("*", STATUSBAR_PANE_ISMODIFIED);
    } else {
        SetStatusText(wxEmptyString, STATUSBAR_PANE_ISMODIFIED);
    }
}

void MainWindow::RefreshStatusBarFileInfo()
{
    if (!wxGetApp().LatestTranslationErrorHint().empty()) {
        // Do this, if there is an error in text editor text

        if (wxGetApp().LatestTranslationErrorPosition().GetIndexOfLine() != -1) {
            SetStatusText(wxString::Format(
                _("Error at %i:%i"),
                wxGetApp().LatestTranslationErrorPosition().GetIndexOfLine() + 1,
                wxGetApp().LatestTranslationErrorPosition().GetPlaceInLine()
                ), STATUSBAR_PANE_ERROR_MESSAGE);
        } else {
            SetStatusText("", STATUSBAR_PANE_ERROR_MESSAGE);
        }

        SetStatusText(_(wxGetApp().LatestTranslationErrorHint().c_str()), STATUSBAR_PANE_FILEPATH);
    } else {
        // Do this, if there are no errors in text editor text

        SetStatusText("", STATUSBAR_PANE_ERROR_MESSAGE);

        if (CurrentTranslationFileInfo().GetPath().empty()) {
            SetStatusText(_("File wasn't neither saved nor checked"), STATUSBAR_PANE_FILEPATH);
        } else {
            SetStatusText(_("Current file: ") + CurrentTranslationFileInfo().GetPath(), STATUSBAR_PANE_FILEPATH);
        }
    }

    SetStatusText(CurrentTranslationFileInfo().GetCodepage(), STATUSBAR_PANE_CODEPAGE);
}

bool MainWindow::RecheckCurrentTranslation()
{
    const wxString& time = wxDateTime::Now().Format("%x %X");
    const string temporaryFilePath = TranslationFileActions::CreateTemporaryFileWithTextEditorContent(m_textEditor);

    try {
        WeiDUModTranslation parser;
        parser.LoadFromFile(temporaryFilePath.c_str());

        wxGetApp().LatestTranslationErrorHint() = "";
        wxGetApp().LatestTranslationErrorPosition().Update(-1, -1, -1);

        RefreshAllStatusBarInfo();
        SetStatusText(wxString::Format(
            _("No errors, entries - %i! Last check on %s"),
            parser.Size(),
            time.utf8_str()
        ), STATUSBAR_PANE_FILEPATH);
    } catch (TranslationException& e) {
        wxGetApp().LatestTranslationErrorHint() = e.GetHint();
        wxGetApp().LatestTranslationErrorPosition().Update(e.GetIndexOfBadLine(), e.GetIndexOfBadCharacterInLine(), e.GetOffsetToBadCharacter());

        RefreshAllStatusBarInfo();
        SetStatusText(wxString::Format(
            _("%s. Last check on %s"),
            _(wxGetApp().LatestTranslationErrorHint().c_str()),
            time.utf8_str()
        ), STATUSBAR_PANE_FILEPATH);

        if (e.GetIndexOfBadLine() != -1) {
            MoveCursorToError();
        }
    }
    
    remove(temporaryFilePath.c_str());

    CurrentTranslationFileInfo().SetVerified(true);

    return (wxGetApp().LatestTranslationErrorHint() == "");
}

void MainWindow::MoveCursorToError()
{
    if (!wxGetApp().LatestTranslationErrorHint().empty()) {
        if (m_timerErrorHighlight.IsRunning()) m_timerErrorHighlight.Stop();

        m_textEditor->GotoLine(wxGetApp().LatestTranslationErrorPosition().GetIndexOfLine());
        m_textEditor->EnsureVisible(wxGetApp().LatestTranslationErrorPosition().GetIndexOfLine());
        m_textEditor->Home();
        for (int i = wxGetApp().LatestTranslationErrorPosition().GetPlaceInLine() - 1; i > 0; --i) {
            m_textEditor->CharRight();
        }
        m_textEditor->SetCaretLineBackground(wxColor(255, 220, 220));

        m_timerErrorHighlight.Start(500, false);
    }
}

// Main window events callbacks and private methods
//

void MainWindow::OnTimerHighlightError(wxTimerEvent &event)
{
    static int runsCount = 0;

    if (runsCount & 1) {
        m_textEditor->SetCaretLineBackground(MainWindow::EDITOR_ERROR_LINE_BACKGROUND);
    } else {
        m_textEditor->SetCaretLineBackground(MainWindow::EDITOR_ACTIVE_LINE_BACKGROUND);
    }

    if (runsCount == 4) {
        event.GetTimer().Stop();
        runsCount = 0;
    } else {
        ++runsCount;
    }
}

// Search/Replace dialog related code
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
    CurrentTranslationFileInfo().SetModified(true);

	RefreshStatusBarChangesMarker();
}

void MainWindow::EditorStateUpdate(wxStyledTextEvent &event)
{
	RefreshStatusBarCaretInfo();
}

// Menus commands
//

void MainWindow::MenuFileNew(wxCommandEvent &event)
{
	m_textEditor->ClearAll();

    CurrentTranslationFileInfo().SetPath(wxEmptyString);
    CurrentTranslationFileInfo().SetCodepage(wxGetApp().CurrentConfiguration().GetDefaultCodepage());
    CurrentTranslationFileInfo().SetVerified(false);
    CurrentTranslationFileInfo().SetModified(false);

    RefreshAllStatusBarInfo();
}

void MainWindow::MenuFileOpen(wxCommandEvent &event)
{
	wxFileDialog FileDialog(this, _("Open translation file"), "", "", _("WeiDU TRA (*.tra)|*.tra|All files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (FileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    CurrentTranslationFileInfo().SetPath(FileDialog.GetPath());
    TranslationFileActions::LoadTranslationFromFile(CurrentTranslationFileInfo().GetPath(), m_textEditor);

    CurrentTranslationFileInfo().SetModified(false);
    CurrentTranslationFileInfo().SetVerified(false);

    RefreshAllStatusBarInfo();
}

void MainWindow::MenuFileReload(wxCommandEvent &event)
{
    if (!wxFile::Exists(CurrentTranslationFileInfo().GetPath())) return;

    if (wxMessageBox(_("Do you really want to reload file from drive?\nAll changes will be lost!"), _("Warning"), wxICON_WARNING | wxYES_NO, this) == wxNO) {
        return;
    }

    TranslationFileActions::LoadTranslationFromFile(CurrentTranslationFileInfo().GetPath(), m_textEditor);

    CurrentTranslationFileInfo().SetModified(false);
    CurrentTranslationFileInfo().SetVerified(false);

    RefreshAllStatusBarInfo();
}

void MainWindow::MenuFileSave(wxCommandEvent &event)
{
    if (CurrentTranslationFileInfo().GetPath().empty()) {
        MenuFileSaveAsUTF8(event);
	} else {
        TranslationFileActions::SaveTranslationToFile(
            CurrentTranslationFileInfo().GetPath(),
            CurrentTranslationFileInfo().GetCodepage(),
            m_textEditor
        );

        CurrentTranslationFileInfo().SetModified(false);

        RefreshAllStatusBarInfo();

        if (m_autoRecheckFlag->IsChecked()) {
            RecheckCurrentTranslation();
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

        for (int i = m_currentSearchData.GetFindString().Length(); i > 0; --i) {
            m_textEditor->CharRightExtend();
        }
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
    RecheckCurrentTranslation();
}

void MainWindow::MenuCheckGotoError(wxCommandEvent &event)
{
    if (CurrentTranslationFileInfo().GetVerified() == true) {
        MoveCursorToError();
    } else {
        RecheckCurrentTranslation();
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
            wxString anchor = wxString::Format(
                _("Error at %i:%i"),
                e.GetIndexOfBadLine()+1,
                e.GetIndexOfBadCharacterInLine()
            );

            m_dialogBatch->AddFileInfo(names[i], _(e.GetHint()), anchor);
        }
    }

    m_dialogBatch->Show();
}

void MainWindow::MenuHelpAbout(wxCommandEvent &event)
{
    int splashRawImageDataSize;
    auto_ptr<char> splashRawImageData(::wxLoadUserResource(L"SPLASH_ABOUT", L"RC_DATA", &splashRawImageDataSize));
    wxMemoryInputStream splashRawImageStream(splashRawImageData.get(),splashRawImageDataSize);
    wxBitmap splash_image(wxImage(splashRawImageStream, wxBITMAP_TYPE_PNG, -1), -1);

    auto_ptr<AboutDialog> wndAbout(new AboutDialog(this, splash_image));
    wndAbout->ShowModal();
}

void MainWindow::SaveAs(const wxString& codepage)
{
    wxFileDialog FileDialog(this, wxString::Format(_("Save translation file in %s codepage"), codepage), "", "", _("WeiDU TRA (*.tra)|*.tra|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (FileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    CurrentTranslationFileInfo().SetPath(FileDialog.GetPath());
    CurrentTranslationFileInfo().SetCodepage(codepage);
    TranslationFileActions::SaveTranslationToFile(CurrentTranslationFileInfo().GetPath(), codepage, m_textEditor);

    CurrentTranslationFileInfo().SetModified(false);

    RefreshAllStatusBarInfo();

    if (m_autoRecheckFlag->IsChecked()) {
        RecheckCurrentTranslation();
    }
}

void MainWindow::MenuUtilsToTlk(wxCommandEvent &event)
{
    if (!RecheckCurrentTranslation()) {
        wxMessageBox(_("Please fix errors in translation file before exporting it to .TLK!"), _("Error"), wxOK | wxICON_ERROR, this);

        return;
    }

    string temporaryFilePath = TranslationFileActions::CreateTemporaryFileWithTextEditorContent(m_textEditor);

    WeiDUModTranslation traFile;
    traFile.LoadFromFile(temporaryFilePath.c_str());
    remove(temporaryFilePath.c_str());

    wxString errorText = CheckWeiduTranslationToBeTLKCompatible(traFile);

    if (!errorText.empty()) {
        if (wxMessageBox(errorText + _("\nDo you want to continue?"), _("Warning"), wxICON_WARNING | wxYES_NO, this) == wxNO) {
            return;
        }
    }

    // TODO: Give other options than just BG:EE

    wxFileDialog FileDialog(this, _("Export translation to BG:EE .TLK"), "", "", _("Talktable file (*.tlk)|*.tlk|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (FileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    TalkTable dialogTlk;

    for (WeiDUModTranslation::iterator currentEntry = traFile.Begin(), entryEnd = traFile.End(); currentEntry != entryEnd; ++currentEntry) {
        dialogTlk.GetItems().push_back(TalkTableEntry(7, 0, 0, currentEntry->second->GetMainSound(), currentEntry->second->GetMainText()));
    }

    dialogTlk.SaveToFile(FileDialog.GetPath());
}

void MainWindow::MenuUtilsFromTlk(wxCommandEvent &event)
{
    // TODO: Give other options than just BG:EE

    wxFileDialog FileDialog(this, _("Import translation from BG:EE .TLK"), "", "", _("Talktable file (*.tlk)|*.tlk|All files (*.*)|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (FileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    m_textEditor->ClearAll();

    TalkTable dialogTlk;
    dialogTlk.LoadFromFile(FileDialog.GetPath());

    for (size_t currentEntryIndex = 0, entryEnd = dialogTlk.GetItems().size(); currentEntryIndex != entryEnd; ++currentEntryIndex) {
        wxString oneLine = TalkTableEntryToString(dialogTlk.GetItems()[currentEntryIndex], currentEntryIndex) + '\n';
        oneLine = wxString::FromUTF8(oneLine.c_str());

        m_textEditor->AddText(oneLine);
    }
}

void MainWindow::MenuUtilsCompareFolders(wxCommandEvent &event)
{
	CompareFoldersDialog dlgCompareFolders(this);
	dlgCompareFolders.ShowModal();
}

void MainWindow::InitializeMenubar()
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_FILE_NEW, _("&New"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_FILE_OPEN, _("&Open...\tCtrl+O"));
    menuFile->Append(ID_FILE_RELOAD, _("Re&load\tCtrl+R"));
    menuFile->Append(ID_FILE_SAVE, _("&Save changes\tCtrl+S"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_FILE_SAVE_AS_UTF8, _("S&ave as UTF-8 file..."));
    menuFile->Append(ID_FILE_SAVE_AS_CP1251, _("S&ave as CP1251 file..."));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, _("E&xit\tESC"));

    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append(ID_EDIT_UNDO, _("&Undo\tCtrl+Z"));
    menuEdit->Append(ID_EDIT_REDO, _("&Redo\tCtrl+Y"));
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_EDIT_CUT, _("&Cut\tCtrl+X"));
    menuEdit->Append(ID_EDIT_COPY, _("C&opy\tCtrl+C"));
    menuEdit->Append(ID_EDIT_PASTE, _("&Paste\tCtrl+V"));
    menuEdit->AppendSeparator();
    menuEdit->Append(ID_EDIT_SELECTALL, _("&Select all\tCtrl+A"));

    wxMenu *menuSearch = new wxMenu;
    menuSearch->Append(ID_SEARCH_FIND, _("&Find...\tCtrl+F"));
    menuSearch->AppendSeparator();
    menuSearch->Append(ID_SEARCH_FINDNEXT, _("Find &next\tF3"));
    menuSearch->Append(ID_SEARCH_FINDPREV, _("Find &prev\tF2"));
    menuSearch->AppendSeparator();
    menuSearch->Append(ID_SEARCH_REPLACE, _("&Replace\tCtrl+H"));

    wxMenu *menuCheck = new wxMenu;
    menuCheck->Append(ID_CHECK_DOCHECK, _("&Check content\tCtrl+T"));
    menuCheck->Append(ID_CHECK_GOTOERROR, _("Go to &mistake\tCtrl+G"));
    menuCheck->AppendSeparator();
    m_autoRecheckFlag = menuCheck->Append(ID_CHECK_AUTORECHECK, _("Automatic recheck on save"), wxEmptyString, wxITEM_CHECK);
    menuCheck->AppendSeparator();
    menuCheck->Append(ID_CHECK_BATCH, _("&Batch check\tCtrl+B"));

    wxMenu *menuUtils = new wxMenu;
    menuUtils->Append(ID_UTILS_FROM_TLK, _("Import text &from BG:EE style .TLK"));
    menuUtils->Append(ID_UTILS_TO_TLK, _("&Export text of BG:EE style .TLK"));
	menuUtils->AppendSeparator();
	menuUtils->Append(ID_UTILS_COMPARE_FOLDERS, _("&Compare folders with translations"));

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
}

void MainWindow::InitializeAccelerators()
{
    // Do nothing
}

bool MainWindow::GetAutoRecheck() const
{
    return m_autoRecheckFlag->IsChecked();
}

void MainWindow::SetAutoRecheck(bool autoRecheck)
{
    m_autoRecheckFlag->Check(autoRecheck);
}

wxStyledTextCtrl* MainWindow::GetTextEditor()
{
    return m_textEditor;
}

TranslationFileInfo& MainWindow::CurrentTranslationFileInfo()
{
    return wxGetApp().CurrentFileInfo();
}
