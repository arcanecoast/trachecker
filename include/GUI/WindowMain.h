#ifndef TRACHECKER_MAINWINDOW
#define TRACHECKER_MAINWINDOW

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/stc/stc.h>
#include <wx/fdrepdlg.h>
#include <wx/mstream.h>
#include <wx/progdlg.h>
#include <wx/timer.h>

#include <memory>

#include <cppbg/tra/TranslationException.h>

using std::auto_ptr;
using cppbg_tra::TranslationException;

class BatchDialog;

class MainWindow: public wxFrame
{
public:
    MainWindow();
    ~MainWindow();    

	void UpdateCaretStatus();
	void UpdateFileStatus();
	void UpdateChangesStatus(const bool &ChangesMade = true);

    wxString MainWindow::CreateTemporaryFileWithTextEditorContent();

    bool RecheckFile();
    void GoToMistake();

    bool ReadContentFromFile(const wxString& path);
    void WriteToFile(const wxString& path, const wxString& encoding = "UTF-8");
    void SaveAs(const wxString& codepage);

    bool IsAutoRecheckEnabled() const;
    void EnableAutoRecheck(bool autoRecheck);

private:
    wxTimer m_errorsHighlightingTimer;

    wxFindReplaceData m_currentSearchData;

    auto_ptr<wxFindReplaceDialog> m_dialogSearch;
    auto_ptr<BatchDialog> m_dialogBatch;
    wxStyledTextCtrl *m_textEditor;
    wxMenuItem *m_autoRecheckFlag;

    auto_ptr<TranslationException> m_latestTranslationError;

private:
    void DoMistakeHighlight(wxTimerEvent &event);

	void EditorFind(wxFindDialogEvent& event);
	void EditorReplace(wxFindDialogEvent& event);
	void EditorReplaceAll(wxFindDialogEvent& event);
	void EditorSearchClosed(wxFindDialogEvent& event);
	void EditorChange(wxStyledTextEvent &event);
	void EditorStateUpdate(wxStyledTextEvent &event);

	void MenuFileNew(wxCommandEvent &event);
	void MenuFileOpen(wxCommandEvent &event);
	void MenuFileReload(wxCommandEvent &event);
	void MenuFileSave(wxCommandEvent &event);
    void MenuFileSaveAsUTF8(wxCommandEvent &event);
    void MenuFileSaveAsCP1251(wxCommandEvent &event);
	void MenuFileExit(wxCommandEvent &event);

	void MenuEditUndo(wxCommandEvent &event);
	void MenuEditRedo(wxCommandEvent &event);
	void MenuEditCut(wxCommandEvent &event);
	void MenuEditCopy(wxCommandEvent &event);
	void MenuEditPaste(wxCommandEvent &event);
	void MenuEditSelectAll(wxCommandEvent &event);

	void MenuSearchFind(wxCommandEvent &event);
	void MenuSearchFindNext(wxCommandEvent &event);
	void MenuSearchFindPrev(wxCommandEvent &event);
	void MenuSearchReplace(wxCommandEvent &event);

    void MenuCheckDoRecheck(wxCommandEvent &event);
    void MenuCheckGotoError(wxCommandEvent &event);
    void MenuCheckBatch(wxCommandEvent &event);

    void MenuUtilsToTlk(wxCommandEvent &event);
    void MenuUtilsFromTlk(wxCommandEvent &event);

    void MenuHelpAbout(wxCommandEvent &event);

    void InitializeAccelerators();
    void InitializeMenubar();

	wxDECLARE_EVENT_TABLE();
};

void MainWindow_UpdateStatus(MainWindow* window, const bool &are_changes_made = true);

#endif //TRACHECKER_MAINWINDOW