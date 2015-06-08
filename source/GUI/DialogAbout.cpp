#include <GUI/DialogAbout.h>

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
    m_aboutImage(image)
{ 
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
}

void AboutDialog::OnBackgroundErase(wxEraseEvent& event)
{
}