#ifndef _MICROGEO_H
#define _MICROGEO_H

/** @file Micro librairie de géométrie */

/** Distance entre un point et un segment
 *
 * @param px coordonnée X du point
 * @param py coordonnée Y du point
 * @param x1 coordonnée X du premier point de l'intervalle
 * @param y1 coordonnée Y du premier point de l'intervalle
 * @param x2 coordonnée X du deuxième point de l'intervalle
 * @param y2 coordonnée Y du deuxième point de l'intervalle
 * @return La distance
 */
double DistanceSegmentPoint(
        double px,
        double py,
        double x1,
        double y1,
        double x2,
        double y2);

/** Carré de ma distance entre un point et un segment
 *
 * Cette fonction est plus rapide que @ref DistanceSegmentPoint
 *
 * @param px coordonnée X du point
 * @param py coordonnée Y du point
 * @param x1 coordonnée X du premier point de l'intervalle
 * @param y1 coordonnée Y du premier point de l'intervalle
 * @param x2 coordonnée X du deuxième point de l'intervalle
 * @param y2 coordonnée Y du deuxième point de l'intervalle
 * @return La distance
 */
double CarreDistanceSegmentPoint(
        double px,
        double py,
        double x1,
        double y1,
        double x2,
        double y2);

/** Produit scalaire entre deux vecteur
 *
 * @param x1 Coordonnée X du premier vecteur
 * @param y1 Coordonnée Y du premier vecteur
 * @param x2 Coordonnée X du deuxième vecteur
 * @param y2 Coordonnée Y du deuxième vecteur
 * @return Le produit scalaire
 */
double ProduitScalaire(
        double x1,
        double y1,
        double x2,
        double y2);

/** Calcule le point à l'intérieur du virage des trois points
 * successifs (x1,y1), (x2,y2), (x3,y3) à une distance dist
 *
 * Le point résultant est (xp,yp)
 */
void InterieurVirage(
        double x1,
        double y1,
        double x2,
        double y2,
        double x3,
        double y3,
        double dist,
        double& xp,
        double& yp);

/** Calcule le point à l'extérieur du virage des trois points
 * successifs (x1,y1), (x2,y2), (x3,y3) à une distance dist
 *
 * Le point résultant est (xp,yp)
 */
void ExterieurVirage(
        double x1,
        double y1,
        double x2,
        double y2,
        double x3,
        double y3,
        double dist,
        double& xp,
        double& yp);


#endif
