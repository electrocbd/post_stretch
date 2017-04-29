#ifndef _PARAMS_H
#define _PARAMS_H

/** @file */

/** @brief Global parameters */
struct Params
{
    int stretch /** Stretch correction factor in microns */;
    int wallWidth /** Wall width in microns */;
    int dumpLayer /** Layer to debug, or 0 */;
    int nozzleDiameter /** Nozzle diameter in microns */;
};

#endif
