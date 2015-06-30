#include <wx/file.h>
#include <wx/stdpaths.h>

#include <GUI/Application.h>
#include <GUI/WindowMain.h>

#include <Utilities/TranslationFileActions.h>

wxIMPLEMENT_APP(Application);

bool Application::OnInit()
{
    static wxLocale defaultLocale;

    // Initialize libpng for wxWidgets

    wxImage::AddHandler(new wxPNGHandler());

    // Get current executable directory

    wxString applicationDir = wxStandardPaths::Get().GetExecutablePath();
    applicationDir = applicationDir.substr(0, applicationDir.rfind("TRAChecker.exe"));

    // Initialize localization

	wxLocale::AddCatalogLookupPathPrefix(applicationDir);
    defaultLocale.AddCatalog("application");
    defaultLocale.Init(wxLocale::GetSystemLanguage(), wxLOCALE_LOAD_DEFAULT);

    // Load and apply configuration

    CurrentConfiguration().Load();

    CurrentFileInfo().SetCodepage(CurrentConfiguration().GetDefaultCodepage());

    // Create main window

    MainWindow* m_mainWindow = new MainWindow();

    m_mainWindow->SetSize(CurrentConfiguration().GetWidth(), CurrentConfiguration().GetHeight());
    m_mainWindow->Maximize(CurrentConfiguration().GetMaximized());
    m_mainWindow->SetAutoRecheck(CurrentConfiguration().GetAutoRecheckOnSave());

    // Read command line parameters

    bool doRecheck = false;

    if (this->argc == 2) {
        CurrentFileInfo().SetPath(this->argv[1]);
    } else if (this->argc == 3) {
        if (this->argv[1] == "--recheck") {
            doRecheck = true;
        }

        CurrentFileInfo().SetPath(this->argv[2]);
    }

    // Use command line parameters

    if (!CurrentFileInfo().GetPath().empty() && wxFile::Exists(CurrentFileInfo().GetPath())) {
        TranslationFileActions::LoadTranslationFromFile(CurrentFileInfo().GetPath(), m_mainWindow->GetTextEditor());

        m_mainWindow->RefreshAllStatusBarInfo();
    }
    
    m_mainWindow->Show(true);

    if (doRecheck) {
        m_mainWindow->RecheckCurrentTranslation();
    }

    return true;
}

int Application::OnExit()
{
    CurrentConfiguration().Save();

    return wxApp::OnExit();
}

TranslationFileInfo& Application::CurrentFileInfo()
{
    return m_currentFileInfo;
}

Configuration& Application::CurrentConfiguration()
{
    return m_currentConfiguration;
}

Position& Application::LatestTranslationErrorPosition()
{
    return m_latestTranslationErrorPosition;
}

wxString& Application::LatestTranslationErrorHint()
{
    return m_latestTranslationErrorHint;
}

