#include "Option.hpp"
#include <iostream>
#include <string>
#include <stdio.h>
#include <assert.h>

using namespace std;
//classe fille de Option
class OptionMultiFlux : public Option {
public:
    double r; 
    PnlVect *strikes__; // Strike prices
    PnlVect *dates; // Dates for each strike
    OptionMultiFlux(double T_, int nbTimeSteps_, int size_, double r_, PnlVect *strikes___, PnlVect *dates_);
    double payoff(const PnlMat* path) override;
    virtual ~OptionMultiFlux();
};