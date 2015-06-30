#include <wx/stc/stc.h>
#include <wx/convauto.h>

#include <fstream>
#include <string>

#include <GUI/Application.h>
#include <Utilities/TranslationFileActions.h>

using std::ofstream;
using std::ios_base;
using std::string;

void TranslationFileActions::LoadTranslationFromFile(const wxString& filepath, wxStyledTextCtrl* textEditor)
{
    wxConvAuto::SetFallbackEncoding(wxFONTENCODING_CP1251);

    textEditor->LoadFile(filepath);
    textEditor->GotoPos(0);

    wxGetApp().CurrentFileInfo().SetCodepage(wxGetApp().CurrentConfiguration().GetDefaultCodepage());
}

void TranslationFileActions::SaveTranslationToFile(const wxString& path, const wxString& encoding, wxStyledTextCtrl* textEditor)
{
    textEditor->SetEOLMode(wxSTC_EOL_CRLF);
    string translationText = textEditor->GetTextRaw();

    if (encoding == "UTF-8") {
        // Do nothing
    } else if (encoding == "CP1251") {
        translationText = wxString::FromUTF8(translationText.c_str());

        if (translationText.empty() && !textEditor->GetText().empty()) {
            wxMessageBox(_("Unable to convert text to CP1251. File wasn't saved!"), _("Error"), wxOK_DEFAULT | wxOK | wxICON_ERROR, textEditor->GetParent());

            return;
        }
    }

    ofstream out(static_cast<const char*>(path.c_str()), ios_base::binary | ios_base::out);

    if (!out.good()) {
        return;
    }

    out.write(translationText.c_str(), translationText.length());
    out.close();
}

wxString TranslationFileActions::CreateTemporaryFileWithTextEditorContent(wxStyledTextCtrl* textEditor)
{
    textEditor->SetEOLMode(wxSTC_EOL_CRLF);
    wxString text = textEditor->GetTextRaw();

    if (text.empty() && !textEditor->GetText().empty()) {
        wxMessageBox(_("Unable to check the file."), _("Warning"), wxOK_DEFAULT | wxOK | wxICON_WARNING, textEditor->GetParent());

        return "";
    }

    string filename = wxString::Format("test_tra_file#%i.tra", abs(rand()));

    ofstream output(filename.c_str(), ios_base::binary | ios_base::out);
    if (!output.is_open()) {
        wxMessageBox(_("Unable to create temporary file."), _("Error"), wxOK_DEFAULT | wxOK | wxICON_ERROR, textEditor->GetParent());

        return "";
    } else {
        output.write(text.c_str(), text.length());
        output.close();
    }

    return filename;
}
