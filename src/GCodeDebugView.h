#ifndef _GCODEDEBUGVIEW_H
#define _GCODEDEBUGVIEW_H

/** @file */

#include <vector>
#include <memory>

/** Generates debug graphic */
struct GCodeDebugView
{
    /** Virtual destructor to allow polymorphism */
    virtual ~GCodeDebugView() {}
    virtual void Sequences(std::vector<std::pair<double,double>>& v,int nColor,double width) = 0;
    virtual void Segment(double x1,double y1,double x2,double y2,int nColor,double width) = 0;
    virtual void Point(double x,double y,int nColor) = 0;
    virtual void Array(double x1,double y1,double x2,double y2) = 0;
};



/** GCode debug image writer factory */
std::unique_ptr<GCodeDebugView> GCodeDebugViewFactory();

#endif
