//
// Created by Artur Hermann on 02.12.24.
//

#ifndef CATCHPARAMETERSARC1_H
#define CATCHPARAMETERSARC1_H

#include "json.hpp"

class CatchParametersArc1 {
private:
public:
    int iteration;
    double MAX_PLAUSIBLE_SPEED;//
    double MAX_PLAUSIBLE_ACCEL;//
    double MAX_PLAUSIBLE_DECEL;//
    double MAX_PLAUSIBLE_RANGE;//
    double MAX_HEADING_CHANGE;//
    double MAX_DELTA_INTER;//
    double MAX_SA_RANGE;//
    double MAX_PLAUSIBLE_DIST;
    double threshold;
    bool optimized;
    std::vector<double> weightCatchChecks;

    CatchParametersArc1();
    CatchParametersArc1(bool optimizedValues) {
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
            weightCatchChecks = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
        }else {
            optimized = false;
            MAX_PLAUSIBLE_SPEED = 20;
            MAX_PLAUSIBLE_ACCEL = 11.5;
            MAX_PLAUSIBLE_DECEL = 16;
            MAX_PLAUSIBLE_RANGE = 420;
            MAX_HEADING_CHANGE = 90;
            MAX_DELTA_INTER = 2.0;
            MAX_SA_RANGE = 420;
            MAX_PLAUSIBLE_DIST = 2;
            weightCatchChecks = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
        }
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CatchParametersArc1, MAX_PLAUSIBLE_SPEED, MAX_PLAUSIBLE_ACCEL, MAX_PLAUSIBLE_DECEL, MAX_PLAUSIBLE_RANGE, MAX_HEADING_CHANGE,MAX_DELTA_INTER,MAX_SA_RANGE,threshold,weightCatchChecks, MAX_PLAUSIBLE_DIST)
};

#endif //CATCHPARAMETERSARC1_H
