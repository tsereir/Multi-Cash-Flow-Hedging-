#include "OptionMultiFlux.hpp"
#include <cmath>
#include <iostream>
#include "pnl/pnl_vector.h"

using namespace std;

using namespace std;

OptionMultiFlux::OptionMultiFlux(double T_, int nbTimeSteps_, int size_, double r_, PnlVect *strikes___, PnlVect *dates_)
{
    nbTimeSteps = nbTimeSteps_;
    size = size_;
    r = r_;
    strikes__ = pnl_vect_copy(strikes___);
    dates = pnl_vect_copy(dates_);
    //prendre la dernière valeur du vecteur datesd
    T = pnl_vect_get(dates, dates-> size - 1);
}

//si option exercée est > 0 , alors on break, on calcule la valeur actualisée et on l'ajoute au payoff fina
// on fais cela pour tous les flux qui sont versés
double OptionMultiFlux::payoff(const PnlMat* path) {
    double totalPayoff = 0.0;  // otal payoff de l'option
    for (int i = 0; i < dates -> size; ++i) {
        double paymentDate = pnl_vect_get(dates, i);  //Date de paiement actuelle
        double timeFactor = exp(r * (T - paymentDate));  //Facteur d'actualisation

        //Obtenir le prix du sous-jacent à la date de paiement
        double stockPrice = MGET(path, i, i);
        double strikePrice = GET(strikes__, i);  //Prix d'exercice pour la date de paiement

        double fluxPayoff = stockPrice - strikePrice;  //Payoff pour le flux courant

        if (fluxPayoff > 0) {
            totalPayoff = fluxPayoff * timeFactor;
            break;
        }
    }
    return totalPayoff;
}
OptionMultiFlux::~OptionMultiFlux() {
    pnl_vect_free(&strikes__);
    pnl_vect_free(&dates);
}


