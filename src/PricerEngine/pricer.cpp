#include <iostream>
#include "json_reader.hpp"
#include "pricer.hpp"
#include <ctime>
#include <cmath>

BlackScholesPricer::BlackScholesPricer(nlohmann::json &jsonParams) {
    jsonParams.at("VolCholeskyLines").get_to(vol);
    jsonParams.at("MathPaymentDates").get_to(paymentDates);
    jsonParams.at("Strikes").get_to(strikes__);
    jsonParams.at("DomesticInterestRate").get_to(taux);
    jsonParams.at("RelativeFiniteDifferenceStep").get_to(fdStep);
    jsonParams.at("SampleNb").get_to(nSamples);
    nAssets = vol->n;
    //-------------------------------------------------------------------
    T = pnl_vect_get(paymentDates, nAssets - 1);   
    //nouvelle option multiflux
    opt = new OptionMultiFlux(T, nAssets, nAssets, taux, strikes__, paymentDates);
    //création du modèle
    model__ = new BlackScholesModel(nAssets, taux, vol, paymentDates);
    rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));
}
BlackScholesPricer::~BlackScholesPricer() {
    pnl_vect_free(&paymentDates);
    pnl_vect_free(&strikes__);
    pnl_mat_free(&vol);
    pnl_rng_free(&rng);
}

void BlackScholesPricer::print() {
    std::cout << "nAssets: " << nAssets << std::endl;
    std::cout << "fdStep: " << fdStep << std::endl;
    std::cout << "nSamples: " << nSamples << std::endl;
    std::cout << "strikes__: ";
    pnl_vect_print_asrow(strikes__);
    std::cout << "paymentDates: ";
    pnl_vect_print_asrow(paymentDates);
    std::cout << "vol: ";
    pnl_mat_print(vol);
}

 //Fonction qui renvoie le prix estimé de l'option, l'écart type du prix estimé le delta de l'option
void BlackScholesPricer::priceAndDeltas(const PnlMat *pastData, double currentEvalDate, bool isEvalDate, double &computedPrice, double &priceVariance, PnlVect *&assetDeltas, PnlVect *&deltaVariances) {
    computedPrice = 0.0;
    priceVariance = 0.0;
    assetDeltas = pnl_vect_create_from_zero(nAssets);
    deltaVariances = pnl_vect_create_from_zero(nAssets);
    double totalPayoff = 0.0;
    double totalPayoffSquared = 0.0;
    PnlMat *simulatedPath = pnl_mat_create(opt->nbTimeSteps, nAssets);
    double singleDelta = 0;
    double assetPayoff = 0;
    double assetPayoffUp = 0;
    double assetPayoffDown = 0;
    //boucle sur les simulations
    for (int sampleIndex = 0; sampleIndex < nSamples; sampleIndex++) {
        model__ -> asset(simulatedPath, currentEvalDate, paymentDates, rng,  pastData, isEvalDate);
        assetPayoff = opt->payoff(simulatedPath);
        totalPayoff += assetPayoff;
        totalPayoffSquared += assetPayoff * assetPayoff;
        //boucle sur les actifs
        for (int assetIndex = 0; assetIndex < nAssets; assetIndex++) {
            model__->shiftAsset(simulatedPath, simulatedPath, assetIndex, fdStep, currentEvalDate);
            assetPayoffUp = opt->payoff(simulatedPath);
            model__->shiftAsset(simulatedPath, simulatedPath, assetIndex, (-2 * fdStep) / (1 + fdStep), currentEvalDate);
            assetPayoffDown = opt->payoff(simulatedPath);
        //calcul du delta qui est la dérivée partielle du prix de l'option par rapport au prix de l'actif
            singleDelta = assetPayoffUp - assetPayoffDown;
            model__->shiftAsset(simulatedPath, simulatedPath, assetIndex, fdStep / (1-fdStep), currentEvalDate);
            //mis a jour du vecteur des deltas
            LET(assetDeltas, assetIndex) = GET(assetDeltas, assetIndex) + singleDelta;
            LET(deltaVariances, assetIndex) = GET(deltaVariances, assetIndex) + singleDelta*singleDelta;
        }
    }
    //calcul du prix de l'option
    double discountFactor = exp(-model__->taux * (T - currentEvalDate));
    totalPayoff /= nSamples;
    totalPayoffSquared /= nSamples;
    computedPrice = discountFactor * totalPayoff;
    priceVariance = exp(-2*model__->taux * (T - currentEvalDate)) * (totalPayoffSquared - computedPrice * computedPrice);
    //calcul du delta de l'option
    double avgDeltaFactor = discountFactor / (2 * fdStep * nSamples);
    double avgDeltaSquaredFactor = avgDeltaFactor * avgDeltaFactor * nSamples;
    double spotPrice = 0;
    double varianceAdjustment = 0;
    //boucle sur les actifs
    for (int deltaIndex = 0; deltaIndex < opt->size; deltaIndex++) {
        spotPrice = pnl_mat_get(pastData, pastData->m - 1, deltaIndex);
        singleDelta = pnl_vect_get(assetDeltas, deltaIndex);
        LET(assetDeltas, deltaIndex) = exp(-model__->taux * (opt->T - currentEvalDate)) * (GET(assetDeltas, deltaIndex) / (nSamples * (2 * fdStep*spotPrice)));        
        varianceAdjustment = pnl_vect_get(deltaVariances, deltaIndex) * (avgDeltaSquaredFactor / (spotPrice * spotPrice)) - std::pow(singleDelta / (2 * fdStep * nSamples * spotPrice), 2.0);
        pnl_vect_set(deltaVariances, deltaIndex, sqrt(abs(varianceAdjustment) / nSamples));
    }
    pnl_mat_free(&simulatedPath);
}