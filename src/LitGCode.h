#ifndef _LITGCODE_H
#define _LITGCODE_H

#include "Traitement.h"

/** Lit de g-code depuis l'entrée standard
 * @param stretch Décalage à apporter en microns
 * @param visu Contrôleur de génération de l'image de débogage
 */
void LitGCode(Traitement *traitement);

#endif

