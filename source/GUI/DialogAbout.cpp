#include <string.h>

#include <map>
#include <string>

#include <GUI/DialogAbout.h>

using std::map;
using std::string;

wxBEGIN_EVENT_TABLE(AboutDialog, wxDialog)
    EVT_LEFT_DOWN(AboutDialog::OnMouseClick)
    EVT_KEY_DOWN(AboutDialog::OnKeyDown)
    EVT_PAINT(AboutDialog::OnPaint)
    EVT_ERASE_BACKGROUND(AboutDialog::OnBackgroundErase)
wxEND_EVENT_TABLE()

AboutDialog::AboutDialog(wxWindow *parent, const wxBitmap &image) :
    wxDialog(parent, wxID_ANY, wxEmptyString,
        parent ? wxPoint(
            parent->GetPosition().x + (parent->GetSize().GetWidth() - image.GetSize().GetWidth()) / 2,
            parent->GetPosition().y + (parent->GetSize().GetHeight() - image.GetSize().GetHeight()) / 2
        ) : wxDefaultPosition,
        image.GetSize(), wxBORDER_NONE | wxBG_STYLE_TRANSPARENT),
    m_aboutImage(image),
    m_buildAuthor("by metamaker")
{
    m_buildDate = "build " + CalculateBuildDate();
}

void AboutDialog::OnMouseClick(wxMouseEvent &event)
{
    Close();
}

void AboutDialog::OnKeyDown(wxKeyEvent &event)
{
    Close();
}

void AboutDialog::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    dc.DrawBitmap(m_aboutImage, 0, 0, true);

    dc.SetTextForeground(wxColor(0xFF, 0xFF, 0xFF));
    dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Verdana"));

    dc.DrawText(m_buildDate, wxPoint(255, 122));
    dc.DrawText(m_buildAuthor, wxPoint(273, 139));
}

void AboutDialog::OnBackgroundErase(wxEraseEvent& event)
{
}

wxString AboutDialog::CalculateBuildDate()
{
    static const char *builtOn = __DATE__;

    int year;
    char month[20];
    int day;

    sscanf(builtOn, "%s %i %i", month, &day, &year);

    map<string, int> monthsMapping;
    monthsMapping["Jan"] = 1;
    monthsMapping["Feb"] = 2;
    monthsMapping["Mar"] = 3;
    monthsMapping["Apr"] = 4;
    monthsMapping["May"] = 5;
    monthsMapping["Jun"] = 6;
    monthsMapping["Jul"] = 7;
    monthsMapping["Aug"] = 8;
    monthsMapping["Sep"] = 9;
    monthsMapping["Oct"] = 10;
    monthsMapping["Nov"] = 11;
    monthsMapping["Dec"] = 12;

    return wxString::Format("%.2i-%.2i-%i", day, monthsMapping[month], year);
}