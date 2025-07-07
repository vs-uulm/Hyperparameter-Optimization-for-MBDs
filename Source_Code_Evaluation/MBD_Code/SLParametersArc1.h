//
// Created by Artur Hermann on 04.12.24.
//

#ifndef SLPARAMETERSARC1_H
#define SLPARAMETERSARC1_H


#include "json.hpp"

class SLParametersArc1 {
private:
public:
    int iteration;

    double maxPlausilbeRange;
    double alpBRP;
    double betBRP;
    double alpDRP;
    double betDRP;

    double maxPlausibleSpeed;
    double alpBSP;
    double betBSP;
    double alpDSP;
    double betDSP;

    double alpBPC;
    double betBPC;
    double alpDPC;
    double betDPC;

    double maxPlausibleAccel;
    double maxPlausibleDecel;
    double alpBSC;
    double betBSC;
    double alpDSC;
    double betDSC;

    double thrPSC;
    double alpBPSC;
    double betBPSC;
    double alpDPSC;
    double betDPSC;

    double maxHeadingChange;
    double alpBPHC;
    double betBPHC;
    double alpDPHC;
    double betDPHC;

    double maxDeltaInter;
    double thrIC;
    double alpBIC;
    double betBIC;
    double alpDIC;
    double betDIC;

    double maxSARange;
    double alpBSA;
    double betBSA;
    double alpDSA;
    double betDSA;

    double thrBelief;
    double thrDisbelief;
    double thrUncertainty;
    bool optimized;
    std::vector<double> weightSLCheck;

    double maxDist;
    double alpBPP;
    double betBPP;
    double alpDPP;
    double betDPP;

    SLParametersArc1() {
        weightSLCheck = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
    };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SLParametersArc1, maxPlausilbeRange, alpBRP, betBRP, alpDRP, betDRP, maxPlausibleSpeed, alpBSP, betBSP, alpDSP, betDSP, alpBPC, betBPC, alpDPC, betDPC,
    maxPlausibleAccel, maxPlausibleDecel, alpBSC, betBSC, alpDSC, betDSC, thrPSC, alpBPSC, betBPSC, alpDPSC, betDPSC, maxHeadingChange, alpBPHC, betBPHC, alpDPHC, betDPHC,
    maxDeltaInter, thrIC, alpBIC, betBIC, alpDIC,betDIC, maxSARange, alpBSA, betBSA, alpDSA, betDSA, thrBelief, thrDisbelief, thrUncertainty, weightSLCheck, maxDist, alpBPP, betBPP, alpDPP, betDPP)
};

#endif //SLPARAMETERSARC1_H
