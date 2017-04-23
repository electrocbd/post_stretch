#include "LitGCode.h"
#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <iomanip>
#include "PasGCode.h"

using namespace std;

namespace qi = boost::spirit::qi;
using boost::spirit::qi::grammar;
using qi::char_;
using qi::int_;
using qi::double_;
using qi::eps;
using qi::lit;
using qi::rule;

namespace phx = boost::phoenix;

/** Classe pour écrire le G-Code
 *
 * La classe garde un état interne pour n'écrire les paramètres
 * des ordres de mouvement que lorsque ces paramètres changent
 *
 * Il faut donc appeler la méthode Ecrit dans l'ordre des pas de g-code
 *
 * @todo Utiliser des entiers plutôt que des double?
 */
struct EcritGCode
{
    double m_XCourant;
    double m_YCourant;
    double m_ZCourant;
    double m_ECourant;
    double m_FCourant;

    EcritGCode() :
        m_XCourant(0),
        m_YCourant(0),
        m_ZCourant(0),
        m_ECourant(0),
        m_FCourant(0) {}

    /** Écrit le g-code sur la sortie standard
     *
     * @todo Il faut pouvoir spécifier un flux
     */
    void Ecrit(const PasGCode& pas);
    /** Écriture des paramètres des commandes G0 et G1
     *
     * Il faut écrire le minimum de paramètres pour limiter la volume
     * de données à transmettre. En général, lorsqu'un paramètre est inchangé
     * depuis la précédente commande, il est possible de ne pas le spécifier
     */
    void ParamsG0G1(const PasGCode& pas);
};

void EcritGCode::ParamsG0G1(const PasGCode& pas)
{
    if (m_FCourant != pas.m_F)
        cout << " F" << pas.m_F;
    if (m_XCourant != pas.m_X || m_YCourant != pas.m_Y || m_ZCourant != pas.m_Z)
    {
        cout << " X" << pas.m_X;
        cout << " Y" << pas.m_Y;
    }
    if (m_ZCourant != pas.m_Z)
        cout << " Z" << pas.m_Z;
    if (m_ECourant != pas.m_E)
        cout << " E" << setprecision(10) << pas.m_E;
}

void EcritGCode::Ecrit(const PasGCode& pas)
{
    switch (pas.m_Pas)
    {
        case GC_FanOn:
            cout << "M106 S" << pas.m_S;
            break;
        case GC_FanOff:
            cout << "M107";
            break;
        case GC_RetractStart:
            cout << "G10";
            break;
        case GC_RetractStop:
            cout << "G11";
            break;
        case GC_MoveFast:
            cout << "G0";
            ParamsG0G1(pas);
            break;
        case GC_MoveLin:
            cout << "G1";
            ParamsG0G1(pas);
            break;
        case GC_DefinePos:
            cout << "G92";
            ParamsG0G1(pas);
            break;
    }

    if (pas.m_Comment.size())
        cout << ";" << pas.m_Comment;
    cout << endl;

    m_XCourant = pas.m_X;
    m_YCourant = pas.m_Y;
    m_ZCourant = pas.m_Z;
    m_ECourant = pas.m_E;
    m_FCourant = pas.m_F;
}

/** Données temporaires de l'analyse du fichier gcode
 *
 * Cet objet est utilisé par la grammaire @ref grammaire_gcode
 * lors de son analyse de chaque ligne
 */
struct ParserFichierGCode
{
    /** Numéro de la couche courante */
    int m_nLayer;
    /** Valeur Z de la couche courante */
    double m_ZCouche;
    /** Ensemble des opérations de la couche courante */
    vector<PasGCode> m_vPasCouche;
    Traitement *traitement;
    EcritGCode m_Ecrit;

    ParserFichierGCode(
            Traitement *traitement_) :
        traitement(traitement_),
        m_nLayer(0),
        m_ZCouche(0) {}

    void Commentaire(const vector<char>& v);
    void FanOn(int n);
    void FanOff();
    void DebutRetractation();
    void FinRetractation();
    void ParametreE(double v);
    void MouvementRapide();
    void MouvementLineaire();
    void DefinePos();

    PasGCode m_PasCourant /** Pas en cours de décodage */;

    void FlushPas() /** Écrit le pas courant */;

    /** Termine toutes les opérations accumulées */
    void Flush();
};

void ParserFichierGCode::Flush()
{
    if (m_vPasCouche.size())
    {
        traitement->Traite(++m_nLayer,m_vPasCouche);
        for (auto i = m_vPasCouche.begin() ; i != m_vPasCouche.end(); i++)
            m_Ecrit.Ecrit(*i);
    }
}


void ParserFichierGCode::FlushPas()
{
    if (m_ZCouche != m_PasCourant.m_Z)
    {
        if (m_vPasCouche.size())
        {
            traitement->Traite(++m_nLayer,m_vPasCouche);
            for (auto i = m_vPasCouche.begin() ; i != m_vPasCouche.end(); i++)
            {
                assert(i->m_Z == m_vPasCouche.begin()->m_Z);
                m_Ecrit.Ecrit(*i);
            }
            m_vPasCouche.clear();
        }
        m_ZCouche = m_PasCourant.m_Z;
    }
    m_vPasCouche.push_back(m_PasCourant);

    // Remise à zéro pour le pas suivant
    m_PasCourant.m_Comment.clear();
    m_PasCourant.m_Pas = GC_NOP;
}

void ParserFichierGCode::Commentaire(const vector<char>& v)
{
    m_PasCourant.m_Comment = string(v.begin(),v.end());
}

void ParserFichierGCode::FanOn(int n)
{
    m_PasCourant.m_Pas = GC_FanOn;
    m_PasCourant.m_S = n;
}

void ParserFichierGCode::FanOff()
{
    m_PasCourant.m_Pas = GC_FanOff;
}

void ParserFichierGCode::DebutRetractation()
{
    m_PasCourant.m_Pas = GC_RetractStart;
}

void ParserFichierGCode::FinRetractation()
{
    m_PasCourant.m_Pas = GC_RetractStop;
}

void ParserFichierGCode::ParametreE(double v)
{
    m_PasCourant.m_E = v;
}

void ParserFichierGCode::MouvementRapide()
{
    m_PasCourant.m_Pas = GC_MoveFast;
}

void ParserFichierGCode::MouvementLineaire()
{
    m_PasCourant.m_Pas = GC_MoveLin;
}

void ParserFichierGCode::DefinePos()
{
    m_PasCourant.m_Pas = GC_DefinePos;
}


/** Grammaire d'analyse d'un ligne de g_code
 */
struct grammaire_gcode : grammar<string::iterator>
{
    ParserFichierGCode& data;
    grammaire_gcode(ParserFichierGCode& data_) : grammaire_gcode::base_type(start),data(data_)
    {
        start = -instruction >> -comment >> eps[phx::bind(&ParserFichierGCode::FlushPas,&data)];
    }
    rule<string::iterator> comment =
        (";" >> *char_)[phx::bind(&ParserFichierGCode::Commentaire,&data,qi::_1)];
    rule<string::iterator> param =
        ("X" >> double_)[phx::ref(data.m_PasCourant.m_X) = qi::_1] |
        ("Y" >> double_)[phx::ref(data.m_PasCourant.m_Y) = qi::_1] |
        ("Z" >> double_)[phx::ref(data.m_PasCourant.m_Z) = qi::_1] |
        ("E" >> double_)[phx::bind(&ParserFichierGCode::ParametreE,&data,qi::_1)] |
        ("F" >> double_)[phx::ref(data.m_PasCourant.m_F) = qi::_1]
        ;
    rule<string::iterator> ins_g0 =
        (lit("G0") >> +char_(' ') >> (param % ' '))[phx::bind(&ParserFichierGCode::MouvementRapide,&data)];
    rule<string::iterator> ins_m107 =
        lit("M107")[phx::bind(&ParserFichierGCode::FanOff,&data)];
    rule<string::iterator> ins_g1 =
        ("G1" >> +char_(' ') >> (param % ' '))[phx::bind(&ParserFichierGCode::MouvementLineaire,&data)];
    rule<string::iterator> ins_g10 =
        lit("G10")[phx::bind(&ParserFichierGCode::DebutRetractation,&data)];
    rule<string::iterator> ins_g11 =
        lit("G11")[phx::bind(&ParserFichierGCode::FinRetractation,&data)];
    rule<string::iterator> ins_m106 =
        ("M106" >> +char_(' ') >> "S" >> int_)[phx::bind(&ParserFichierGCode::FanOn,&data,qi::_2)];
    rule<string::iterator> ins_g92 =
        (lit("G92") >> +char_(' ') >> (param % ' '))[phx::bind(&ParserFichierGCode::DefinePos,&data)];
    rule<string::iterator> instruction =
        ins_g0 | ins_m107 | ins_g1 | ins_g10 | ins_g11 | ins_m106 | ins_g92;
    rule<string::iterator> start;
};

void LitGCode(Traitement *traitement)
{
    string str;
    int nLigne = 0;
    ParserFichierGCode data(traitement);
    grammaire_gcode grammaire_gcode_obj(data);
    while (!getline(cin,str).fail())
    {
        ++nLigne;
        if (str.size() && str[str.size() -1] == '\r')
            str.resize(str.size()-1);
        string::iterator it = str.begin();
        bool res = parse(
                it,
                str.end(),
                grammaire_gcode_obj
                );
        if (!res)
        {
            cerr << "Erreur ligne " << nLigne /*<< " fichier " << rbFile.generic_string()*/ << endl;
            throw std::runtime_error("Invalid gcode");
        }
        if (it != str.end())
        {
            cerr << "Ligne " << nLigne << " partielle (" << it-str.begin() << ")" << endl;
            throw std::runtime_error("Invalid gcode");
        }
    }
    data.Flush();
}
