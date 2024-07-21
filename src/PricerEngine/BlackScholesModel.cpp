#include <iostream>
#include "BlackScholesModel.hpp"
#include "math.h"
using namespace std;

BlackScholesModel::BlackScholesModel(int nAssets_, double taux_, PnlMat* vol_, PnlVect* discretisationDates_)
{
    //paramètres du modèle
    nAssets = nAssets_;
    taux = taux_;
    vol = pnl_mat_copy(vol_);
    discretisationDates = pnl_vect_copy(discretisationDates_);
}
BlackScholesModel::~BlackScholesModel()
{
    //Libération de la mémoire
    pnl_mat_free(&vol);
}

void BlackScholesModel::asset(PnlMat *path,  double t, PnlVect *paymentDates, PnlRng *rng,  const PnlMat *past, bool isMonitoringDate)
{
//Génère une trajectoire du modèle et la stocke dans path
    int d = past->n;
    int size_d = paymentDates-> size;
    double time_ecart = 0;
    double derniereDate = t;
    PnlVect *G = pnl_vect_new();
    pnl_mat_set_subblock(path, past, 0,0);
    int i_suivant = 0;
    //tant que ma date actuelle est superieure au prochain paiement
    while (GET(paymentDates, i_suivant) <= t) {
        i_suivant +=1; }
    PnlVect  *vol_ac = pnl_vect_new();
    for (int t = i_suivant; t< size_d; t++) {
        pnl_vect_rng_normal(G, d, rng);
        time_ecart = GET(paymentDates, t) - derniereDate;
        for (int j=0; j <d; j++) {
            pnl_mat_get_row(vol_ac, vol, j);
            double vol = pnl_vect_norm_two(vol_ac);
            double quantity = (this -> taux - ((vol * vol) / 2)) * time_ecart;
            quantity += sqrt(time_ecart) * pnl_vect_scalar_prod(G, vol_ac);
            quantity = exp(quantity);
            //si on est à la date de monitoring
            if(isMonitoringDate) {
                double s = MGET(path, t-1, j);
                s *= quantity;
                MLET(path, t, j) = s;
            } else {
                //si on est pas a la date de monitoring
                if (t == i_suivant) {
                    double s = MGET(path, t, j);
                    s *= quantity;
                    MLET(path, t, j) = s;
                } else {
                    double s = MGET(path, t-1, j);
                    s *= quantity;
                    MLET(path, t , j) =s;
                }
            }
        }
    derniereDate = GET(paymentDates, t);
    }
    pnl_vect_free(&G);
    pnl_vect_free(&vol_ac);
   
}

void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double fdStep, double currentDate) {
    // Identifier l'indice de temps correspondant à la date actuelle
    int k = 0;
    while (k < discretisationDates->size) {
    if (GET(discretisationDates, k) >= currentDate) {
        break;
    }
        k += 1;
    }

    for (int i = k; i < path->m; i++) {
           MLET(shift_path, i, d) *= (1+fdStep);   
    }
 }