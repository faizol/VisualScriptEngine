#include "wxDrawingContext.hpp"
#include "Debug.hpp"

wxDrawingContext::wxDrawingContext () :
	NUIE::DrawingContext (),
	width (0),
	height (0),
	memoryBitmap (new wxBitmap ()),
	memoryDC (new wxMemoryDC ())
{
	memoryDC->SelectObject (*memoryBitmap);
}

wxDrawingContext::~wxDrawingContext ()
{
	delete memoryBitmap;
	delete memoryDC;
}

void wxDrawingContext::DrawToDC (wxDC* targetDC)
{
	targetDC->Blit (0, 0, width, height, memoryDC, 0, 0);
}

void wxDrawingContext::Resize (int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;

	delete memoryBitmap;
	delete memoryDC;

	memoryBitmap = new wxBitmap (newWidth, newHeight);
	memoryDC = new wxMemoryDC ();
	memoryDC->SelectObject (*memoryBitmap);
}

double wxDrawingContext::GetWidth () const
{
	return width;
}

double wxDrawingContext::GetHeight () const
{
	return height;
}

void wxDrawingContext::BeginDraw ()
{

}

void wxDrawingContext::EndDraw ()
{

}

void wxDrawingContext::DrawLine (const NUIE::Point& beg, const NUIE::Point& end, const NUIE::Pen& pen)
{
	
}

void wxDrawingContext::DrawBezier (const NUIE::Point& p1, const NUIE::Point& p2, const NUIE::Point& p3, const NUIE::Point& p4, const NUIE::Pen& pen)
{
	// TODO: Bezier would be better
	memoryDC->SetBrush (*wxTRANSPARENT_BRUSH);
	memoryDC->SetPen (GetPen (pen));
	wxPoint points[4] = {
		GetPoint (p1),
		GetPoint (p2),
		GetPoint (p3),
		GetPoint (p4)
	};
	memoryDC->DrawSpline (4, points);
}

void wxDrawingContext::DrawRect (const NUIE::Rect& rect, const NUIE::Pen& pen)
{
	memoryDC->SetBrush (*wxTRANSPARENT_BRUSH);
	memoryDC->SetPen (GetPen (pen));
	memoryDC->DrawRectangle (GetRect (rect));
}

void wxDrawingContext::FillRect (const NUIE::Rect& rect, const NUIE::Color& color)
{
	memoryDC->SetBrush (wxBrush (wxColour (color.GetR (), color.GetG (), color.GetB ())));
	memoryDC->SetPen (*wxTRANSPARENT_PEN);
	memoryDC->DrawRectangle (GetRect (rect));
}

void wxDrawingContext::DrawEllipse (const NUIE::Rect& rect, const NUIE::Pen& pen)
{
	
}

void wxDrawingContext::FillEllipse (const NUIE::Rect& rect, const NUIE::Color& color)
{
	
}

void wxDrawingContext::DrawFormattedText (const NUIE::Rect& rect, const NUIE::Font& font, const std::wstring& text, NUIE::HorizontalAnchor hAnchor, NUIE::VerticalAnchor vAnchor, const NUIE::Color& textColor)
{
	
}

NUIE::Size wxDrawingContext::MeasureText (const NUIE::Font& font, const std::wstring& text)
{
	return NUIE::Size (100, 20);
}

wxPoint wxDrawingContext::GetPoint (const NUIE::Point& point)
{
	return wxPoint ((int) std::floor (point.GetX ()) - 1, (int) std::floor (point.GetY ()) - 1);
}

wxRect wxDrawingContext::GetRect (const NUIE::Rect& rect)
{
	NUIE::IntRect intRect (rect);
	return wxRect (intRect.GetX (), intRect.GetY (), intRect.GetWidth (), intRect.GetHeight ());
}

wxColour wxDrawingContext::GetColor (const NUIE::Color& color)
{
	return wxColour (color.GetR (), color.GetG (), color.GetB ());
}

wxPen wxDrawingContext::GetPen (const NUIE::Pen& pen)
{
	int width = (int) pen.GetThickness ();
	return wxPen (GetColor (pen.GetColor ()), width);
}