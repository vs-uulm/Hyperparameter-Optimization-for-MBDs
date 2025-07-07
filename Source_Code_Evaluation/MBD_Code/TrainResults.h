//
// Created by Artur Hermann on 26.11.24.
//

#ifndef TRAINRESULTS_H
#define TRAINRESULTS_H
#include <string>

#include "json.hpp"
#include "LegacyParametersArc1.h"


class TrainResults {
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
    LegacyParametersArc1 parameters;
    //LegacyParametersArc1 test;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(TrainResults, iteration, architecture, pattern, attack, tp, tn, fp, fn, accuracy, precision, recall, f1,parameters)
};

#endif //TRAINRESULTS_H
