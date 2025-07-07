//
// Created by Artur Hermann on 26.11.24.
//

#ifndef TRAINRESULTSCatch_H
#define TRAINRESULTSCatch_H
#include <string>

#include "json.hpp"
#include "CatchParametersArc1.h"


class TrainResultsCatch {
private:
public:
    int iteration;
    int architecture;
    std::string pattern;
    std::string attack;
    double tp;
    double tn;
    double fp;
    double fn;
    double accuracy;
    double precision;
    double recall;
    double f1;
    CatchParametersArc1 parameters;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TrainResultsCatch, iteration, architecture, pattern, attack, tp, tn, fp, fn, accuracy, precision, recall, f1,parameters)
};

#endif //TRAINRESULTS_H
