#pragma once

#include <iostream>
#include <nlohmann/json.hpp>
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "pnl/pnl_random.h"
#undef OK
#include "OptionMultiFlux.hpp"
#include "BlackScholesModel.hpp"


class BlackScholesPricer {
public:
    PnlMat *vol;
    PnlVect *paymentDates;
    PnlVect *strikes__;
    int nAssets;
    double taux;
    double fdStep;
    int nSamples;
    double T;
    //------------------
    //------------------
    OptionMultiFlux *opt;
    /*
        MultiFluxOptionMultiFlux* opt;
    */
    BlackScholesModel *model__;
    PnlRng *rng;

    BlackScholesPricer(nlohmann::json &jsonParams);

    // Constructeur pour les fichiers test_pricer*
    BlackScholesPricer(
        PnlMat *vol_,
        PnlVect *paymentDates_,
        PnlVect *strikes___,
        double taux_,
        double fdStep_,
        int nSamples_
    );

    ~BlackScholesPricer();
    void priceAndDeltas(
        const PnlMat *past,
        double currentDate,
        bool isMonitoringDate,
        double &price,
        double &priceStdDev,
        PnlVect *&deltas,
        PnlVect *&deltasStdDev
    );

    void print();
};
