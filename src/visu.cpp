#include "visu.h"
#include <iostream>
#include <cairo.h>
#include <cairo-svg.h>

using namespace std;

struct ImplVisuGCode : VisuGCode
{
    cairo_t *c;
    cairo_surface_t *cs;

    ImplVisuGCode() :
        c(NULL),
        cs(NULL) {}

    virtual ~ImplVisuGCode()
    {
        if (cs)
            cairo_surface_destroy (cs);
        if (c)
            cairo_destroy(c);
    }
    virtual void Flush()
    {
        cairo_surface_write_to_png(cs,"toto.png");
    }
    virtual void Segments(const std::vector<std::pair<double,double>>& v);
};


void ImplVisuGCode::Segments(const std::vector<std::pair<double,double>>& v)
{
    cairo_set_line_width(c,0.8*10.0);
    cairo_set_source_rgba(c,1,0,0,1);
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    for (auto i = v.begin(); i != v.end(); i++)
    {
        if (i == v.begin())
            cairo_move_to(c,i->first*10.0,i->second*10.0);
        else
            cairo_line_to(c,i->first*10.0,i->second*10.0);
    }
    cairo_stroke(c);



    cairo_set_line_width(c,0.8*10.0*0.5);
    cairo_set_source_rgba(c,0,0,0,0.5);
    for (auto i = v.begin(); i != v.end(); i++)
    {
        if (i == v.begin())
            cairo_move_to(c,i->first*10.0,i->second*10.0);
        else
            cairo_line_to(c,i->first*10.0,i->second*10.0);
    }

    /*
    cout << "segment " << x1 << "," << y1 << " -> " << x2 << "," << y2 << endl;
    cairo_set_line_width(c,0.8*10.0*0.5);
    cairo_set_source_rgb(c,0,0,0);
    cairo_move_to(c,x1*10.0,y1*10.0);
    cairo_line_to(c,x2*10.0,y2*10.0);
    */
    cairo_stroke(c);
}

VisuGCode *visu_start()
{
    ImplVisuGCode *ret = new ImplVisuGCode();
    ret->cs = cairo_svg_surface_create("toto.svg",2000.0,2000.0);
    ret->c = cairo_create (ret->cs);
    return ret;
}

/** Deuxième implémentation de graphique */
struct ImplVisuGCode2 : VisuGCode
{
    cairo_t *c;
    cairo_surface_t *cs;
    int nSeg /** Numéro de segment, pour changer de couleur */;

    ImplVisuGCode2() :
        c(NULL),
        cs(NULL),
        nSeg(0) {}

    virtual ~ImplVisuGCode2()
    {
        if (cs)
            cairo_surface_destroy (cs);
        if (c)
            cairo_destroy(c);
    }
    virtual void Flush() {}
    virtual void Segments(const std::vector<std::pair<double,double>>& v);
    virtual void Segment(double x1,double y1,double x2,double y2,int nColor);
    virtual void Point(double x,double y,int nColor);
    /** Met à l'échelle (taille) */
    static double Scale(double sz);
    /** Met à l'échelle (position) retourne l'axe Y */
    static void Scale(double& x,double& y);
};

double ImplVisuGCode2::Scale(double sz)
{
    return sz * 10.0;
}

void ImplVisuGCode2::Scale(double& x,double& y)
{
    x = Scale(x);
    y = (200.0 - y);
    y = Scale(y);
}

void ImplVisuGCode2::Segments(const std::vector<std::pair<double,double>>& v)
{
    cairo_set_line_width(c,Scale(0.7));
    switch (nSeg)
    {
        case 0:
            cairo_set_source_rgba(c,0,1,1,0.7);
            break;
        case 1:
            cairo_set_source_rgba(c,1,1,0,0.7);
            break;
        case 2:
            cairo_set_source_rgba(c,1,0,1,0.7);
            break;
    }
    cairo_set_line_cap(c,CAIRO_LINE_CAP_ROUND);
    for (auto i = v.begin(); i != v.end(); i++)
    {
        double x = i->first;
        double y = i->second;
        Scale(x,y);
        if (i == v.begin())
            cairo_move_to(c,x,y);
        else
            cairo_line_to(c,x,y);
    }
    cairo_stroke(c);

    if (++nSeg == 3)
        nSeg = 0;
}

void ImplVisuGCode2::Segment(double x1,double y1,double x2,double y2,int nColor)
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

void ImplVisuGCode2::Point(double x,double y,int nColor)
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

VisuGCode *FabriqueVisu2()
{
    ImplVisuGCode2 *ret = new ImplVisuGCode2();
    ret->cs = cairo_svg_surface_create("toto2.svg",ret->Scale(200.0),ret->Scale(200.0));
    ret->c = cairo_create (ret->cs);
    return ret;
}
