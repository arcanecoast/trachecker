#ifndef TRACHECKER_APPLICATION
#define TRACHECKER_APPLICATION

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class MyApp: public wxApp
{
public:
    virtual bool OnInit();

    wxString GetCurrentFilePath() const;
    void UpdateCurrentFilePath(const wxString& path);

    bool IsFileChecked() const;
    bool IsFileModified() const;
    void SetFileChecked(bool value);
    void SetFileModified(bool value);

    void SetCurrentCodepage(const wxString& codepage);
    wxString GetCurrentCodepage() const;

    void SetDefaultCodepage(const wxString& codepage);
    wxString GetDefaultCodepage() const;

protected:
	wxLocale m_locale;
    wxString m_currentFilePath;
    wxString m_currentCodePage;
    wxString m_defaultCodePage;

    bool m_isFileChecked;  // Current file is checked with TRA HighLevelParser
    bool m_isFileModified; // Current file is modified
};

MyApp& wxGetApp();

#endif // TRACHECKER_APPLICATION