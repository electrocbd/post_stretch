#define BOOST_SPIRIT_USE_PHOENIX_V3


#include "GCodeParser.h"
#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <iomanip>
#include "GCodeStep.h"

using namespace std;

namespace qi = boost::spirit::qi;
using boost::spirit::qi::grammar;
using qi::char_;
using qi::int_;
using qi::double_;
using qi::eps;
using qi::lit;
using qi::rule;

namespace phx = boost::phoenix;

/** GCode writer class
 *
The object keeps the values of all parameters (X,Y,Z,E) in order to write only changes

For example, the two following steps:
@verbatim
G01 X10 Y10 Z10
G01 X10 Y11 Z10
@endverbatim

Will be written:

@verbatim
G01 X10 Y10 Z10
G01 Y11
@endverbatim
 */
struct GCodeWriter
{
    double m_CurX;
    double m_CurY;
    double m_CurZ;
    double m_CurE;
    double m_CurF;

    GCodeWriter() :
        m_CurX(0),
        m_CurY(0),
        m_CurZ(0),
        m_CurE(0),
        m_CurF(0) {}

    /** Writes G-Code step
     */
    void Write(const GCodeStep& step);
    /** Write positions part of G0 and G1 commands
     *
     * A position parameter (X,Y,Z,E,F) if printed only if it changed
     * since the previous g-code step
     */
    void ParamsG0G1(const GCodeStep& step);
};

void GCodeWriter::ParamsG0G1(const GCodeStep& step)
{
    if (m_CurF != step.m_F)
        cout << " F" << step.m_F;
    if (m_CurX != step.m_X || m_CurY != step.m_Y || m_CurZ != step.m_Z)
    {
        cout << " X" << step.m_X;
        cout << " Y" << step.m_Y;
    }
    if (m_CurZ != step.m_Z)
        cout << " Z" << step.m_Z;
    if (m_CurE != step.m_E)
        cout << " E" << setprecision(10) << step.m_E;
}

void GCodeWriter::Write(const GCodeStep& step)
{
    switch (step.m_Step)
    {
        case GC_FanOn:
            cout << "M106 S" << step.m_S;
            break;
        case GC_FanOff:
            cout << "M107";
            break;
        case GC_RetractStart:
            cout << "G10";
            break;
        case GC_RetractStop:
            cout << "G11";
            break;
        case GC_MoveFast:
            cout << "G0";
            ParamsG0G1(step);
            break;
        case GC_MoveLin:
            cout << "G1";
            ParamsG0G1(step);
            break;
        case GC_DefinePos:
            cout << "G92";
            ParamsG0G1(step);
            break;
    }

    if (step.m_Comment.size())
        cout << ";" << step.m_Comment;
    cout << endl;

    m_CurX = step.m_X;
    m_CurY = step.m_Y;
    m_CurZ = step.m_Z;
    m_CurE = step.m_E;
    m_CurF = step.m_F;
}

/** Temporary object used by @ref gcode_grammar
 * during the parsing of the gcode input file
 */
struct GCodeFileParser
{
    /** Current layer number */
    int m_nLayer;
    /** Z position of the current layer */
    double m_ZLayer;
    /** GCode steps of the current layer */
    vector<GCodeStep> m_vLayerGCode;
    StretchAlgorithm *algo;
    GCodeWriter m_Writer;

    GCodeFileParser(
            StretchAlgorithm *algo_) :
        algo(algo_),
        m_nLayer(0),
        m_ZLayer(0) {}

    void Comment(const vector<char>& v);

    GCodeStep m_CurrentStep;

    void FlushStep();
    void Flush();
};

void GCodeFileParser::Flush()
{
    if (m_vLayerGCode.size())
    {
        algo->Process(++m_nLayer,m_vLayerGCode);
        for (auto i = m_vLayerGCode.begin() ; i != m_vLayerGCode.end(); i++)
            m_Writer.Write(*i);
    }
}


void GCodeFileParser::FlushStep()
{
    if (m_ZLayer != m_CurrentStep.m_Z)
    {
        if (m_vLayerGCode.size())
        {
            algo->Process(++m_nLayer,m_vLayerGCode);
            for (auto i = m_vLayerGCode.begin() ; i != m_vLayerGCode.end(); i++)
            {
                assert(i->m_Z == m_vLayerGCode.begin()->m_Z);
                m_Writer.Write(*i);
            }
            m_vLayerGCode.clear();
        }
        m_ZLayer = m_CurrentStep.m_Z;
    }
    m_vLayerGCode.push_back(m_CurrentStep);

    // Clear next gcode step
    m_CurrentStep.m_Comment.clear();
    m_CurrentStep.m_Step = GC_NOP;
}

void GCodeFileParser::Comment(const vector<char>& v)
{
    m_CurrentStep.m_Comment = string(v.begin(),v.end());
}

/** Boost.Spirit grammar of a g-code step
 */
struct gcode_grammar : grammar<string::iterator>
{
    GCodeFileParser& data;
    gcode_grammar(GCodeFileParser& data_) : gcode_grammar::base_type(start),data(data_)
    {
        start = -instruction >> -comment >> eps[phx::bind(&GCodeFileParser::FlushStep,&data)];
    }
    rule<string::iterator> comment =
        (";" >> *char_)[phx::bind(&GCodeFileParser::Comment,&data,qi::_1)];
    rule<string::iterator> param =
        ("X" >> double_)[phx::ref(data.m_CurrentStep.m_X) = qi::_1] |
        ("Y" >> double_)[phx::ref(data.m_CurrentStep.m_Y) = qi::_1] |
        ("Z" >> double_)[phx::ref(data.m_CurrentStep.m_Z) = qi::_1] |
        ("E" >> double_)[phx::ref(data.m_CurrentStep.m_E) = qi::_1] |
        ("F" >> double_)[phx::ref(data.m_CurrentStep.m_F) = qi::_1]
        ;
    rule<string::iterator> ins_g0 =
        (lit("G0") >> +char_(' ') >> (param % ' '))[phx::ref(data.m_CurrentStep.m_Step) = GC_MoveFast];
    rule<string::iterator> ins_m107 =
        lit("M107")[phx::ref(data.m_CurrentStep.m_Step) = GC_FanOff];
    rule<string::iterator> ins_g1 =
        ("G1" >> +char_(' ') >> (param % ' '))[phx::ref(data.m_CurrentStep.m_Step) = GC_MoveLin];
    rule<string::iterator> ins_g10 =
        lit("G10")[phx::ref(data.m_CurrentStep.m_Step) = GC_RetractStart];
    rule<string::iterator> ins_g11 =
        lit("G11")[phx::ref(data.m_CurrentStep.m_Step) = GC_RetractStop];
    rule<string::iterator> ins_m106 =
        ("M106" >> +char_(' ') >> "S" >> int_)[phx::ref(data.m_CurrentStep.m_S) = qi::_2][phx::ref(data.m_CurrentStep.m_Step) = GC_FanOn];
    rule<string::iterator> ins_g92 =
        (lit("G92") >> +char_(' ') >> (param % ' '))[phx::ref(data.m_CurrentStep.m_Step) = GC_DefinePos];
    rule<string::iterator> instruction =
        ins_g0 | ins_m107 | ins_g1 | ins_g10 | ins_g11 | ins_m106 | ins_g92;
    rule<string::iterator> start;
};

void GCodeParser(StretchAlgorithm *algo,istream& is)
{
    string str;
    int nLine = 0;
    GCodeFileParser data(algo);
    gcode_grammar gcode_grammar_obj(data);
    while (!getline(is,str).fail())
    {
        ++nLine;
        if (str.size() && str[str.size() -1] == '\r')
            str.resize(str.size()-1);
        string::iterator it = str.begin();
        bool res = parse(
                it,
                str.end(),
                gcode_grammar_obj
                );
        if (!res)
        {
            cerr << "Error line " << nLine << endl;
            throw std::runtime_error("Invalid gcode");
        }
        if (it != str.end())
        {
            cerr << "Line " << nLine << " parsing stopped pos (" << it-str.begin() << ")" << endl;
            throw std::runtime_error("Invalid gcode");
        }
    }
    data.Flush();
}
