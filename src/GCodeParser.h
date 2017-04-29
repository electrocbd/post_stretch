#ifndef _GCODEPARSER_H
#define _GCODEPARSER_H

#include "StretchAlgorithm.h"
#include <istream>

/** Parse G-Code from the input stream is
 * @param algo Applied algorithm
 */
void GCodeParser(StretchAlgorithm *algo,std::istream& is);

#endif

