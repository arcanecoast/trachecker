#include <wx/file.h>
#include <wx/stdpaths.h>

#include "application.h"
#include "window_main.h"

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    m_isFileChecked  = false;
    m_isFileModified = false;

    wxImage::AddHandler(new wxPNGHandler());

    wxString applicationDir = wxStandardPaths::Get().GetExecutablePath();
    applicationDir = applicationDir.substr(0, applicationDir.rfind("TRAChecker.exe"));

	wxLocale::AddCatalogLookupPathPrefix(applicationDir + "languages");
    m_locale.Init(wxLocale::GetSystemLanguage(), wxLOCALE_DONT_LOAD_DEFAULT);
	m_locale.AddCatalog("application");

    bool do_recheck = false;

    switch (this->argc) {
        case 2:
            m_currentFilePath = this->argv[1];
        break;
        case 3:
            m_currentFilePath = this->argv[2];
            if (this->argv[1] == "--recheck") {
                do_recheck = true;
            }
        break;
    }

    MainWindow *wndMain = new MainWindow();

    if (!m_currentFilePath.empty() && wxFile::Exists(m_currentFilePath)) {
        wndMain->ReadFromFile(m_currentFilePath);

        MainWindow_UpdateStatus(wndMain, false);
    }

    wndMain->Show(true);

    if (do_recheck) {
        wndMain->RecheckFile();
    }

    return true;
}

wxString MyApp::GetCurrentFilePath() const
{
    return m_currentFilePath;
}

void MyApp::UpdateCurrentFilePath(const wxString& path)
{
    m_currentFilePath = path;
}

bool MyApp::IsFileChecked() const
{
    return m_isFileChecked;
}

bool MyApp::IsFileModified() const
{
    return m_isFileModified;
}

void MyApp::SetFileChecked(bool value)
{
    m_isFileChecked = value;
}

void MyApp::SetFileModified(bool value)
{
    m_isFileModified = value;
}

void MyApp::SetCurrentCodepage(const wxString& codepage)
{
    m_currentCodePage = codepage;
}

wxString MyApp::GetCurrentCodepage() const
{
    return m_currentCodePage;
}

void MyApp::SetDefaultCodepage(const wxString& codepage)
{
    m_defaultCodePage = codepage;
}

wxString MyApp::GetDefaultCodepage() const
{
    return m_defaultCodePage;
}