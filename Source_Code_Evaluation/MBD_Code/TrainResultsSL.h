//
// Created by Artur Hermann on 26.11.24.
//

#ifndef TRAINRESULTSSL_H
#define TRAINRESULTSSL_H
#include <string>

#include "json.hpp"
#include "SLParametersArc1.h"


class TrainResultsSL {
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
    SLParametersArc1 parameters;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TrainResultsSL, iteration, architecture, pattern, attack, tp, tn, fp, fn, accuracy, precision, recall, f1,parameters)
};

#endif //TRAINRESULTSSL_H
