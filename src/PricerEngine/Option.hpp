#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
//classe abstraite pour les options
class Option
{
public:
    double T;
    int nbTimeSteps;
    int size;
    virtual double payoff(const PnlMat* path) = 0;

};
