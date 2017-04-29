#include "GCodeDebugView.h"
#include <iostream>
#include <cairo.h>
#include <cairo-svg.h>

using namespace std;

/** Debug graphic implementation */
struct GCodeDebugViewImpl : GCodeDebugView
{
    cairo_t *c;
    cairo_surface_t *cs;

    GCodeDebugViewImpl() :
        c(NULL),
        cs(NULL)
    {
    }

    virtual ~GCodeDebugViewImpl()
    {
        if (cs)
            cairo_surface_destroy (cs);
        if (c)
            cairo_destroy(c);
    }
    virtual void Segment(double x1,double y1,double x2,double y2,int nColor);
    virtual void Point(double x,double y,int nColor);
    /** Scale size */
    static double Scale(double sz);
    /** Scale position and flip Y axis */
    static void Scale(double& x,double& y);
};

double GCodeDebugViewImpl::Scale(double sz)
{
    return sz * 10.0;
}

void GCodeDebugViewImpl::Scale(double& x,double& y)
{
    x = Scale(x);
    y = (200.0 - y);
    y = Scale(y);
}

void GCodeDebugViewImpl::Segment(double x1,double y1,double x2,double y2,int nColor)
{
    Scale(x1,y1);
    Scale(x2,y2);
    cairo_set_line_width(c,Scale(0.7));
    switch (nColor)
    {
        case 0:
            cairo_set_source_rgba(c,0,1,0,0.1);
            break;
        case 1:
            cairo_set_source_rgba(c,1,0,0,0.1);
            break;
        case 3:
            cairo_set_source_rgba(c,1,0,0,0); // transparent
            break;
        case 2:
        default:
            cairo_set_source_rgba(c,0,1,1,0.4);
            break;
    }
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    cairo_move_to(c,x1,y1);
    cairo_line_to(c,x2,y2);
    cairo_stroke(c);

    cairo_set_line_width(c,Scale(0.7)*0.1);
    if (nColor == 3)
        cairo_set_source_rgba(c,0.1,0.1,0.1,0.5);
    else
        cairo_set_source_rgba(c,0,0.4,0.5,0.9);
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    cairo_move_to(c,x1,y1);
    cairo_line_to(c,x2,y2);
    cairo_stroke(c);
}

void GCodeDebugViewImpl::Point(double x,double y,int nColor)
{
    Scale(x,y);
    switch (nColor)
    {
        case 0:
        default:
            cairo_set_source_rgba(c,0,1,0,0.7);
            break;
        case 1:
            cairo_set_source_rgba(c,0,0,1,0.7);
            break;
        case 2:
            cairo_set_source_rgba(c,1,0,0.5,0.7);
            break;
        case 3:
            cairo_set_source_rgba(c,0,1,0.2,0.7);
            break;

    }
    cairo_move_to(c,x,y);
    if (nColor == 1 || nColor == 3)
        cairo_arc(c,x,y,Scale(0.35)*0.3,0,2*3.14159265358979323846);
    else
        cairo_arc(c,x,y,Scale(0.35)*0.2,0,2*3.14159265358979323846);
    cairo_fill(c);
/*
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(c,0.35*10.0);
    cairo_move_to (c, x*10.0, y*10.0);
    cairo_close_path (c);
    cairo_stroke (c);
    */
}

std::unique_ptr<GCodeDebugView> GCodeDebugViewFactory()
{
    unique_ptr<GCodeDebugViewImpl> ret(new GCodeDebugViewImpl());
    ret->cs = cairo_svg_surface_create("post_stretch.svg",ret->Scale(200.0),ret->Scale(200.0));
    ret->c = cairo_create (ret->cs);
    return ret;
}
