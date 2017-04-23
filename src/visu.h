#ifndef _VISU_H
#define _VISU_H

/** @file */

#include <vector>

/** Génération d'un graphique de débogage */
struct VisuGCode
{
    /** Destructeur virtuel, pour utiliser celui de la classe fille */
    virtual ~VisuGCode() {}
    /** Écriture effective de l'image */
    virtual void Flush() = 0;
    /** Dessin de segments */
    virtual void Segments(const std::vector<std::pair<double,double>>& v) = 0;
    /** Dessin de surfaces */
    virtual void Segment(double x1,double y1,double x2,double y2,int nColor) {}
    virtual void Point(double x,double y,int nColor) {}
};



/** Fabrique de la visualisation de débogage
 * L'appelant doit supprimer l'objet à la fin de son utilisation
 *
 * Attention, il s'agit d'une classe polymorphe, il ne faut pas chercher
 * à copier l'objet
 */
VisuGCode *visu_start();
//void segment(double x1,double y1,double x2,double y2);
//void visu_end();

/** Fabrique du deuxième graphique */
VisuGCode *FabriqueVisu2();

#endif
