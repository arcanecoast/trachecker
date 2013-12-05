#ifndef TRACHECKER_BATCHDIALOG
#define TRACHECKER_BATCHDIALOG

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/listctrl.h>

class MainWindow;

class BatchDialog: public wxDialog
{
public:
    BatchDialog(MainWindow *parent, const wxString& directory = wxEmptyString);

    void AddFileInfo(const wxString& Path, const wxString& ErrorType, const wxString& Anchor = wxEmptyString);
    void OpenFile(const wxString& Path);
    void ClearFilesList();

private:
    wxListCtrl *m_listFiles;
    wxString    m_directory;

private:
    void OnOpenClick(wxCommandEvent& event);
    void OnCloseClick(wxCommandEvent& event);
    void OnListDoubleClick(wxListEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#endif //TRACHECKER_BATCHDIALOG