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

protected:
    wxBitmap m_aboutImage;

private:
    void OnMouseClick(wxMouseEvent &event);
    void OnKeyDown(wxKeyEvent &event);
    void OnPaint(wxPaintEvent& event);
    void OnBackgroundErase(wxEraseEvent& event);

    wxDECLARE_EVENT_TABLE();
};

#endif //TRACHECKER_ABOUTDIALOG
