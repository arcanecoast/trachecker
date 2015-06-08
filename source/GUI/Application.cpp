#include <wx/file.h>
#include <wx/stdpaths.h>
#include <wx/fileconf.h>

#include <GUI/Application.h>
#include <GUI/WindowMain.h>

wxIMPLEMENT_APP(Application);

bool Application::OnInit()
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

    m_windowMain = new MainWindow();

    if (!m_currentFilePath.empty() && wxFile::Exists(m_currentFilePath)) {
        m_windowMain->ReadContentFromFile(m_currentFilePath);

        MainWindow_UpdateStatus(m_windowMain, false);
    }

    LoadConfig();
    
    m_windowMain->Show(true);

    if (do_recheck) {
        m_windowMain->RecheckFile();
    }

    return true;
}

wxString Application::GetCurrentFilePath() const
{
    return m_currentFilePath;
}

void Application::UpdateCurrentFilePath(const wxString& path)
{
    m_currentFilePath = path;
}

bool Application::IsFileChecked() const
{
    return m_isFileChecked;
}

bool Application::IsFileModified() const
{
    return m_isFileModified;
}

void Application::SetFileChecked(bool value)
{
    m_isFileChecked = value;
}

void Application::SetFileModified(bool value)
{
    m_isFileModified = value;
}

void Application::SetCurrentCodepage(const wxString& codepage)
{
    m_currentCodePage = codepage;
}

wxString Application::GetCurrentCodepage() const
{
    return m_currentCodePage;
}

void Application::SetDefaultCodepage(const wxString& codepage)
{
    m_defaultCodePage = codepage;
}

wxString Application::GetDefaultCodepage() const
{
    return m_defaultCodePage;
}

void Application::LoadConfig()
{
    wxFileConfig config("TRA Check", "Aldark", "tracheck.ini", "tracheck.ini", wxCONFIG_USE_LOCAL_FILE);

    bool bAutoRecheck, bMaximized;
    int iWidth, iHeight;
    wxString defaultCodepage;
    config.Read("AutoRecheckOnSave", &bAutoRecheck, false);
    config.Read("Width", &iWidth, 620);
    config.Read("Height", &iHeight, 440);
    config.Read("Maximized", &bMaximized, false);
    config.Read("DefaultCodepage", &defaultCodepage, "UTF-8");

    SetDefaultCodepage(defaultCodepage);
    SetCurrentCodepage(defaultCodepage);

    m_windowMain->SetSize(iWidth, iHeight);
    m_windowMain->Maximize(bMaximized);
    m_windowMain->EnableAutoRecheck(bAutoRecheck);
}

void Application::SaveConfig()
{
    wxFileConfig config("TRA Check", "Aldark", "tracheck.ini", "tracheck.ini", wxCONFIG_USE_LOCAL_FILE);
    config.Write("AutoRecheckOnSave", m_windowMain->IsAutoRecheckEnabled());
    if (!m_windowMain->IsMaximized()) {
        config.Write("Width", m_windowMain->GetSize().GetWidth());
        config.Write("Height", m_windowMain->GetSize().GetHeight());
    }
    config.Write("Maximized", m_windowMain->IsMaximized());
    config.Write("DefaultCodepage", GetDefaultCodepage());
}
