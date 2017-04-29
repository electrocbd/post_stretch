#ifndef _GCODESTEP_H
#define _GCODESTEP_H

/** @file */

#include <string>

/** Supported g-code steps
 *
 * */
enum EGCodeStep
{
    GC_NOP /**< Empty ligne or comment */,
    GC_FanOn /**< Fan speed changes */,
    GC_FanOff /**< Fan off */,
    GC_RetractStart /**< Start of retraction, stops extrusion */,
    GC_RetractStop /**< End of retraction, restarts extrusion */,
    GC_MoveFast /**< Fast movement */,
    GC_MoveLin /**< Linear movement */,
    GC_DefinePos /**< Origin redefinition */
};

/** @brief G-Code step */
class GCodeStep
{
    public:
        EGCodeStep m_Step /** GCode step */;

        double m_X /** Current X position */;
        double m_Y /** Current Y position */;
        double m_Z /** Current Z position */;
        double m_E /** Current extrusion position */;
        double m_F /** Speed at the end of the movement */;
        int m_S /** Fan speed */;
        std::string m_Comment /** Comment */;

        GCodeStep() :
            m_X(0),
            m_Y(0),
            m_Z(0),
            m_E(0),
            m_F(0),
            m_S(0),
            m_Step(GC_NOP) {}
};

#endif
