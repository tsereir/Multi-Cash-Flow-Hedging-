#pragma once

#include "pnl/pnl_random.h"
#undef OK
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

/**
 * @brief Implémentation d'un modèle de B&S
 *
 */
class BlackScholesModel
{
public:
    int nAssets;
    double taux;
    PnlMat* vol;
    PnlVect* discretisationDates;

    BlackScholesModel(int nAssets_, double taux_, PnlMat* vol_, PnlVect* discretisationDates_);
    ~BlackScholesModel();

    void asset(PnlMat *path,  double t, PnlVect *paymentDates, PnlRng *rng,  const PnlMat *past, bool isMonitoringDate);
/*
    void asset(PnlMat* path, const PnlMat* past, double currentDate, bool isMonitoringDate, int nbTimeSteps, double T, PnlRng* rng);
*/
/*
    void asset_ti(int i, PnlMat* path, double timeStep, PnlVect* normal_vect, int offset);
*/

    void shiftAsset(PnlMat* shift_path, const PnlMat* path, int d, double fdStep, double currentDate);

    


};