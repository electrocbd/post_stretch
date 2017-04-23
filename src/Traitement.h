#ifndef _TRAITEMENT_H
#define _TRAITEMENT_H

/** @file */

#include <vector>
#include "PasGCode.h"

/** Interface de traitement d'une couche de g_code */
struct Traitement
{
    /** Destructeur virtuel pour le polymorphisme */
    virtual ~Traitement() {}
    /** Traitement du g-code
     *
     * @param nLayer Numéro de couche, commençant à 1
     * @param v Ensemble des pas de g-code de la couche courante */
    virtual void Traite(int nLayer,std::vector<PasGCode>& v) = 0;
};

class Params;

/** Fabrique de l'implémentation de l'interface de traitement
 *
 * @param params Paramètres globaux
 *
 * L'appelant doit supprimer l'objet retourné */
Traitement *FabriqueTraitement(const Params& params);

#endif
