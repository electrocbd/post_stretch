#ifndef _PASGCODE_H
#define _PASGCODE_H

/** @file */

#include <string>

/** Type de pas de g-code supportés
 *
 * @todo Gérer G92, utilisé pour remettre à zéro E pour les impressions longues
 * */
enum EPasGCode
{
    GC_NOP /**< Ligne vide ou commentaire seul */,
    GC_FanOn /**< Changement de force de ventilation */,
    GC_FanOff /**< Arrêt de la ventilation */,
    GC_RetractStart /**< Début de rétractation */,
    GC_RetractStop /**< Fin de rétractation */,
    GC_MoveFast /**< Mouvement rapide */,
    GC_MoveLin /**< Mouvement linéaire */,
    GC_DefinePos /**< Redéfinition du point zéro */
};

/** @brief Pas de G-Code */
class PasGCode
{
    public:
        EPasGCode m_Pas /** Type de pas */;

        double m_X /** Valeur courante en X */;
        double m_Y /** Valeur courante en Y */;
        double m_Z /** Valeur courante en Z */;
        double m_E /** Valeur courante de l'extrusion */;
        double m_F /** Vitesse à la fin du mouvement */;
        int m_S /** Force de ventilation */;
        std::string m_Comment /** Commentaire de fin de ligne */;

        PasGCode() :
            m_X(0),
            m_Y(0),
            m_Z(0),
            m_E(0),
            m_F(0),
            m_S(0),
            m_Pas(GC_NOP) {}
};

#endif
