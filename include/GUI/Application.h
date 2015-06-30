#ifndef TRACHECKER_APPLICATION
#define TRACHECKER_APPLICATION

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <memory>

#include <Utilities/TranslationFileInfo.h>
#include <Utilities/Configuration.h>

#include <cppbg/tra/Position.h>

using std::auto_ptr;
using cppbg_tra::Position;

class Application: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();

    TranslationFileInfo& CurrentFileInfo();
    Configuration& CurrentConfiguration();

    Position& LatestTranslationErrorPosition();
    wxString& LatestTranslationErrorHint();

private:
    TranslationFileInfo m_currentFileInfo;
    Configuration m_currentConfiguration;

    Position m_latestTranslationErrorPosition;
    wxString m_latestTranslationErrorHint;
};

Application& wxGetApp();

#endif // TRACHECKER_APPLICATION