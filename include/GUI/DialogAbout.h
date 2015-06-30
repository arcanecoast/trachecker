#ifndef TRACHECKER_ABOUTDIALOG
#define TRACHECKER_ABOUTDIALOG

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class AboutDialog: public wxDialog
{
public:
    AboutDialog(wxWindow *parent, const wxBitmap &image);

private:
    wxBitmap m_aboutImage;

    wxString m_buildAuthor;
    wxString m_buildDate;

private:
    wxDECLARE_EVENT_TABLE();

    void OnMouseClick(wxMouseEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnPaint(wxPaintEvent& event);
    void OnBackgroundErase(wxEraseEvent& event);

    wxString CalculateBuildDate();
};

#endif //TRACHECKER_ABOUTDIALOG
