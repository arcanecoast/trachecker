#include <wx/fileconf.h>

#include <Utilities/Configuration.h>

const wxString Configuration::appName = "TRA Check";
const wxString Configuration::vendorName = "Aldark";
const wxString Configuration::localFilename = "tracheck.ini";
const wxString Configuration::globalFilename = "tracheck.ini";

const bool Configuration::default_autoRecheckOnSave = false;
const wxString Configuration::default_defaultCodepage = "UTF-8";
const bool Configuration::default_maximized = false;
const int  Configuration::default_height = 440;
const int  Configuration::default_width = 620;

Configuration::Configuration() : 
    m_autoRecheckOnSave(Configuration::default_autoRecheckOnSave),
    m_defaultCodepage(Configuration::default_defaultCodepage),
    m_maximized(Configuration::default_maximized),
    m_height(Configuration::default_height),
    m_width(Configuration::default_width)
{
    
}

void Configuration::Load()
{
    wxFileConfig config(Configuration::appName, Configuration::vendorName, Configuration::localFilename, Configuration::globalFilename, wxCONFIG_USE_LOCAL_FILE);

    config.Read("AutoRecheckOnSave", &m_autoRecheckOnSave, Configuration::default_autoRecheckOnSave);
    config.Read("DefaultCodepage", &m_defaultCodepage, Configuration::default_defaultCodepage);
    config.Read("Maximized", &m_maximized, Configuration::default_maximized);
    config.Read("Height", &m_height, Configuration::default_height);
    config.Read("Width", &m_width, Configuration::default_width);
}

void Configuration::Save() const
{
    wxFileConfig config(Configuration::appName, Configuration::vendorName, Configuration::localFilename, Configuration::globalFilename, wxCONFIG_USE_LOCAL_FILE);

    config.Write("AutoRecheckOnSave", m_autoRecheckOnSave);
    config.Write("DefaultCodepage", m_defaultCodepage);
    config.Write("Maximized", m_maximized);
    config.Write("Height", m_height);
    config.Write("Width", m_width);
}

bool Configuration::GetAutoRecheckOnSave() const
{
    return m_autoRecheckOnSave;
}

wxString Configuration::GetDefaultCodepage() const
{
    return m_defaultCodepage;
}

bool Configuration::GetMaximized() const
{
    return m_maximized;
}

int Configuration::GetHeight() const
{
    return m_height;
}

int Configuration::GetWidth() const
{
    return m_width;
}

void Configuration::SetAutoRecheckOnSave(bool val)
{
    m_autoRecheckOnSave = val;
}

void Configuration::SetDefaultCodepage(const wxString& val)
{
    m_defaultCodepage = val;
}

void Configuration::SetMaximized(bool val)
{
    m_maximized = val;
}

void Configuration::SetHeight(int val)
{
    m_height = val;
}

void Configuration::SetWidth(int val)
{
    m_width = val;
}