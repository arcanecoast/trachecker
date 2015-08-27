#ifndef TRACHECKER_MAINWINDOW
#define TRACHECKER_MAINWINDOW

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/stc/stc.h>
#include <wx/fdrepdlg.h>
#include <wx/timer.h>

#include <memory>

using std::auto_ptr;

class BatchDialog;
class TranslationFileInfo;

class MainWindow: public wxFrame
{
public:
    MainWindow();
    ~MainWindow();

    void RefreshAllStatusBarInfo();

    void RefreshStatusBarCaretInfo();
    void RefreshStatusBarFileInfo();
    void RefreshStatusBarChangesMarker();

    bool RecheckCurrentTranslation();
    void MoveCursorToError();

    bool GetAutoRecheck() const;
    wxStyledTextCtrl* GetTextEditor();
    TranslationFileInfo& CurrentTranslationFileInfo();

    void SetAutoRecheck(bool autoRecheck);

private:
    wxStyledTextCtrl* m_textEditor;
    wxMenuItem* m_autoRecheckFlag;

    wxTimer m_timerErrorHighlight;

    wxFindReplaceData m_currentSearchData;
    auto_ptr<wxFindReplaceDialog> m_dialogSearch;

    auto_ptr<BatchDialog> m_dialogBatch;

    static const int STATUSBAR_PANE_CARET;
    static const int STATUSBAR_PANE_CODEPAGE;
    static const int STATUSBAR_PANE_ISMODIFIED;
    static const int STATUSBAR_PANE_ERROR_MESSAGE;
    static const int STATUSBAR_PANE_FILEPATH;

    static const wxColor EDITOR_ACTIVE_LINE_BACKGROUND;
    static const wxColor EDITOR_ERROR_LINE_BACKGROUND;

private:
    wxDECLARE_EVENT_TABLE();

    void SaveAs(const wxString& codepage);

    void OnTimerHighlightError(wxTimerEvent &event);

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
	void MenuUtilsCompareFolders(wxCommandEvent &event);

    void MenuHelpAbout(wxCommandEvent &event);

    void InitializeAccelerators();
    void InitializeMenubar();
};

#endif //TRACHECKER_MAINWINDOW