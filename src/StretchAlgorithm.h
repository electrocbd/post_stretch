#ifndef _TRAITEMENT_H
#define _TRAITEMENT_H

/** @file */

#include <vector>
#include <memory>
#include "GCodeStep.h"

/** GCode processing algorithm interface */
struct StretchAlgorithm
{
    /** Virtual destructor to allow polymorphism */
    virtual ~StretchAlgorithm() {}
    /** G-Code transform
     *
     * @param nLayer Layer number, starting at 1
     * @param v G-Code steps of the current layer */
    virtual void Process(int nLayer,std::vector<GCodeStep>& v) = 0;
};

class Params;

/** Stretch algorithm factory
 *
 * @param params Global parameters
 */
std::unique_ptr<StretchAlgorithm> StretchAlgorithmFactory(const Params& params);

#endif
