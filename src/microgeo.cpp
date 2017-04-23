#include "microgeo.h"
#include <math.h>

double ProduitScalaire(
        double x1,
        double y1,
        double x2,
        double y2)
{
    return x1 * x2 + y1 * y2;
}

double CarreDistanceSegmentPoint(
        double px,
        double py,
        double x1,
        double y1,
        double x2,
        double y2)
{
    // Cf. http://www.faqs.org/faqs/graphics/algorithms-faq/
    double r = ProduitScalaire(px-x1,py-y1,x2-x1,y2-y1) / ProduitScalaire(x2-x1,y2-y1,x2-x1,y2-y1);

    double ppx,ppy;
    if (r < 0) // À l'extérieur du segment du côté du premier point
        r = 0; // Je me place au début du segment
    if (r > 1) // À l'extérieur du segment du côté du dernier point
        r = 1; // Je me place à la fin du segment
    ppx = x1 + r*(x2-x1);
    ppy = y1 + r*(y2-y1); // Coordonnées du point le plus proche

    return ProduitScalaire(ppx-px,ppy-py,ppx-px,ppy-py);
}

double DistanceSegmentPoint(
        double px,
        double py,
        double x1,
        double y1,
        double x2,
        double y2)
{
    return sqrt(CarreDistanceSegmentPoint(px,py,x1,y1,x2,y2));
}

void InterieurVirage(
        double x1,
        double y1,
        double x2,
        double y2,
        double x3,
        double y3,
        double dist,
        double& xp,
        double& yp)
{
    // Il faut trouver la projection du point x2,y2 sur le segment (x1,y1)-(x3,y3)
    double r = ProduitScalaire(x2-x1,y2-y1,x3-x1,y3-y1) / ProduitScalaire(x3-x1,y3-y1,x3-x1,y3-y1);
    double ppx,ppy;
    ppx = x1 + r*(x3-x1);
    ppy = y1 + r*(y3-y1); // Coordonnées de la projection du point milieu sur le segment formé des points extrêmes
    double d1 = sqrt(ProduitScalaire(ppx-x2,ppy-y2,ppx-x2,ppy-y2));
    // d1 est la distance entre le point milieu et sa projection
    // Si la valeur est trop faible, il y a une perte totale de précision,
    // je préfère alors retourner le point milieu
    if (d1 < dist/1000.0)
    {
        xp = x2;
        yp = y2;
        return;
    }
    // Je calcule le point à distance dist du point milieu dans le même axe que la projection
    xp = x2 + (dist/d1)*(ppx-x2);
    yp = y2 + (dist/d1)*(ppy-y2);
}

void ExterieurVirage(
        double x1,
        double y1,
        double x2,
        double y2,
        double x3,
        double y3,
        double dist,
        double& xp,
        double& yp)
{
    // Il faut trouver la projection du point x2,y2 sur le segment (x1,y1)-(x3,y3)
    double rd = ProduitScalaire(x3-x1,y3-y1,x3-x1,y3-y1);
    double r = ProduitScalaire(x2-x1,y2-y1,x3-x1,y3-y1);
    if (fabs(r) < 1000.0 * fabs(rd))
        r /= rd;
    else
        r = 0.5; // Sécurisation lorsque le troisième point est identique au premier
    double ppx,ppy;
    ppx = x1 + r*(x3-x1);
    ppy = y1 + r*(y3-y1); // Coordonnées de la projection du point milieu sur le segment formé des points extrêmes
    double d1 = sqrt(ProduitScalaire(ppx-x2,ppy-y2,ppx-x2,ppy-y2));
    // d1 est la distance entre le point milieu et sa projection
    // Si la valeur est trop faible, il y a une perte totale de précision,
    // je préfère alors retourner le point milieu
    if (d1 < dist/10000.0)
    {
        xp = x2;
        yp = y2;
        return;
    }
    // Je calcule le point à distance dist du point milieu dans le même axe que la projection
    // mais en sens inverse
    xp = x2 - (dist/d1)*(ppx-x2);
    yp = y2 - (dist/d1)*(ppy-y2);
}


