//
// Created by Artur Hermann on 23.11.24.
//

#ifndef LEGACYPARAMETERSARC1_H
#define LEGACYPARAMETERSARC1_H




#include "Coord.h"
#include "json.hpp"

class LegacyParametersArc1 {
private:
public:
    int iteration;
    double MAX_PLAUSIBLE_SPEED;
    double MAX_PLAUSIBLE_ACCEL;
    double MAX_PLAUSIBLE_DECEL;
    double MAX_PLAUSIBLE_RANGE;
    double MAX_HEADING_CHANGE;
    double MAX_DELTA_INTER;
    double MAX_SA_RANGE;
    double MAX_PLAUSIBLE_DIST;
    double threshold;
    bool optimized;
    std::vector<double> weightLegacyCheck;

    LegacyParametersArc1();
    LegacyParametersArc1(bool optimizedValues) {
        if(optimizedValues == true) {
            //Todo: Specify files
            optimized = true;
            MAX_PLAUSIBLE_SPEED = 20;
            MAX_PLAUSIBLE_ACCEL = 11.5;
            MAX_PLAUSIBLE_DECEL = 16;
            MAX_PLAUSIBLE_RANGE = 420;
            MAX_HEADING_CHANGE = 90;
            MAX_DELTA_INTER = 2.0;
            MAX_SA_RANGE = 420;
            MAX_PLAUSIBLE_DIST = 2;
            threshold = 8.0;
            weightLegacyCheck = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
        }else {
            optimized = false;
            MAX_PLAUSIBLE_SPEED = 20;
            MAX_PLAUSIBLE_ACCEL = 11.5;
            MAX_PLAUSIBLE_DECEL = 16;
            MAX_PLAUSIBLE_RANGE = 420;
            MAX_HEADING_CHANGE = 90;
            MAX_DELTA_INTER = 2.0;
            MAX_PLAUSIBLE_DIST = 2;
            MAX_SA_RANGE = 420;
            weightLegacyCheck = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
        }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(LegacyParametersArc1, MAX_PLAUSIBLE_SPEED, MAX_PLAUSIBLE_ACCEL, MAX_PLAUSIBLE_DECEL, MAX_PLAUSIBLE_RANGE, MAX_HEADING_CHANGE,MAX_DELTA_INTER,MAX_SA_RANGE,threshold,weightLegacyCheck, MAX_PLAUSIBLE_DIST)
};


#endif //LEGACYPARAMETERSARC1_H
