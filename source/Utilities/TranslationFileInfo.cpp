#include <Utilities/TranslationFileInfo.h>

TranslationFileInfo::TranslationFileInfo() :
    m_currentCodePage("?"),
    m_filePath(""),
    m_isFileVerified(false),
    m_isFileModified(false)
{

}

wxString TranslationFileInfo::GetPath() const
{
    return m_filePath;
}

bool TranslationFileInfo::GetVerified() const
{
    return m_isFileVerified;
}

bool TranslationFileInfo::GetModified() const
{
    return m_isFileModified;
}

wxString TranslationFileInfo::GetCodepage() const
{
    return m_currentCodePage;
}

void TranslationFileInfo::SetPath(const wxString& path)
{
    m_filePath = path;
}

void TranslationFileInfo::SetVerified(bool value)
{
    m_isFileVerified = value;
}

void TranslationFileInfo::SetModified(bool value)
{
    m_isFileModified = value;
}

void TranslationFileInfo::SetCodepage(const wxString& codepage)
{
    m_currentCodePage = codepage;
}
