#ifndef _PARAMS_H
#define _PARAMS_H

/** @file */

/** @brief Paramètre généraux */
struct Params
{
    int stretch /** Décalage de correction en microns */;
    int wallWidth /** Largeur de piste en microns */;
    int dumpLayer /** Couche pour laquelle il faut créer une image de débogage, ou 0 */;
    int nozzleDiameter /** Diamètre du bec en microns */;
};

#endif
