#include "StretchAlgorithm.h"
#include <memory>
#include "GCodeDebugView.h"
//#include "clipper/clipper.hpp"
#include <iostream>
#include <assert.h>
#include "microgeo.h"
#include <math.h>
#include "params.h"
#include <sstream>

#define ENABLE_WIDETURN
#define ENABLE_WIDECIRCLE
#define ENABLE_PUSHWALL

using namespace std;

/** Implémentation concrète du traitement d'une couche */
class StretchAlgorithmImpl : public StretchAlgorithm
{
    public:
        struct Segment
        {
            double x1;
            double y1;
            double x2;
            double y2;
            Segment() : x1(0),y1(0),x2(0),y2(0) {}
            Segment(double x1_, double y1_, double x2_, double y2_) : x1(x1_),y1(y1_),x2(x2_),y2(y2_) {}
        };


        StretchAlgorithmImpl(const Params& params_) :
            m_Params(params_) {}
        virtual ~StretchAlgorithmImpl() {}
        virtual void Process(int nLayer,std::vector<GCodeStep>& v);
    private:
        void PushWall(vector<pair<double,double>>& v,
                vector<pair<double,double>>& vTrans,
                GCodeDebugView *debugView);
        void Process(std::vector<GCodeStep>& v,GCodeDebugView *debugView);
        const Params& m_Params /** Paramètres globaux */;
        std::vector<Segment> m_Deposited /** Segments de plastique de la couche courante */;
        void WorkOnSequence(vector<GCodeStep*>& v,GCodeDebugView *debugView);
        string Dump(const GCodeStep& step);
        double CarreDistance(const pair<double,double>& p1,const pair<double,double>& p2);
        /** Corrige un segment aux indices i1 et i2 dans les deux tableaux v (mouvement désiré)
         * et vTrans (mouvement corrigé)
         */
        void CorrigeSegment(vector<pair<double,double>>& v,
                vector<pair<double,double>>& vTrans,
                int i1,
                int i2,
                GCodeDebugView *debugView,
                double d2,
                double d3,
                double d4);
        /** La séquence semble être linéaire
         *
         * @param v Positions d'origine
         * @param vTrans Positions transformées
         * @param debugView Si non nul, traces d'affichage
         */
        void WideTurn(vector<pair<double,double>>& v,
               vector<pair<double,double>>& vTrans,
               GCodeDebugView *debugView);
        /** La séquence semble être circulaire, il est possible de mieux calculer les virages
         *
         * @param v Positions d'origine
         * @param vTrans Positions transformées
         * @param debugView Si non nul, traces d'affichage
         */
        void WideCircle(vector<pair<double,double>>& v,
               vector<pair<double,double>>& vTrans,
               GCodeDebugView *debugView);
        /** Conversion de l'indice i passé en paramètre pour être dans l'intervalle [0:sz-1] */
        static int IndiceCirculaire(int i,int sz);
};

int StretchAlgorithmImpl::IndiceCirculaire(int i,int sz)
{
    while (i < 0)
        i += sz;
    while (i >= sz)
        i -= sz;
    assert(i >= 0 && i < sz);
    return i;
}

double StretchAlgorithmImpl::CarreDistance(const pair<double,double>& p1,const pair<double,double>& p2)
{
    return (p2.first-p1.first)*(p2.first-p1.first) + (p2.second-p1.second)*(p2.second-p1.second);
}

string StretchAlgorithmImpl::Dump(const GCodeStep& step)
{
    ostringstream ss;
    if (step.m_Step == GC_NOP)
    {
        ss << "GC_NOP";
    }
    else if (step.m_Step==GC_NOP)
    {
        ss << "GC_NOP";
    }
    else if (step.m_Step==GC_FanOn)
    {
        ss << "GC_FanOn";
    }
    else if (step.m_Step==GC_FanOff)
    {
        ss << "GC_FanOff";
    }
    else if (step.m_Step==GC_RetractStart)
    {
        ss << "GC_RetractStart";
    }
    else if (step.m_Step==GC_RetractStop)
    {
        ss << "GC_RetractStop";
    }
    else if (step.m_Step==GC_MoveFast)
    {
        ss << "GC_MoveFast";
    }
    else if (step.m_Step==GC_MoveLin)
    {
        ss << "GC_MoveLin X=" << step.m_X << " Y=" << step.m_Y << " E=" << step.m_E;
    }
    else if (step.m_Step==GC_DefinePos)
    {
        ss << "GC_DefinePos";
    }
    return ss.str();
}

void StretchAlgorithmImpl::CorrigeSegment(vector<pair<double,double>>& v,
        vector<pair<double,double>>& vTrans,
        int i1,
        int i2,
        GCodeDebugView *debugView,
        double d2,
        double d3,
        double d4)
{
    /*
     * Je n'ai qu'un segment. S'il n'y a du plastique que d'un seul côté,
     * je décale le segment pour "pousser le mur"
     */
    //double xm = (v[i1].first + v[i2].first) / 2.0;
    double xm = v[i1].first;
    //double ym = (v[i1].second + v[i2].second) / 2.0;
    double ym = v[i1].second;
    //if (debugView)
    //    debugView->Point(xm,ym,0);
    double xperp = -(v[i2].second - v[i1].second); // Coordonnées de la perpendiculaire au segment
    double yperp = (v[i2].first - v[i1].first);
    double dperp = sqrt(xperp*xperp+yperp*yperp); // Norme de la perpendiculaire
    xperp /= dperp;
    yperp /= dperp;
    double xp1 = xm + xperp * d2;
    double yp1 = ym + yperp * d2;
    //if (debugView)
    //    debugView->Point(xp1,yp1,0);
    bool toucheplus = false;
    for (auto j=m_Deposited.begin();!toucheplus && j!=m_Deposited.end();j++)
    {
        if (CarreDistanceSegmentPoint(xp1,yp1,j->x1,j->y1,j->x2,j->y2) <= (d3/2.0)*(d3/2.0))
            toucheplus = true;
    }
    double xp2 = xm - xperp * d2;
    double yp2 = ym - yperp * d2;
    //if (debugView)
    //    debugView->Point(xp2,yp2,0);
    bool touchemoins = false;
    for (auto j=m_Deposited.begin();!touchemoins && j!=m_Deposited.end();j++)
    {
        if (CarreDistanceSegmentPoint(xp2,yp2,j->x1,j->y1,j->x2,j->y2) <= (d3/2.0)*(d3/2.0))
            touchemoins = true;
    }
    /*
     * Je décale vTrans, pour que l'effet soit cumulatif
     */
    if (toucheplus && !touchemoins)
    {
        double xp = vTrans[i1].first + xperp * d4;
        double yp = vTrans[i1].second + yperp * d4;
        assert(xp >= 0 && xp < 200);
        assert(yp >= 0 && yp < 200);
        vTrans[i1].first = floor(xp*1000.0 + 0.5)/1000.0;
        vTrans[i1].second = floor(yp*1000.0 + 0.5)/1000.0;
    }
    if (touchemoins && !toucheplus)
    {
        double xp = vTrans[i1].first - xperp * d4;
        double yp = vTrans[i1].second - yperp * d4;
        assert(xp >= 0 && xp < 200);
        assert(yp >= 0 && yp < 200);
        vTrans[i1].first = floor(xp*1000.0 + 0.5)/1000.0;
        vTrans[i1].second = floor(yp*1000.0 + 0.5)/1000.0;
    }
    if (toucheplus && touchemoins)
    {
        // Vu qu'on est entouré de murs, autant rester sages
        // J'annule toutes les transformations
        vTrans[i1] = v[i1];
    }
}

void StretchAlgorithmImpl::WideTurn(vector<pair<double,double>>& v,
        vector<pair<double,double>>& vTrans,
        GCodeDebugView *debugView)
{
#ifdef ENABLE_WIDETURN
    /*
    if (debugView)
    {
        debugView->Point(v[0].first,v[0].second,1);
        debugView->Point(v[v.size()-1].first,v[v.size()-1].second,2);
    }
    */
    const double d1 = 0.5;
    const double d2 = /*0.7 / 2.0*/ (double)m_Params.wallWidth / 1000.0 / 2.0;
    const double d3 = /*0.8*/ (double)m_Params.nozzleDiameter / 1000.0;
    const double d4 = /*0.17*/(double)m_Params.stretch / 1000.0;
    for (int i=1;i+1<v.size();i++)
    {
        /*
         * Il faut constituer un triangle à partir de trois points du vecteur v
         * La distance entre le premier point A et le deuxième point B doit être
         * supérieure à une distance minimale d1
         * La distance entre le deuxième point B et le troisième point C doit être
         * également supérieure à cette même distance minimale d1
         * À partir de ces trois points, on recherche la direction du virage,
         * et le point D à une distance de la largeur de piste d2 en direction de l'intérieur
         * du virage du point B.
         * Si ce point D contient du plastique, donc s'il est à une distance inférieure
         * à la moitié du diamètre du bec d3 d'un des segments déjà déposés (sans tenir compte
         * de l'éventuelle correction des positions) alors le point milieu B est inchangé
         * Si par contre il n'y a pas de plastique, le point B est déplacé vers le point E,
         * à une distance d4 en direction de l'extérieur du virage
         */
        /*
         * Maintenant, je décale d'office, et le traitement des segments peut
         * me remettre au point de départ
         */
        int i1 = i-1;
        double d;
        while ((d = CarreDistance(v[i1],v[i])) < d1*d1 && i1 > 0)
            i1--;
        int i3 = i+1;
        while ((d = CarreDistance(v[i],v[i3])) < d1*d1 && i3+1 < v.size())
            i3++;
        /*
         * Le triangle est constitué des points aux indices i1, i et i3
         */
        double xp,yp;
        // C'est là que ça se passe :-)
        ExterieurVirage(v[i1].first,v[i1].second,
                v[i].first,v[i].second,
                v[i3].first,v[i3].second,
                d4,
                xp,yp
                );
        assert(xp >= 0 && xp < 200);
        assert(yp >= 0 && yp < 200);
        vTrans[i].first = floor(xp*1000.0 + 0.5)/1000.0;
        vTrans[i].second = floor(yp*1000.0 + 0.5)/1000.0;
        //if (debugView)
        //    debugView->Point(xp,yp,0);

    }
#endif
}

void StretchAlgorithmImpl::WideCircle(vector<pair<double,double>>& v,
        vector<pair<double,double>>& vTrans,
        GCodeDebugView *debugView)
{
#ifdef ENABLE_WIDECIRCLE
    /*
    if (debugView)
    {
        debugView->Point(v[0].first,v[0].second,3);
        debugView->Point(v[v.size()-1].first,v[v.size()-1].second,3);
    }
    */
    const double d1 = 0.5;
    const double d2 = /*0.7 / 2.0*/ (double)m_Params.wallWidth / 1000.0 / 2.0;
    const double d3 = /*0.8*/ (double)m_Params.nozzleDiameter / 1000.0;
    const double d4 = /*0.17*/(double)m_Params.stretch / 1000.0;
    /*
     * Cette fois-ci, il est possible d'utiliser des points en "rebouclant"
     * pour constituer des triangles larges.
     * Le décalage maximal n'est plus l'atteinte des extrémités du vecteur,
     * mais le tiers de la taille du vecteur: Pour une répartition homogène
     * de tous les points, cela fait un triangle équilatéral
     */
    int decMax = v.size()/3;
    for (int i=0;i<v.size();i++)
    {
        /*
         * Il faut constituer un triangle à partir de trois points du vecteur v
         * La distance entre le premier point A et le deuxième point B doit être
         * supérieure à une distance minimale d1
         * La distance entre le deuxième point B et le troisième point C doit être
         * également supérieure à cette même distance minimale d1
         * À partir de ces trois points, on recherche la direction du virage,
         * et le point D à une distance de la largeur de piste d2 en direction de l'intérieur
         * du virage du point B.
         * Si ce point D contient du plastique, donc s'il est à une distance inférieure
         * à la moitié du diamètre du bec d3 d'un des segments déjà déposés (sans tenir compte
         * de l'éventuelle correction des positions) alors le point milieu B est inchangé
         * Si par contre il n'y a pas de plastique, le point B est déplacé vers le point E,
         * à une distance d4 en direction de l'extérieur du virage
         */
        /*
         * Maintenant, je décale d'office, et le traitement des segments peut
         * me remettre au point de départ
         */
        int dec12 = 1; // Décalage en indice du premier point du triangle par rapport au deuxième
        int i1 = IndiceCirculaire(i-dec12,v.size());
        double d;
        while ((d = CarreDistance(v[i1],v[i])) < d1*d1 && dec12 < decMax)
        {
            dec12++;
            i1 = IndiceCirculaire(i-dec12,v.size());
        }
        int dec23 = 1; // Décalage en indice du deuxième point du triangle par rapport au troisième
        int i3 = IndiceCirculaire(i+dec23,v.size());
        while ((d = CarreDistance(v[i],v[i3])) < d1*d1 && dec23 < decMax)
        {
            dec23++;
            i3 = IndiceCirculaire(i+dec23,v.size());
        }
        /*
         * Le triangle est constitué des points aux indices i1, i et i3
         */
        double xp,yp;
        // C'est là que ça se passe :-)
        ExterieurVirage(v[i1].first,v[i1].second,
                v[i].first,v[i].second,
                v[i3].first,v[i3].second,
                d4,
                xp,yp
                );
        assert(xp >= 0 && xp < 200);
        assert(yp >= 0 && yp < 200);
        vTrans[i].first = floor(xp*1000.0 + 0.5)/1000.0;
        vTrans[i].second = floor(yp*1000.0 + 0.5)/1000.0;
        /*
        if (debugView)
            debugView->Point(xp,yp,0);
            */

    }
#endif
}

void StretchAlgorithmImpl::PushWall(vector<pair<double,double>>& v,
        vector<pair<double,double>>& vTrans,
        GCodeDebugView *debugView)
{
#ifdef ENABLE_PUSHWALL
    const double d2 = /*0.7 / 2.0*/ (double)m_Params.wallWidth / 1000.0 / 2.0;
    const double d3 = /*0.8*/ (double)m_Params.nozzleDiameter / 1000.0;
    const double d4 = /*0.17*/(double)m_Params.stretch / 1000.0;
     for (int i=0;i<v.size();i++)
    {
        int i1 = i;
        int i2 = i+1;
        if (i2 == v.size())
            i2 = i-1;
        /*
         * Je n'ai qu'un segment. S'il n'y a du plastique que d'un seul côté,
         * je décale le segment pour "pousser le mur"
         */
        //double xm = (v[i1].first + v[i2].first) / 2.0;
        double xm = v[i1].first;
        //double ym = (v[i1].second + v[i2].second) / 2.0;
        double ym = v[i1].second;
        //if (debugView)
        //    debugView->Point(xm,ym,0);
        double xperp = -(v[i2].second - v[i1].second); // Coordonnées de la perpendiculaire au segment
        double yperp = (v[i2].first - v[i1].first);
        double dperp = sqrt(xperp*xperp+yperp*yperp); // Norme de la perpendiculaire
        xperp /= dperp;
        yperp /= dperp;
        double xp1 = xm + xperp * d2;
        double yp1 = ym + yperp * d2;
        //if (debugView)
        //    debugView->Point(xp1,yp1,0);
        bool toucheplus = false;
        for (auto j=m_Deposited.begin();!toucheplus && j!=m_Deposited.end();j++)
        {
            if (CarreDistanceSegmentPoint(xp1,yp1,j->x1,j->y1,j->x2,j->y2) <= (d3/2.0)*(d3/2.0))
                toucheplus = true;
        }
        double xp2 = xm - xperp * d2;
        double yp2 = ym - yperp * d2;
        //if (debugView)
        //    debugView->Point(xp2,yp2,0);
        bool touchemoins = false;
        for (auto j=m_Deposited.begin();!touchemoins && j!=m_Deposited.end();j++)
        {
            if (CarreDistanceSegmentPoint(xp2,yp2,j->x1,j->y1,j->x2,j->y2) <= (d3/2.0)*(d3/2.0))
                touchemoins = true;
        }
        /*
         * Je décale vTrans, pour que l'effet soit cumulatif
         */
        if (toucheplus && !touchemoins)
        {
            double xp = vTrans[i1].first + xperp * d4;
            double yp = vTrans[i1].second + yperp * d4;
            assert(xp >= 0 && xp < 200);
            assert(yp >= 0 && yp < 200);
            vTrans[i1].first = floor(xp*1000.0 + 0.5)/1000.0;
            vTrans[i1].second = floor(yp*1000.0 + 0.5)/1000.0;
        }
        if (touchemoins && !toucheplus)
        {
            double xp = vTrans[i1].first - xperp * d4;
            double yp = vTrans[i1].second - yperp * d4;
            assert(xp >= 0 && xp < 200);
            assert(yp >= 0 && yp < 200);
            vTrans[i1].first = floor(xp*1000.0 + 0.5)/1000.0;
            vTrans[i1].second = floor(yp*1000.0 + 0.5)/1000.0;
        }
        if (toucheplus && touchemoins)
        {
            // Vu qu'on est entouré de murs, autant rester sages
            // J'annule toutes les transformations
            vTrans[i1] = v[i1];
        }
    }
#endif
}


void StretchAlgorithmImpl::WorkOnSequence(vector<GCodeStep*>& vG,GCodeDebugView *debugView)
{
    vector<pair<double,double>> v; // Original positions, where material should be after cooling
    vector<pair<double,double>> vTrans; // New positions
    for (auto i = vG.begin();i!=vG.end();i++)
    {
        vTrans.push_back(pair<double,double>((*i)->m_X,(*i)->m_Y));
        v.push_back(pair<double,double>((*i)->m_X,(*i)->m_Y));
    }
    if (debugView)
        debugView->Sequences(v,0,(double)m_Params.wallWidth / 1000.0);
    if (v.size() > 2 && CarreDistance(v[0],v[v.size()-1]) < 0.3*0.3) // TODO Un paramètre pour la distance minimale?
        WideCircle(v,vTrans,debugView);
    else
        WideTurn(v,vTrans,debugView);
    PushWall(v,vTrans,debugView);
    for (int i=0;i+1<v.size();i++)
    {
        /*
         * The material positions recorded are the initial positions, because the new positions
         * are temporary. When material cools down, it moves to the initial and wanted positions.
         */
        m_Deposited.push_back(Segment(v[i].first,v[i].second,v[i+1].first,v[i+1].second));
    }
    for (int i=0;i<vG.size();i++)
    {
        if (debugView && (vTrans[i].first != v[i].first || vTrans[i].second != v[i].second))
            debugView->Array(v[i].first,v[i].second,vTrans[i].first,vTrans[i].second);
        /* if (vG[i]->m_X != vTrans[i].first)
           cerr << "pos " << i << " " << vG[i]->m_X << " devient " << vTrans[i].first << endl;*/
        vG[i]->m_X = vTrans[i].first;
        vG[i]->m_Y = vTrans[i].second;

        assert(vG[i]->m_X >= 0 && vG[i]->m_X < 200);
        assert(vG[i]->m_Y >= 0 && vG[i]->m_Y < 200);
    }
}

std::unique_ptr<StretchAlgorithm> StretchAlgorithmFactory(const Params& params)
{
    return unique_ptr<StretchAlgorithm>(new StretchAlgorithmImpl(params));
}

void StretchAlgorithmImpl::Process(std::vector<GCodeStep>& v,GCodeDebugView *debugView)
{
    m_Deposited.clear();
    double curE = 0;
    vector<GCodeStep*> vPos;
    for (auto i = v.begin();i!=v.end();i++)
    {
        if (debugView)
        {
            cerr << "pos " << i-v.begin() << " " << Dump(*i) << endl;
        }
        if (i == v.begin())
        {
            curE = i->m_E;
        }
        if (i->m_E == curE)
        {
            if (debugView && vPos.size())
            {
                cerr << "flush pos " << i-v.begin() << " step " << i->m_Step << endl;
            }
            if (vPos.size() >= 2)
                WorkOnSequence(vPos,debugView);
            vPos.clear();
            vPos.push_back(&*i);
        }
        else if (i->m_Step == GC_MoveFast || i->m_Step == GC_MoveLin)
        {
            vPos.push_back(&*i);
        }
        curE = i->m_E;
    }
    if (vPos.size() >= 2)
    {
        WorkOnSequence(vPos,debugView);
    }
}

void StretchAlgorithmImpl::Process(int nLayer,std::vector<GCodeStep>& v)
{
    if (m_Params.dumpLayer == nLayer)
    {
        unique_ptr<GCodeDebugView> debugView(GCodeDebugViewFactory());
        Process(v,debugView.get());
    }
    else
        Process(v,NULL);

}


