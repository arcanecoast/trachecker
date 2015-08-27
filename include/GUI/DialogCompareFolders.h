#ifndef TRACHECKER_DIALOG_COMPARE_FOLDERS
#define TRACHECKER_DIALOG_COMPARE_FOLDERS

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/listctrl.h>
#include <set>

using std::set;

class MainWindow;

class CompareFoldersDialog: public wxDialog
{
public:
	CompareFoldersDialog(MainWindow *parent);

    void SetFileInfoInList(const wxString& Filename, const wxString& Error);
    void ClearFilesList();

private:
    wxListCtrl *m_listFiles;
	wxTextCtrl *m_editFirstFolder;
	wxTextCtrl *m_editSecondFolder;

private:
    void OnCompareByEntriesContentClick(wxCommandEvent& event);
	void OnCompareByEntriesCountClick(wxCommandEvent& event);
	void OnSelectFirstFolderClick(wxCommandEvent& event);
	void OnSelectSecondFolderClick(wxCommandEvent& event);
    void OnCloseClick(wxCommandEvent& event);

	wxString CallSelectFolderDialog(const wxString& defaultFolder);

	unsigned int CompareFiles(const set<wxString>& filenames, bool compareContent);
	bool HaveDifferentFolderStructure(const set<wxString>& firstFolderFiles, const set<wxString>& secondFolderFiles);

    wxDECLARE_EVENT_TABLE();
};

#endif //TRACHECKER_DIALOG_COMPARE_FOLDERS