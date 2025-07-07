#include <iostream>
#include "LegacyChecks.h"
#include "CatchChecks.h"
#include "Coord.h"
#include "ReadJson.cc"
#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include "NodeKinematicData.h"
#include "NodeKinematicHistoryData.h"
#include "MDMLib.h"
#include "SLChecks.h"
#include <cstdlib> // for atof
#include <getopt.h> // for getopt
#include <fstream>

#include <iostream>
#include <filesystem>

#include "CatchParametersArc1.h"
#include "LegacyParametersArc1.h"
#include "SLParametersArc1.h"
#include "TrainResults.h"
#include "TrainResultsCatch.h"
#include "TrainResultsSL.h"

namespace fs = std::filesystem;


using namespace rapidjson;

veins::Coord STANDARD_CAR_SIZE{4, 2, 2};
int fpCounter[8];
std::array<SLOpinion, 9> slOpinions;
unordered_map<int, NodeKinematicData> messagesGroundTruth;
float timeFirstMessageEgo = 0.0;
std::vector<double> weightLegacyCheck = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
std::vector<std::vector<NodeKinematicData> > alldata;
bool train;
string attackType;

void addNodeHistory(std::map<std::string, NodeKinematicHistoryData> mapHistoryData, Document *d2) {
}


std::array<bool, 10> mbdLegacyChecks(LegacyChecks lc, NodeKinematicData ego, NodeKinematicData external,
                                    NodeKinematicData externalPrevious,
                                    std::map<int, NodeKinematicHistoryData> historyData,
                                    LegacyParametersArc1 legacyParameters) {
    int returnVal = 0;
    int counter = 0;

    std::array<bool, 10> activations = {0};

    MDMLib mdmLib;
    int res = (int) round(lc.RangePlausibilityCheck(&external.pos, &ego.pos, legacyParameters.MAX_PLAUSIBLE_RANGE));
    returnVal = returnVal + res;
    counter++;
    if (res == 0) {
        activations[0] = true;
    }

    res = (int) round(lc.SpeedPlausibilityCheck(mdmLib.calculateSpeed(external.spd),
                                                legacyParameters.MAX_PLAUSIBLE_SPEED));
    returnVal = returnVal + res;
    counter++;
    if (res == 0) {
        activations[1] = true;
    }

    if (externalPrevious.type != 0) {
        res = (int) round(lc.PositionConsistancyCheck(&external.pos, &externalPrevious.pos,
                                                      external.sendTime - externalPrevious.sendTime,
                                                      legacyParameters.MAX_PLAUSIBLE_SPEED));
        returnVal = returnVal + res;
        counter++;
        if (res == 0) {
            activations[2] = true;
        }

        res = (int) round(lc.SpeedConsistancyCheck(mdmLib.calculateSpeed(external.spd),
                                                   mdmLib.calculateSpeed(externalPrevious.spd),
                                                   external.sendTime - externalPrevious.sendTime,
                                                   legacyParameters.MAX_PLAUSIBLE_ACCEL,
                                                   legacyParameters.MAX_PLAUSIBLE_DECEL));
        returnVal = returnVal + res;
        counter++;
        if (res == 0) {
            activations[3] = true;
        }

        res = (int) round(lc.PositionSpeedConsistancyCheck(&external.pos, &externalPrevious.pos,
                                                           mdmLib.calculateSpeed(external.spd),
                                                           mdmLib.calculateSpeed(externalPrevious.spd),
                                                           external.sendTime - externalPrevious.sendTime,
                                                           legacyParameters.MAX_PLAUSIBLE_SPEED,
                                                           legacyParameters.MAX_PLAUSIBLE_ACCEL,
                                                           legacyParameters.MAX_PLAUSIBLE_DECEL));
        returnVal = returnVal + res;
        counter++;
        if (res == 0) {
            activations[4] = true;
        }

        res = (int) round(lc.PositionHeadingConsistancyCheck(&external.hed, &external.pos, &externalPrevious.pos,
                                                             external.sendTime - externalPrevious.sendTime,
                                                             mdmLib.calculateSpeed(external.spd),
                                                             legacyParameters.MAX_HEADING_CHANGE));
        returnVal = returnVal + res;
        counter++;
        if (res == 0) {
            activations[5] = true;
        }
    }

    for (auto const &[key, val]: historyData) {
        NodeKinematicData data = val.dataHistory[0];

        double maxDeltaInter = legacyParameters.MAX_DELTA_INTER;

        if (((external.rcvTime - data.rcvTime) <= maxDeltaInter) and (data.sender != external.sender)) {
            res = (int) round(lc.IntersectionCheck(external.pos, STANDARD_CAR_SIZE, external.hed, data.pos,
                                                   STANDARD_CAR_SIZE, data.hed, external.sendTime - data.sendTime,
                                                   legacyParameters.MAX_DELTA_INTER));
            returnVal = returnVal + res;
            counter++;
            if (res == 0) {
                activations[6] = true;
                break;
            }
        }
    }

    if (external.rcvTime - timeFirstMessageEgo >= 0 and (
            externalPrevious.type == 0 or ((external.rcvTime - externalPrevious.rcvTime) > lc.params->MAX_SA_TIME))) {
        res = (int) round(lc.SuddenAppearenceCheck(&external.pos, &ego.pos, legacyParameters.MAX_SA_RANGE));
        returnVal = returnVal + res;
        counter++;
        if (res == 0) {
            activations[7] = true;
        }
    }

    res = (int)round(lc.PositionPlausibilityCheck(&external.pos, mdmLib.calculateSpeed(external.spd),external.dist,legacyParameters.MAX_PLAUSIBLE_DIST));
    returnVal = returnVal + res;
    counter++;
    if (res == 0) {
        activations[8] = true;
    }

    activations[9] = returnVal != counter;

    return activations;
}



std::array<bool, 10> mbdCatchChecks(Catch_checks cc, NodeKinematicData ego, NodeKinematicData external,
                                   NodeKinematicData externalPrevious,
                                   std::map<int, NodeKinematicHistoryData> historyData, CatchParametersArc1 catchParameters) {
    int returnVal = 0;
    int counter = 0;

    std::array<bool, 10> activations = {0};
    std::array<double, 9> detectorOutput = {0};
    detectorOutput.fill(1);

    MDMLib mdmLib;
    double res = cc.RangePlausibilityCheck(&ego.pos, &ego.pos_noise, &external.pos, &external.pos_noise,
                                                    catchParameters.MAX_PLAUSIBLE_RANGE);

    detectorOutput[0] = res;
    returnVal = returnVal + round(res);
    counter++;
    if (round(res) == 0) {
        activations[0] = true;
    }


    res = cc.SpeedPlausibilityCheck(mdmLib.calculateSpeed(external.spd),
                                                mdmLib.calculateSpeed(external.spd_noise), catchParameters.MAX_PLAUSIBLE_SPEED);
    detectorOutput[1] = res;
    returnVal = returnVal + round(res);
    counter++;
    if (round(res) == 0) {
        activations[1] = true;
    }

    if (externalPrevious.type != 0) {
        res = cc.PositionConsistancyCheck(&external.pos, &external.pos_noise, &externalPrevious.pos,
                                                      &externalPrevious.pos_noise,
                                                      external.sendTime - externalPrevious.sendTime, catchParameters.MAX_PLAUSIBLE_SPEED);
        detectorOutput[2] = res;
        returnVal = returnVal + round(res);
        counter++;
        if (round(res) == 0) {
            activations[2] = true;
        }

        res = cc.SpeedConsistancyCheck(mdmLib.calculateSpeed(external.spd),
                                                   mdmLib.calculateSpeed(external.spd_noise),
                                                   mdmLib.calculateSpeed(externalPrevious.spd),
                                                   mdmLib.calculateSpeed(external.spd_noise),
                                                   external.sendTime - externalPrevious.sendTime, catchParameters.MAX_PLAUSIBLE_ACCEL, catchParameters.MAX_PLAUSIBLE_DECEL);
        detectorOutput[3] = res;
        returnVal = returnVal + round(res);
        counter++;
        if (round(res) == 0) {
            activations[3] = true;
        }

        res = cc.PositionSpeedConsistancyCheck(&external.pos, &external.pos_noise, &externalPrevious.pos,
                                                           &externalPrevious.pos_noise,
                                                           mdmLib.calculateSpeed(external.spd),
                                                           mdmLib.calculateSpeed(external.spd_noise),
                                                           mdmLib.calculateSpeed(externalPrevious.spd),
                                                           mdmLib.calculateSpeed(externalPrevious.spd_noise),
                                                           external.sendTime - externalPrevious.sendTime, catchParameters.MAX_PLAUSIBLE_ACCEL, catchParameters.MAX_PLAUSIBLE_DECEL, catchParameters.MAX_PLAUSIBLE_SPEED);
        detectorOutput[4] = res;
        returnVal = returnVal + round(res);
        counter++;
        if (round(res) == 0) {
            activations[4] = true;
        }

        res = cc.PositionHeadingConsistancyCheck(&external.hed, &external.hed_noise, &externalPrevious.pos,
                                                             &externalPrevious.pos_noise, &external.pos,
                                                             &external.pos_noise,
                                                             external.sendTime - externalPrevious.sendTime,
                                                             mdmLib.calculateSpeed(external.spd),
                                                             mdmLib.calculateSpeed(external.spd_noise), catchParameters.MAX_HEADING_CHANGE);
        detectorOutput[5] = res;
        returnVal = returnVal + round(res);
        counter++;
        if (round(res) == 0) {
            activations[5] = true;
        }
    }

    for (auto const &[key, val]: historyData) {
        NodeKinematicData data = val.dataHistory[0];

        double maxDeltaInter = catchParameters.MAX_DELTA_INTER;

        if (((external.rcvTime - data.rcvTime) <= maxDeltaInter) and (data.sender != external.sender)) {
            res = cc.IntersectionCheck(&external.pos, &external.pos_noise, &data.pos, &data.pos_noise,
                                                   &external.hed, &data.hed, &STANDARD_CAR_SIZE, &STANDARD_CAR_SIZE,
                                                   external.sendTime - data.sendTime, maxDeltaInter);
            detectorOutput[6] = res;
            returnVal = returnVal + round(res);
            counter++;
            if (round(res) == 0) {
                activations[6] = true;
            }
        }
    }

    if (external.rcvTime - timeFirstMessageEgo >= 0 and (
            externalPrevious.type == 0 or ((external.rcvTime - externalPrevious.rcvTime) > cc.params->MAX_SA_TIME))) {
        res = cc.SuddenAppearenceCheck(&ego.pos, &ego.pos_noise, &external.pos, &external.pos_noise,
                                                   catchParameters.MAX_SA_RANGE);
        detectorOutput[7] = res;
        returnVal = returnVal + round(res);
        counter++;
        if (round(res) == 0) {
            activations[7] = true;
        }
    }

    res = cc.PositionPlausibilityCheck( &external.pos, &external.pos_noise,mdmLib.calculateSpeed(external.spd),mdmLib.calculateSpeed(external.spd_noise),external.dist, catchParameters.MAX_PLAUSIBLE_DIST);

    detectorOutput[8] = res;
    returnVal = returnVal + round(res);
    counter++;
    if (round(res) == 0) {
        activations[8] = true;
    }

    activations[9] = returnVal != counter;

    return activations;
}




SLOpinion cumulativeFusion(std::array<SLOpinion, 8> slOpinions) {
    SLOpinion opinion = slOpinions[0];

    for (int i = 1; i < size(slOpinions); i++) {
        if (opinion.uncertainty != 0 || slOpinions[i].uncertainty != 0) {
            opinion.belief = (opinion.belief * slOpinions[i].uncertainty + slOpinions[i].belief * opinion.uncertainty) /
                             (opinion.uncertainty + slOpinions[i].uncertainty - opinion.uncertainty * slOpinions[i].
                              uncertainty);
            opinion.uncertainty = (opinion.uncertainty * slOpinions[i].uncertainty) / (
                                      opinion.uncertainty + slOpinions[i].uncertainty - opinion.uncertainty * slOpinions
                                      [i].uncertainty);
            if (opinion.uncertainty != 0 || slOpinions[i].uncertainty != 0) {
                opinion.baserate = (opinion.baserate * slOpinions[i].uncertainty + slOpinions[i].baserate * opinion.
                                    uncertainty - (opinion.baserate + slOpinions[i].baserate) * opinion.uncertainty *
                                    slOpinions[i].uncertainty) / (
                                       opinion.uncertainty + slOpinions[i].uncertainty - 2 * opinion.uncertainty *
                                       slOpinions[i].uncertainty);
            } else {
                opinion.baserate = (opinion.baserate + slOpinions[i].baserate) / 2;
            }
        } else {
            opinion.belief = 0.5 * opinion.belief + 0.5 * slOpinions[i].belief;
            opinion.disbelief = 1 - opinion.belief;
            opinion.uncertainty = 0;
            opinion.baserate = 0.5 * opinion.baserate + 0.5 * slOpinions[i].baserate;
        }
    }
    return opinion;
}

SLOpinion constraintFusion(std::array<SLOpinion, 8> slOpinions) {
    SLOpinion opinion = slOpinions[0];

    for (int i = 1; i < size(slOpinions); i++) {
        double har = opinion.belief * slOpinions[i].uncertainty + slOpinions[i].belief * opinion.uncertainty + opinion.
                     belief * slOpinions[i].belief;
        double con = opinion.belief * slOpinions[i].disbelief + slOpinions[i].belief * opinion.disbelief;

        if (con == 1) {
            return SLOpinion(0, 0, 1);
        }

        opinion.belief = har / (1 - con);
        opinion.uncertainty = opinion.uncertainty * slOpinions[i].uncertainty / (1 - con);
        opinion.disbelief = 1 - opinion.belief - opinion.uncertainty;

        //opinion.baserate = -1;
        if (opinion.uncertainty + slOpinions[i].uncertainty < 2) {
            opinion.baserate = (opinion.baserate * (1 - opinion.uncertainty) + slOpinions[i].baserate * (
                                    1 - slOpinions[i].uncertainty)) / (
                                   2 - opinion.uncertainty - slOpinions[i].uncertainty);
        } else {
            opinion.baserate = (opinion.baserate + slOpinions[i].baserate) / 2;
        }
    }
    return opinion;
}

SLOpinion multiplication(std::array<SLOpinion, 9> slOpinions) {
    SLOpinion opinion = slOpinions[0];

    for (int i = 1; i < size(slOpinions); i++) {
        opinion.belief = opinion.belief * slOpinions[i].belief + (
                             (1 - opinion.baserate) * slOpinions[i].baserate * opinion.belief * slOpinions[i].
                             uncertainty + opinion.baserate * (1 - slOpinions[i].baserate) * opinion.uncertainty *
                             slOpinions[i].belief) / (1 - opinion.baserate * slOpinions[i].baserate);
        opinion.disbelief = opinion.disbelief + slOpinions[i].disbelief - opinion.disbelief * slOpinions[i].disbelief;
        opinion.uncertainty = opinion.uncertainty * slOpinions[i].uncertainty + (
                                  (1 - slOpinions[i].baserate) * opinion.belief * slOpinions[i].uncertainty + (
                                      1 - opinion.baserate) * opinion.uncertainty * slOpinions[i].belief) / (
                                  1 - opinion.baserate * slOpinions[i].baserate);
        opinion.baserate = opinion.baserate * slOpinions[i].baserate;
    }

    return opinion;
}

SLOpinion weighted_belief_fusion(std::array<SLOpinion, 9> slOpinions, const std::vector<double> &weights) {
    if (slOpinions.size() != weights.size()) {
        throw std::invalid_argument("Number of opinions must match number of weights.");
    }

    double total_weight = std::accumulate(weights.begin(), weights.end(), 0.0);
    if (total_weight == 0) {
        throw std::invalid_argument("Sum of weights must not be zero.");
    }

    // Initialize fused belief, base rate, and uncertainty
    double fused_belief = 0.0;
    double fused_base_rate = 0.0;
    double fused_uncertainty = 0.0;
    double fused_disbelief = 0.0;

    for (size_t i = 0; i < slOpinions.size(); ++i) {
        double weight_normalized = weights[i] / total_weight;
        fused_belief += weight_normalized * slOpinions[i].belief;
        fused_base_rate += weight_normalized * slOpinions[i].baserate;
        fused_uncertainty += weight_normalized * slOpinions[i].uncertainty;
    }

    // Normalize if needed (assuming normalization is needed as in the Python version)
    double normalization_factor = fused_belief + fused_uncertainty;
    if (normalization_factor > 0) {
        fused_belief /= normalization_factor;
        fused_uncertainty /= normalization_factor;
    }

    fused_disbelief = 1 - fused_belief - fused_uncertainty;

    return SLOpinion(fused_belief, fused_disbelief, fused_uncertainty, fused_base_rate);
}


SLOpinion weightedBeliefFusion(std::array<SLOpinion, 9> slOpinions) {
    SLOpinion opinion = slOpinions[0];

    for (int i = 1; i < size(slOpinions); i++) {
        //for (int i=6; i<7; i++) {
        double b1 = opinion.belief;
        double u1 = opinion.uncertainty;
        double a1 = opinion.baserate;

        double b2 = slOpinions[i].belief;
        double u2 = slOpinions[i].uncertainty;
        double a2 = slOpinions[i].baserate;


        if ((u1 != 0 or u2 != 0) and (u1 != 1 or u2 != 1)) {
            opinion.belief = (b1 * (1 - u1) * u2 + b2 * (1 - u2) * u1) / (u1 + u2 - 2 * u1 * u2);
            opinion.uncertainty = (2 - u1 - u2) * u1 * u2 / (u1 + u2 - 2 * u1 * u2);
            opinion.baserate = (a1 * (1 - u1) + a2 * (1 - u2)) / (2 - u1 - u2);
        } else if (u1 == 0 and u2 == 0) {
            opinion.belief = 0.5 * b1 + 0.5 * b2;
            opinion.uncertainty = 0;
            opinion.baserate = 0.5 * a1 + 0.5 * a2;
        } else {
            opinion.belief = 0;
            opinion.uncertainty = 1;
            opinion.baserate = (a1 + a2) / 2;
        }
        opinion.disbelief = 1 - opinion.belief - opinion.uncertainty;
    }

    return opinion;
}


SLOpinion multiSourceFusion(std::array<SLOpinion, 8> opinions) {
    //std::array<SLOpinion,8> opinionsUpdated = createUncertainty(opinions);


    for (int i = 0; i < size(opinions); i++) {
        SLOpinion opinion = opinions[i];
        if (opinion.uncertainty < 0.1) {
            opinion.uncertainty = 0.1;
            if (opinion.belief > 0.1) {
                opinion.belief = 1 - opinion.disbelief - 0.1;
            } else {
                opinion.disbelief = 1 - opinion.belief - 0.1;
            }
        }
        opinions[i] = opinion;
    }

    int n = opinions.size();

    // Variables for fused belief, disbelief, and uncertainty
    double belief_fused = 0.0;
    double disbelief_fused = 0.0;
    double uncertainty_fused = 1.0; // Starting as a product of uncertainties

    // First, calculate the product of all (1 - u_i) for normalization
    double normalization_factor = 1.0;
    for (const auto &op: opinions) {
        normalization_factor *= (1.0 - op.uncertainty);
    }

    // Now, calculate the fused belief and disbelief
    for (int i = 0; i < n; ++i) {
        double product_uncertainties = 1.0;

        // Compute the product of uncertainties for all j ≠ i
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                product_uncertainties *= opinions[j].uncertainty;
            }
        }

        // Contribution of opinion[i] to belief and disbelief
        belief_fused += opinions[i].belief * product_uncertainties;
        disbelief_fused += opinions[i].disbelief * product_uncertainties;

        // Multiply uncertainties for final fused uncertainty
        uncertainty_fused *= opinions[i].uncertainty;
    }

    // Normalize the final belief, disbelief, and uncertainty
    belief_fused /= normalization_factor;
    disbelief_fused /= normalization_factor;
    uncertainty_fused /= normalization_factor;

    // Compute the average base rate (assuming all opinions share the same base rate)
    double avg_baseRate = 0.0;
    for (const auto &op: opinions) {
        avg_baseRate += op.baserate;
    }
    avg_baseRate /= n;

    // Return the fused opinion*/
    SLOpinion fused_opinion = SLOpinion(belief_fused, disbelief_fused, uncertainty_fused, avg_baseRate);

    return fused_opinion;
}


// Multi Source Fusion
SLOpinion cumulativeBeliefFusion(std::array<SLOpinion, 9> &opinions) {
    int n = opinions.size();

    for (int i = 0; i < size(opinions); i++) {
        SLOpinion &opinion = opinions[i];
        if (opinion.uncertainty < 0.1) {
            opinion.uncertainty = 0.1;
            if (opinion.belief > 0.1) {
                opinion.belief = 1 - opinion.disbelief - 0.1;
            } else {
                opinion.disbelief = 1 - opinion.belief - 0.1;
            }
        }
    }

    // Variables for fused belief and normalization factor
    double belief_fused = 0.0;
    double normalization_factor = 1.0;
    double uncertaintySumProduct = 0.0;
    double uncertaintyProduct = 1.0;
    double beliefCase2 = 0.0;

    bool case2 = true;


    // Calculate the fused belief
    for (int i = 0; i < n; i++) {
        if (opinions[i].uncertainty > 0.01) {
            case2 = false;
        }

        double product_uncertainties = 1.0;

        // Compute the product of uncertainties for all j ≠ i
        for (int j = 0; j < n; j++) {
            if (i != j) {
                product_uncertainties *= opinions[j].uncertainty;
            }
        }

        uncertaintyProduct *= opinions[i].uncertainty;

        // Contribution of opinion[i] to the fused belief
        belief_fused += opinions[i].belief * product_uncertainties;
        uncertaintySumProduct += product_uncertainties;

        //Case 2
        beliefCase2 += opinions[i].belief;
    }

    if (case2 == false) {
        belief_fused = belief_fused / (uncertaintySumProduct - (n - 1) * uncertaintyProduct);
        double uncertaintyFused = uncertaintyProduct / (uncertaintySumProduct - (n - 1) * uncertaintyProduct);
        return SLOpinion(belief_fused, 1 - belief_fused - uncertaintyFused, uncertaintyFused);
    } else {
        beliefCase2 = beliefCase2 / n;
        return SLOpinion(beliefCase2, 1 - beliefCase2, 0);
    }
}


// Multi Source Fusion
SLOpinion averageBeliefFusion(std::array<SLOpinion, 8> &opinions) {
    int n = opinions.size();

    for (int i = 0; i < size(opinions); i++) {
        SLOpinion &opinion = opinions[i];
        if (opinion.uncertainty < 0.1) {
            opinion.uncertainty = 0.1;
            if (opinion.belief > 0.1) {
                opinion.belief = 1 - opinion.disbelief - 0.1;
            } else {
                opinion.disbelief = 1 - opinion.belief - 0.1;
            }
        }
    }

    // Variables for fused belief and normalization factor
    double belief_fused = 0.0;
    double normalization_factor = 1.0;
    double uncertaintySumProduct = 0.0;
    double uncertaintyProduct = 1.0;
    double beliefCase2 = 0.0;

    bool case2 = true;


    // Calculate the fused belief
    for (int i = 0; i < n; i++) {
        if (opinions[i].uncertainty > 0.01) {
            case2 = false;
        }

        double product_uncertainties = 1.0;

        // Compute the product of uncertainties for all j ≠ i
        for (int j = 0; j < n; j++) {
            if (i != j) {
                product_uncertainties *= opinions[j].uncertainty;
            }
        }

        uncertaintyProduct *= opinions[i].uncertainty;

        // Contribution of opinion[i] to the fused belief
        belief_fused += opinions[i].belief * product_uncertainties;
        uncertaintySumProduct += product_uncertainties;

        //Case 2
        beliefCase2 += opinions[i].belief;
    }

    if (case2 == false) {
        belief_fused = belief_fused / uncertaintySumProduct;
        double uncertaintyFused = (n * uncertaintyProduct) / uncertaintySumProduct;
        return SLOpinion(belief_fused, 1 - belief_fused - uncertaintyFused, uncertaintyFused);
    } else {
        beliefCase2 = beliefCase2 / n;
        return SLOpinion(beliefCase2, 1 - beliefCase2, 0);
    }
}





// Define Domain Enum
enum class Domain { NIL, TRUE, FALSE, DOMAIN2 };

// Function to compute product of uncertainties
double productOfUncertainties(const std::array<SLOpinion, 9> &opinions) {
    double product = 1.0;
    for (const auto &opinion: opinions) {
        product *= opinion.uncertainty;
    }
    return product;
}

// Function for tabulateOptions placeholder (permutations of domain)
std::vector<std::vector<Domain> > tabulateOptions(int size) {
    std::vector<std::vector<Domain> > options;
    // Placeholder for actual permutations logic
    options.push_back({Domain::TRUE, Domain::FALSE}); // Example
    return options;
}

SLOpinion ccCollectionFusion2(std::array<SLOpinion, 9> &opinions) {
    if (opinions.size() < 2) {
        throw std::invalid_argument("Cannot fuse null opinions, or only one opinion was passed.");
    }

    double baseRate = -1;
    bool first = true;
    for (const auto &so: opinions) {
        if (first) {
            baseRate = so.baserate;
            first = false;
        } else if (baseRate != so.baserate) {
            throw std::runtime_error("Base rates for CC Fusion must be the same.");
        }
    }

    // Step 1: Consensus phase
    double consensusBelief = std::min_element(opinions.begin(), opinions.end(),
                                              [](const SLOpinion &a, const SLOpinion &b) {
                                                  return a.belief < b.belief;
                                              })->belief;
    double consensusDisbelief = std::min_element(opinions.begin(), opinions.end(),
                                                 [](const SLOpinion &a, const SLOpinion &b) {
                                                     return a.disbelief < b.disbelief;
                                                 })->disbelief;
    double consensusMass = consensusBelief + consensusDisbelief;

    std::vector<double> residueBeliefs, residueDisbeliefs, uncertainties;
    for (const auto &so: opinions) {
        residueBeliefs.push_back(std::max(so.belief - consensusBelief, 0.0));
        residueDisbeliefs.push_back(std::max(so.disbelief - consensusDisbelief, 0.0));
        uncertainties.push_back(so.uncertainty);
    }

    // Step 2: Compromise phase
    double productUncertainties = productOfUncertainties(opinions);

    double compromiseBeliefAccumulator = 0;
    double compromiseDisbeliefAccumulator = 0;
    double compromiseXAccumulator = 0; // This is what will later become uncertainty

    // Sub-sum 1
    for (size_t i = 0; i < opinions.size(); ++i) {
        double uWithoutI = productUncertainties / uncertainties[i];
        compromiseBeliefAccumulator += residueBeliefs[i] * uWithoutI;
        compromiseDisbeliefAccumulator += residueDisbeliefs[i] * uWithoutI;
    }

    // Sub-sum 2,3,4 - Permutations logic
    for (const auto &permutation: tabulateOptions(opinions.size())) {
        Domain intersection = std::accumulate(permutation.begin(), permutation.end(), Domain::DOMAIN2,
                                              [](Domain acc, Domain p) { return (acc == p) ? acc : Domain::NIL; });
        Domain unionDomain = std::accumulate(permutation.begin(), permutation.end(), Domain::NIL,
                                             [](Domain acc, Domain p) {
                                                 return (acc == Domain::NIL) ? p : Domain::DOMAIN2;
                                             });

        // Sub-sum 2: intersection of elements in permutation is TRUE
        if (intersection == Domain::TRUE) {
            double prod = 1;
            if (std::find(permutation.begin(), permutation.end(), Domain::DOMAIN2) != permutation.end()) {
                prod = 0;
            } else {
                for (size_t j = 0; j < permutation.size(); ++j) {
                    if (permutation[j] == Domain::TRUE) {
                        prod *= residueBeliefs[j];
                    }
                }
            }
            compromiseBeliefAccumulator += prod;
        } else if (intersection == Domain::FALSE) {
            // Sub-sum 2: add to disbelief
            double prod = 1;
            if (std::find(permutation.begin(), permutation.end(), Domain::DOMAIN2) != permutation.end()) {
                prod = 0;
            } else {
                for (size_t j = 0; j < permutation.size(); ++j) {
                    if (permutation[j] == Domain::FALSE) {
                        prod *= residueDisbeliefs[j];
                    }
                }
            }
            compromiseDisbeliefAccumulator += prod;
        }

        if (unionDomain == Domain::DOMAIN2) {
            // Sub-sum 3: handle union domain as DOMAIN and intersection not NIL
            // Always zero for binary domains
            if (intersection == Domain::NIL) {
                // Sub-sum 4: union TRUE, intersection NIL
                double prod = 1;
                for (size_t j = 0; j < permutation.size(); ++j) {
                    if (permutation[j] == Domain::TRUE) {
                        prod *= residueBeliefs[j];
                    } else if (permutation[j] == Domain::FALSE) {
                        prod *= residueDisbeliefs[j];
                    } else {
                        prod = 0;
                    }
                }
                compromiseBeliefAccumulator += prod;
            }
        } else if (unionDomain == Domain::TRUE) {
            double prod = 1;
            for (size_t j = 0; j < permutation.size(); ++j) {
                if (permutation[j] == Domain::TRUE) {
                    prod *= residueBeliefs[j];
                } else if (permutation[j] == Domain::FALSE) {
                    prod *= residueDisbeliefs[j];
                } else {
                    prod = 0;
                }
            }
            compromiseBeliefAccumulator += prod;
        } else if (unionDomain == Domain::FALSE && intersection == Domain::NIL) {
            // Sub-sum 4: union FALSE, intersection NIL
            double prod = 1;
            for (size_t j = 0; j < permutation.size(); ++j) {
                if (permutation[j] == Domain::FALSE) {
                    prod *= residueDisbeliefs[j];
                } else if (permutation[j] == Domain::TRUE) {
                    prod *= residueBeliefs[j];
                } else {
                    prod = 0;
                }
            }
            compromiseDisbeliefAccumulator += prod;
        }
    }

    double compromiseBelief = compromiseBeliefAccumulator;
    double compromiseDisbelief = compromiseDisbeliefAccumulator;
    double compromiseUncertainty = compromiseXAccumulator;

    double preliminaryUncertainty = productUncertainties;
    double compromiseMass = compromiseBelief + compromiseDisbelief + compromiseUncertainty;

    // Step 3: Normalization phase
    double normalizationFactor = (1 - consensusMass - preliminaryUncertainty) / compromiseMass;

    double fusedUncertainty = preliminaryUncertainty + normalizationFactor * compromiseUncertainty;
    double fusedBelief = consensusBelief + normalizationFactor * compromiseBelief;
    double fusedDisbelief = consensusDisbelief + normalizationFactor * compromiseDisbelief;

    // Create and return the fused opinion
    SLOpinion result(fusedBelief, fusedDisbelief, fusedUncertainty, baseRate);
    return result;
}








bool slOpinionDecisionLogic(SLOpinion opinion, SLParametersArc1 slparams) {
    if(opinion.projectedProb()>=slparams.thrBelief) {
        return false;
    }else {
        return true;
    }

    return true;
}




SLOpinion majorityCollectionFusion(std::array<SLOpinion, 9> slOpinions) {
    int pos=0;
    int neg=0;

    for(int i=0; i^slOpinions.size(); i++) {
        if(slOpinions[i].projectedProb() < slOpinions[i].baserate) {
            neg++;
        }else if (slOpinions[i].projectedProb() > slOpinions[i].baserate) {
            pos++;
        }
    }

    if(pos>neg)
        return SLOpinion(1,0,0,0.5);
    else if(pos<neg)
        return SLOpinion(0,1,0,0.5);
    else
        return SLOpinion(0,0,1,0.5);
}

SLOpinion weightedBeliefFusionMultiSource2(std::array<SLOpinion, 9> slOpinions) {
    double dominator = 0.0;
    double prodUncertainty = 1.0;
    double sumDominator = 0.0;

    for (int i=0; i<slOpinions.size(); i++) {
        double prod = 1.0;
        for(int j=0; j< slOpinions.size(); j++) {
            if(i!=j) {
                prod = prod* slOpinions[j].uncertainty;
            }
        }
        sumDominator += prod;
        prodUncertainty=prodUncertainty*slOpinions[i].uncertainty;
    }

    dominator = sumDominator - slOpinions.size()*prodUncertainty;

    //belief
    double sumNumeratorB = 0.0;
    for (int i=0; i< slOpinions.size(); i++) {
        double prod = 1.0;
        for (int j=0; j<slOpinions.size(); j++) {
            if(i!=j) {
                prod = prod * slOpinions[j].uncertainty;
            }
        }
        sumNumeratorB = sumNumeratorB + slOpinions[i].belief*(1-slOpinions[i].uncertainty)*prod;
    }

    double belief = sumNumeratorB / dominator;

    //uncertainty
    double sumUncertainty = 0.0;
    double prodUncertainty2 = 1.0;
    for (int i=0; i< slOpinions.size(); i++) {
        sumUncertainty+=slOpinions[i].uncertainty;
        prodUncertainty2 = prodUncertainty2*slOpinions[i].uncertainty;
    }

    double numeratorD = (slOpinions.size() - sumUncertainty)*prodUncertainty2;

    double uncertainty = numeratorD / dominator;

    //baserate
    double dominatorBr=0.0;
    double nominatorBr=0.0;
    for(int i=0; i<slOpinions.size(); i++) {
        nominatorBr+=slOpinions[i].baserate*(1-slOpinions[i].uncertainty);
        dominatorBr+=slOpinions[i].uncertainty;
    }

    double baserate = nominatorBr/(slOpinions.size() - dominatorBr);

    return SLOpinion(belief, 1-belief-uncertainty, uncertainty,baserate);
}

SLOpinion mbdSLChecksCatch(SLChecks sl, NodeKinematicData ego, NodeKinematicData external,
                      NodeKinematicData externalPrevious, std::map<int, NodeKinematicHistoryData> historyData, SLParametersArc1 slParams) {
    int returnVal = 0;
    int counter = 0;

    MDMLib mdmLib;
    slOpinions[0] = sl.RangePlausibilityCheckCatch(&ego.pos, &ego.pos_noise, &external.pos, &external.pos_noise, slParams.maxPlausilbeRange, slParams.alpBRP, slParams.betBRP, slParams.alpDRP, slParams.betDRP);
    slOpinions[1] = sl.SpeedPlausibilityCheckCatch(mdmLib.calculateSpeed(external.spd),
                                              mdmLib.calculateSpeed(external.spd_noise), slParams.maxPlausibleSpeed, slParams.alpBSP, slParams.betBSP, slParams.alpDSP, slParams.betDSP);

    if (externalPrevious.type != 0) {
        slOpinions[2] = sl.PositionConsistancyCheckCatch(&external.pos, &external.pos_noise, &externalPrevious.pos, &externalPrevious.pos_noise,
                                                    external.sendTime - externalPrevious.sendTime,slParams.maxPlausibleSpeed, slParams.alpBPC, slParams.betBPC, slParams.alpDPC, slParams.betDPC);
        slOpinions[3] = sl.SpeedConsistancyCheckCatch(mdmLib.calculateSpeed(external.spd),mdmLib.calculateSpeed(external.spd_noise),
                                                  mdmLib.calculateSpeed(externalPrevious.spd),mdmLib.calculateSpeed(externalPrevious.spd_noise),
                                                  external.sendTime - externalPrevious.sendTime, slParams.maxPlausibleAccel, slParams.maxPlausibleDecel, slParams.alpBSC, slParams.betBSC, slParams.alpDSC, slParams.betDSC);
        //slOpinions[4] = sl.PositionSpeedConsistancyCheck(&external.pos,&externalPrevious.pos, mdmLib.calculateSpeed(external.spd), mdmLib.calculateSpeed(externalPrevious.spd), external.sendTime-externalPrevious.sendTime);
        //slOpinions[4] = sl.PositionSpeedConsistancyCheck2(&external.pos,&external.pos_noise,&externalPrevious.pos, &externalPrevious.pos_noise, mdmLib.calculateSpeed(external.spd), mdmLib.calculateSpeed(external.spd_noise), mdmLib.calculateSpeed(externalPrevious.spd), mdmLib.calculateSpeed(externalPrevious.spd_noise),external.sendTime-externalPrevious.sendTime);
        slOpinions[4] = sl.PositionSpeedConsistancyCheckCatch(&external.pos, &external.pos_noise, &externalPrevious.pos,
                                                          &externalPrevious.pos_noise,
                                                          mdmLib.calculateSpeed(external.spd),
                                                          mdmLib.calculateSpeed(external.spd_noise),
                                                          mdmLib.calculateSpeed(externalPrevious.spd),
                                                          mdmLib.calculateSpeed(externalPrevious.spd_noise),
                                                          external.sendTime - externalPrevious.sendTime,slParams.maxPlausibleAccel, slParams.maxPlausibleDecel, slParams.maxPlausibleSpeed, slParams.alpBPSC, slParams.betBPSC, slParams.alpDPSC, slParams.betDPSC);
        slOpinions[5] = sl.PositionHeadingConsistancyCheckCatch(&external.hed, &external.hed_noise, &externalPrevious.pos, &externalPrevious.pos_noise, &external.pos, &external.pos_noise,
                                                           external.sendTime - externalPrevious.sendTime,
                                                           mdmLib.calculateSpeed(external.spd),mdmLib.calculateSpeed(external.spd_noise),slParams.maxHeadingChange,slParams.alpBPHC, slParams.betBPHC, slParams.alpDPHC, slParams.betDPHC);
    } else {
        slOpinions[2] = SLOpinion(0, 0, 1.0);
        slOpinions[3] = SLOpinion(0, 0, 1.0);
        slOpinions[4] = SLOpinion(0, 0, 1.0);
        slOpinions[5] = SLOpinion(0, 0, 1.0);
    }

    SLOpinion opinion = SLOpinion();
    opinion.belief = slParams.alpBIC;
    opinion.disbelief = 0;
    opinion.uncertainty = 1-slParams.alpBIC;
    bool opinionDefault = true;

    double maxDeltaInter = slParams.maxDeltaInter;

    for (auto const &[key, val]: historyData) {
        NodeKinematicData data = val.dataHistory[0];
        if (((external.rcvTime - data.rcvTime) <= maxDeltaInter) and (data.sender != external.sender)) {
            SLOpinion tmpOpinion = sl.IntersectionCheckCatch(&external.pos, &external.pos_noise,&data.pos, &data.pos_noise,&external.hed,&data.hed,&STANDARD_CAR_SIZE, &STANDARD_CAR_SIZE,
                                                                            external.rcvTime - data.rcvTime, slParams.maxDeltaInter,slParams.alpBIC,slParams.betBIC, slParams.alpDIC, slParams.betDIC);
            if ((tmpOpinion.projectedProb() <= opinion.projectedProb()) || opinionDefault == true) {
                opinionDefault = false;
                opinion = tmpOpinion;
            }
        }
    }

    slOpinions[6] = opinion;
    if (external.rcvTime - timeFirstMessageEgo >= sl.params->MAX_SA_TIME and (
            externalPrevious.type == 0 or ((external.rcvTime - externalPrevious.rcvTime) > sl.params->MAX_SA_TIME))) {
        slOpinions[7] = sl.SuddenAppearenceCheckCatch(&ego.pos,&ego.pos_noise, &external.pos, &external.pos_noise, slParams.maxSARange,slParams.alpBSA, slParams.betBSA,slParams.alpDSA, slParams.betDSA);
    } else {
        SLOpinion opinion = SLOpinion();
        opinion.belief = 0;
        opinion.disbelief = 0;
        opinion.uncertainty = 1.0;
        slOpinions[7] = opinion;
    }

    slOpinions[8] = sl.PositionPlausibilityCheck(&external.pos,&external.pos_noise,mdmLib.calculateSpeed(external.spd),mdmLib.calculateSpeed(external.spd_noise),external.dist,slParams.maxDist,slParams.alpBPP,slParams.betBPP,slParams.alpDPP, slParams.betDPP);


    //SLOpinion atl = majorityCollectionFusion(slOpinions);
    //SLOpinion atl = multiplication(slOpinions);
    //SLOpinion atl = ccCollectionFusion2(slOpinions),
    SLOpinion atl = weightedBeliefFusionMultiSource2(slOpinions);

    return atl;

}




std::vector<double> mbdLegacyCatchChecks(LegacyChecks lc, Catch_checks cc,
                                         string checktype, bool basicValues,
                                         LegacyParametersArc1 legacyParameters, string checkAttribute, CatchParametersArc1 catchParameters) {
    int tp = 0;
    int tn = 0;
    int fp = 0;
    int fn = 0;

    NodeKinematicData ego;
    NodeKinematicData external;

    std::map<int, NodeKinematicHistoryData> mapHistoryData;

    std::vector<double> returnArray;

    for (int i = 0; i < size(alldata); i++) {
        std::vector<NodeKinematicData> allDataPerFile = alldata[i];
        int counter = 0;

        mapHistoryData.clear();


        for (int j = 0; j < size(allDataPerFile); j++) {
            counter++;
            NodeKinematicData m = allDataPerFile[j];

            if (m.type == 2) {
                ego = m;
            } else {
                external = m;
                std::array<bool, 10> activations;
                if (checktype == "legacy") {
                    activations = mbdLegacyChecks(lc, ego, external,
                                                  mapHistoryData.find(m.senderPseudo)->second.dataHistory[0],
                                                  mapHistoryData, legacyParameters);
                } else {
                    activations = mbdCatchChecks(cc, ego, external,
                                                 mapHistoryData.find(m.senderPseudo)->second.dataHistory[0],
                                                 mapHistoryData, catchParameters);
                }

                // Check malicious
                NodeKinematicData groundTruth = messagesGroundTruth[external.messageID];
                bool malicious = false;

                if(fabs(groundTruth.pos.x - external.pos.x) > 1 or fabs(groundTruth.pos.y - external.pos.y) > 1) {
                    malicious = true;
                }
                if(fabs(groundTruth.spd.x - external.spd.x) > 1 or fabs(groundTruth.spd.y - external.spd.y) > 1) {
                    malicious = true;
                }

                /*if (checkAttribute == "position") {
                    malicious = (groundTruth.pos.x != external.pos.x) or (groundTruth.pos.y != external.pos.y) or (
                                    groundTruth.pos.z != external.pos.z);
                } else {
                    malicious = (groundTruth.spd.x != external.spd.x) or (groundTruth.spd.y != external.spd.y) or (
                                    groundTruth.spd.z != external.spd.z);
                }*/

                if (activations[9] == true) {
                    if (malicious == true) {
                        tp = tp + 1;
                    } else {
                        fp = fp + 1;
                        //checkFPs(activations);
                    }
                } else {
                    if (malicious == true) {
                        fn = fn + 1;
                    } else {
                        tn = tn + 1;
                    }
                }
            }

            NodeKinematicHistoryData historyData;
            if (m.type == 2) {
                auto kinematicData = mapHistoryData.find(0);
                if (kinematicData != mapHistoryData.end()) {
                    historyData = kinematicData->second;
                    historyData.addKinematicData(ego);
                    mapHistoryData[0] = historyData;
                } else {
                    historyData = NodeKinematicHistoryData();
                    historyData.addKinematicData(ego);
                    mapHistoryData[0] = historyData;
                }
            } else {
                int senderID = external.senderPseudo;
                auto kinematicData = mapHistoryData.find(senderID);

                if (kinematicData != mapHistoryData.end()) {
                    historyData = kinematicData->second;
                    historyData.addKinematicData(external);
                    mapHistoryData[senderID] = historyData;
                } else {
                    historyData = NodeKinematicHistoryData();
                    historyData.addKinematicData(external);
                    mapHistoryData[senderID] = historyData;
                }
            }
        }
    }

    returnArray.push_back(tp);
    returnArray.push_back(tn);
    returnArray.push_back(fp);
    returnArray.push_back(fn);

    return returnArray;
}


std::vector<double> mbdSLChecksPrep(SLChecks sl, SLParametersArc1 slParameters, string checkAttribute) {
    int tp = 0;
    int tn = 0;
    int fp = 0;
    int fn = 0;

    //double rtl = slParameters.threshold;
    NodeKinematicData ego;
    NodeKinematicData external;

    std::map<int, NodeKinematicHistoryData> mapHistoryData;

    std::vector<double> returnArray;

    for (int i = 0; i < size(alldata); i++) {
        std::vector<NodeKinematicData> allDataPerFile = alldata[i];
        int counter = 0;

        mapHistoryData.clear();

        for (int j = 0; j < size(allDataPerFile); j++) {
            counter++;
            NodeKinematicData m = allDataPerFile[j];

            if (m.type == 2) {
                ego = m;
                if(timeFirstMessageEgo < 0) {
                    timeFirstMessageEgo=ego.rcvTime;
                }
            } else {
                auto data = mapHistoryData.find(m.senderPseudo);
                SLOpinion opinion;
                external = m;

                if (data != mapHistoryData.end()) {
                    opinion = mbdSLChecksCatch(sl, ego, external, data->second.dataHistory[0], mapHistoryData,slParameters);
                } else {
                    opinion = mbdSLChecksCatch(sl, ego, external, NodeKinematicData(), mapHistoryData, slParameters);
                }


                // Check malicious
                NodeKinematicData groundTruth = messagesGroundTruth[external.messageID];
                bool malicious = false;

                if(fabs(groundTruth.pos.x - external.pos.x) > 1 or fabs(groundTruth.pos.y - external.pos.y) > 1) {
                    malicious = true;
                }
                if(fabs(groundTruth.spd.x - external.spd.x) > 1 or fabs(groundTruth.spd.y - external.spd.y) > 1) {
                    malicious = true;
                }

                bool output;

                output = slOpinionDecisionLogic(opinion,slParameters);

                if (output == true) {
                    if (malicious == true) {
                        tp = tp + 1;
                    } else {
                        fp = fp + 1;
                        //checkFPs(activations);
                    }
                } else {
                    if (malicious == true) {
                        fn = fn + 1;
                    } else {
                        tn = tn + 1;
                    }
                }
            }

            NodeKinematicHistoryData historyData;
            if (m.type == 2) {
                auto kinematicData = mapHistoryData.find(0);
                if (kinematicData != mapHistoryData.end()) {
                    historyData = kinematicData->second;
                    historyData.addKinematicData(ego);
                    mapHistoryData[0] = historyData;
                } else {
                    historyData = NodeKinematicHistoryData();
                    historyData.addKinematicData(ego);
                    mapHistoryData[0] = historyData;
                }
            } else {
                int senderID = external.senderPseudo;
                auto kinematicData = mapHistoryData.find(senderID);

                if (kinematicData != mapHistoryData.end()) {
                    historyData = kinematicData->second;
                    historyData.addKinematicData(external);
                    mapHistoryData[senderID] = historyData;
                } else {
                    historyData = NodeKinematicHistoryData();
                    historyData.addKinematicData(external);
                    mapHistoryData[senderID] = historyData;
                }
            }
        }
    }

    returnArray.push_back(tp);
    returnArray.push_back(tn);
    returnArray.push_back(fp);
    returnArray.push_back(fn);

    return returnArray;
}

std::vector<std::vector<NodeKinematicData> > readAllData(const std::string &path, bool train) {
    std::vector<std::vector<NodeKinematicData> > allFileData; // To hold arrays for each file
    int fileCounter = 0;
    int numFiles = 100;
    int numLines = 300;

    for (const auto &entry: std::filesystem::directory_iterator(path)) {
        fileCounter++;
        if (fileCounter > numFiles && train) {
            break;
        }

        if (entry.path().filename() == ".DS_Store" ||
            entry.path().filename().string().rfind("traceGroundTruthJSON", 0) == 0) {
            continue; // Skip macOS system files and ground truth files
        }

        if (entry.is_regular_file()) {
            std::ifstream inputFile(entry.path().string());
            if (!inputFile.is_open()) {
                std::cerr << "Failed to open file: " << entry.path().string() << std::endl;
                continue;
            }

            std::vector<NodeKinematicData> fileData; // Array for this file's data
            std::string line;
            int lineCounter = 0;
            while (std::getline(inputFile, line)) {
                lineCounter++;
                if (lineCounter > numLines && train) {
                    break;
                }

                // Parse JSON
                rapidjson::Document d;
                d.Parse(line.c_str());
                if (d.HasParseError()) {
                    std::cerr << "Error parsing JSON in file: " << entry.path().string() << std::endl;
                    continue;
                }

                NodeKinematicData data;
                data.type = d.HasMember("type") ? d["type"].GetInt() : -1;
                data.rcvTime = d.HasMember("rcvTime") ? d["rcvTime"].GetDouble() : 0.0;
                data.sendTime = d.HasMember("sendTime") ? d["sendTime"].GetDouble() : 0.0;
                data.sender = d.HasMember("sender") ? d["sender"].GetInt() : -1;
                data.senderPseudo = d.HasMember("senderPseudo") ? d["senderPseudo"].GetInt() : -1;
                data.messageID = d.HasMember("messageID") ? d["messageID"].GetInt() : -1;
                data.dist = d.HasMember("dist") ? d["dist"].GetDouble() : 0.0;

                if (d.HasMember("pos") && d["pos"].IsArray()) {
                    data.pos = {d["pos"][0].GetDouble(), d["pos"][1].GetDouble(), d["pos"][2].GetDouble()};
                }
                if (d.HasMember("pos_noise") && d["pos_noise"].IsArray()) {
                    data.pos_noise = {
                        d["pos_noise"][0].GetDouble(), d["pos_noise"][1].GetDouble(), d["pos_noise"][2].GetDouble()
                    };
                }
                if (d.HasMember("spd") && d["spd"].IsArray()) {
                    data.spd = {d["spd"][0].GetDouble(), d["spd"][1].GetDouble(), d["spd"][2].GetDouble()};
                }
                if (d.HasMember("spd_noise") && d["spd_noise"].IsArray()) {
                    data.spd_noise = {
                        d["spd_noise"][0].GetDouble(), d["spd_noise"][1].GetDouble(), d["spd_noise"][2].GetDouble()
                    };
                }
                if (d.HasMember("acl") && d["acl"].IsArray()) {
                    data.acl = {d["acl"][0].GetDouble(), d["acl"][1].GetDouble(), d["acl"][2].GetDouble()};
                }
                if (d.HasMember("acl_noise") && d["acl_noise"].IsArray()) {
                    data.acl_noise = {
                        d["acl_noise"][0].GetDouble(), d["acl_noise"][1].GetDouble(), d["acl_noise"][2].GetDouble()
                    };
                }
                if (d.HasMember("hed") && d["hed"].IsArray()) {
                    data.hed = {d["hed"][0].GetDouble(), d["hed"][1].GetDouble(), d["hed"][2].GetDouble()};
                }
                if (d.HasMember("hed_noise") && d["hed_noise"].IsArray()) {
                    data.hed_noise = {
                        d["hed_noise"][0].GetDouble(), d["hed_noise"][1].GetDouble(), d["hed_noise"][2].GetDouble()
                    };
                }

                // Add parsed data to this file's array
                fileData.push_back(data);
            }

            // Add this file's data to the overall array
            allFileData.push_back(fileData);
        }
    }

    return allFileData;
}


std::vector<std::vector<NodeKinematicData> > deserializeAllFileData(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open file for reading.");
    }

    std::vector<std::vector<NodeKinematicData> > allFileData;

    // Read the outer vector size
    size_t outerSize;
    ifs.read(reinterpret_cast<char *>(&outerSize), sizeof(outerSize));
    allFileData.resize(outerSize);

    // Read each inner vector
    for (size_t i = 0; i < outerSize; ++i) {
        size_t innerSize;
        ifs.read(reinterpret_cast<char *>(&innerSize), sizeof(innerSize));
        allFileData[i].resize(innerSize);

        for (size_t j = 0; j < innerSize; ++j) {
            allFileData[i][j].deserialize(ifs);
        }
    }

    return allFileData;
}

std::unordered_map<int, NodeKinematicData> deserializeMessagesGroundTruth(
    const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Failed to open file for reading.");
    }

    std::unordered_map<int, NodeKinematicData> messagesGroundTruth;

    // Read the size of the unordered_map
    size_t mapSize;
    ifs.read(reinterpret_cast<char *>(&mapSize), sizeof(mapSize));

    // Read each key-value pair
    for (size_t i = 0; i < mapSize; ++i) {
        int key;
        NodeKinematicData value;

        // Deserialize the key
        ifs.read(reinterpret_cast<char *>(&key), sizeof(key));

        // Deserialize the value (NodeKinematicData)
        value.deserialize(ifs);

        // Add the key-value pair to the map
        messagesGroundTruth[key] = value;
    }

    return messagesGroundTruth;
}

vector<double> legacyPerformanceAnalysis(int argc, char *argv[], LegacyChecks lc, Catch_checks cc, bool train,
                                         string path, string pathGroundTruth, bool optimized,
                                         LegacyParametersArc1 legacyParameters, string checkAttribute, CatchParametersArc1 catchParameters) {
    //readAndStoreGroundTruth(pathGroundTruth);
    messagesGroundTruth = deserializeMessagesGroundTruth(pathGroundTruth);
    //readAllData(path,train);
    alldata = deserializeAllFileData(path);


    vector<double> returnValues = mbdLegacyCatchChecks(lc, cc, "legacy", true, legacyParameters, checkAttribute, catchParameters);

    return returnValues;
}

vector<double> catchPerformanceAnalysis(int argc, char *argv[], LegacyChecks lc, Catch_checks cc, bool train,
                                         string path, string pathGroundTruth, bool optimized,
                                         LegacyParametersArc1 legacyParameters, string checkAttribute, CatchParametersArc1 catchParameters) {
    //readAndStoreGroundTruth(pathGroundTruth);
    messagesGroundTruth = deserializeMessagesGroundTruth(pathGroundTruth);
    //readAllData(path,train);
    alldata = deserializeAllFileData(path);


    vector<double> returnValues = mbdLegacyCatchChecks(lc, cc, "catch", true, legacyParameters, checkAttribute,catchParameters);

    return returnValues;
}

vector<double> slPerformanceAnalysis(SLChecks sl, string path, string pathGroundTruth, SLParametersArc1 slParameters,
    string checkAttribute) {
    //readAndStoreGroundTruth(pathGroundTruth);
    messagesGroundTruth = deserializeMessagesGroundTruth(pathGroundTruth);
    //readAllData(path,train);
    alldata = deserializeAllFileData(path);

    vector<double> returnValues = mbdSLChecksPrep(sl,slParameters,checkAttribute);

    return returnValues;
}


vector<double> calcStatistic(vector<double> returnValues, bool train) {
    double tp = returnValues[0];
    double tn = returnValues[1];
    double fp = returnValues[2];
    double fn = returnValues[3];

    double accuracy = (double) (tp + tn) / (tp + tn + fp + fn);
    double precision = 0.0;
    if (tp + fp != 0) {
        precision = (double) tp / (tp + fp);
    }

    double recall = 0.0;
    if (tp + fn != 0) {
        recall = (double) tp / (tp + fn);
    }

    double f1 = 0;
    if (precision + recall > 0.0) {
        f1 = (double) 2 * (precision * recall) / (precision + recall);
    }

    /*if(train == false) {
        cout << "tp: " << tp << " tn: " << tn << " fp: " << fp << " f‚n: " << fn << endl;
        std::cout << accuracy << endl;
        std::cout << precision << endl;
        std::cout << recall << endl;
        std::cout << f1 << endl;
    }*/

    return vector<double>{accuracy, precision, recall, f1};
}

void setLegacyParameters(int argc, char *argv[], LegacyParametersArc1 &legacyParameters, bool optimizedValues) {
    std::map<std::string, double> parameters;

    // Parse command-line arguments, expecting pairs of flags and values
    for (int i = 1; i < argc; i += 2) {
        std::string flag(argv[i]);
        if (flag == "-D") {
            continue;
        }
        double value = std::stod(argv[i + 1]);
        parameters[flag] = value;
    }

    if (optimizedValues == true) {
        // Example: Accessing the parameters (for debugging or computation)
        legacyParameters.MAX_PLAUSIBLE_RANGE = parameters["-a"];
        legacyParameters.MAX_PLAUSIBLE_SPEED = parameters["-b"];
        legacyParameters.MAX_PLAUSIBLE_ACCEL = parameters["-c"];
        legacyParameters.MAX_PLAUSIBLE_DECEL = parameters["-d"];
        legacyParameters.MAX_HEADING_CHANGE = parameters["-e"];
        legacyParameters.MAX_DELTA_INTER = parameters["-f"];
        legacyParameters.MAX_SA_RANGE = parameters["-g"];
        legacyParameters.MAX_PLAUSIBLE_DIST = parameters["-u"];
        legacyParameters.weightLegacyCheck[0] = parameters["-h"];
        legacyParameters.weightLegacyCheck[1] = parameters["-i"];
        legacyParameters.weightLegacyCheck[2] = parameters["-j"];
        legacyParameters.weightLegacyCheck[3] = parameters["-k"];
        legacyParameters.weightLegacyCheck[4] = parameters["-l"];
        legacyParameters.weightLegacyCheck[5] = parameters["-m"];
        legacyParameters.weightLegacyCheck[6] = parameters["-n"];
        legacyParameters.weightLegacyCheck[7] = parameters["-o"];
        legacyParameters.weightLegacyCheck[8] = parameters["-t"];
        legacyParameters.threshold = parameters["-p"];
        legacyParameters.iteration = parameters["-q"];
    }

    train = bool(parameters["-r"]);
    int attackTypeIdx = int(parameters["-s"]);

    if (attackTypeIdx == 1) {
        attackType = "const_pos";
    } else if (attackTypeIdx == 2) {
        attackType = "rand_speed";
    } else if (attackTypeIdx == 3) {
        attackType = "sudden_stop";
    } else if (attackTypeIdx == 4) {
        attackType = "mixed";
    }
}

void setCatchParameters(int argc, char *argv[], CatchParametersArc1 &catchParameters, bool optimizedValues) {
    std::map<std::string, double> parameters;

    // Parse command-line arguments, expecting pairs of flags and values
    for (int i = 1; i < argc; i += 2) {
        std::string flag(argv[i]);
        if (flag == "-D") {
            continue;
        }
        double value = std::stod(argv[i + 1]);
        parameters[flag] = value;
    }

    if (optimizedValues == true) {
        // Example: Accessing the parameters (for debugging or computation)
        catchParameters.MAX_PLAUSIBLE_RANGE = parameters["-a"];
        catchParameters.MAX_PLAUSIBLE_SPEED = parameters["-b"];
        catchParameters.MAX_PLAUSIBLE_ACCEL = parameters["-c"];
        catchParameters.MAX_PLAUSIBLE_DECEL = parameters["-d"];
        catchParameters.MAX_HEADING_CHANGE = parameters["-e"];
        catchParameters.MAX_DELTA_INTER = parameters["-f"];
        catchParameters.MAX_SA_RANGE = parameters["-g"];
        catchParameters.weightCatchChecks[0] = parameters["-h"];
        catchParameters.weightCatchChecks[1] = parameters["-i"];
        catchParameters.weightCatchChecks[2] = parameters["-j"];
        catchParameters.weightCatchChecks[3] = parameters["-k"];
        catchParameters.weightCatchChecks[4] = parameters["-l"];
        catchParameters.weightCatchChecks[5] = parameters["-m"];
        catchParameters.weightCatchChecks[6] = parameters["-n"];
        catchParameters.weightCatchChecks[7] = parameters["-o"];
        catchParameters.weightCatchChecks[8] = parameters["-t"];
        catchParameters.threshold = parameters["-p"];
        catchParameters.iteration = parameters["-q"];
        catchParameters.MAX_PLAUSIBLE_DIST = parameters["-u"];
    }

    train = bool(parameters["-r"]);
    int attackTypeIdx = int(parameters["-s"]);

    if (attackTypeIdx == 1) {
        attackType = "const_pos";
    } else if (attackTypeIdx == 2) {
        attackType = "rand_speed";
    } else if (attackTypeIdx == 3) {
        attackType = "sudden_stop";
    } else if (attackTypeIdx == 4) {
        attackType = "mixed";
    }
}

void setSLParameters(int argc, char *argv[], SLParametersArc1 &slParameters) {
    std::map<std::string, double> parameters;

    // Parse command-line arguments, expecting pairs of flags and values
    for (int i = 1; i < argc; i += 2) {
        std::string flag(argv[i]);
        if (flag == "-D") {
            continue;
        }
        double value = std::stod(argv[i + 1]);
        parameters[flag] = value;
    }

    slParameters.alpBRP = parameters["-E"];
    slParameters.alpDRP = parameters["-F"];
    slParameters.betBRP = parameters["-G"];
    slParameters.betDRP = parameters["-H"];
    slParameters.alpBSP = parameters["-I"];
    slParameters.alpDSP = parameters["-J"];
    slParameters.betBSP = parameters["-K"];
    slParameters.betDSP = parameters["-L"];
    slParameters.alpBPC = parameters["-M"];
    slParameters.alpDPC = parameters["-N"];
    slParameters.betBPC = parameters["-O"];
    slParameters.betDPC = parameters["-P"];
    slParameters.alpBSC = parameters["-Q"];
    slParameters.alpDSC = parameters["-R"];
    slParameters.betBSC = parameters["-S"];
    slParameters.betDSC = parameters["-T"];
    slParameters.alpBPSC = parameters["-U"];
    slParameters.alpDPSC = parameters["-V"];
    slParameters.betBPSC = parameters["-W"];
    slParameters.betDPSC = parameters["-X"];
    slParameters.alpBPHC = parameters["-Y"];
    slParameters.alpDPHC = parameters["-Z"];
    slParameters.betBPHC = parameters["-a"];
    slParameters.betDPHC = parameters["-b"];
    slParameters.alpBIC = parameters["-c"];
    slParameters.alpDIC = parameters["-d"];
    slParameters.betBIC = parameters["-e"];
    slParameters.betDIC = parameters["-f"];
    slParameters.alpBSA = parameters["-g"];
    slParameters.alpDSA = parameters["-h"];
    slParameters.betBSA = parameters["-i"];
    slParameters.betDSA = parameters["-j"];
    slParameters.maxPlausilbeRange = parameters["-k"];
    slParameters.maxPlausibleSpeed = parameters["-l"];
    slParameters.maxPlausibleAccel = parameters["-m"];
    slParameters.maxPlausibleDecel = parameters["-n"];
    slParameters.thrPSC = parameters["-o"];
    slParameters.maxHeadingChange = parameters["-p"];
    slParameters.thrIC = parameters["-q"];
    slParameters.maxSARange = parameters["-r"];
    slParameters.weightSLCheck[0] = parameters["-s"];
    slParameters.weightSLCheck[1] = parameters["-t"];
    slParameters.weightSLCheck[2] = parameters["-u"];
    slParameters.weightSLCheck[3] = parameters["-v"];
    slParameters.weightSLCheck[4] = parameters["-w"];
    slParameters.weightSLCheck[5] = parameters["-x"];
    slParameters.weightSLCheck[6] = parameters["-y"];
    slParameters.weightSLCheck[7] = parameters["-z"];
    slParameters.thrBelief = parameters["-aa"];
    slParameters.thrDisbelief = parameters["-ab"];
    slParameters.thrUncertainty = parameters["-ac"];
    slParameters.maxDist = parameters["-ad"];
    slParameters.alpBPP = parameters["-ae"];
    slParameters.betBPP = parameters["-af"];
    slParameters.alpDPP = parameters["-ag"];
    slParameters.betDPP = parameters["-ah"];
    slParameters.weightSLCheck[8] = parameters["-ai"];


    train = bool(parameters["-ba"]);
    int attackTypeIdx = int(parameters["-bb"]);

    slParameters.iteration = parameters["-ca"];

    if (attackTypeIdx == 1) {
        attackType = "const_pos";
    } else if (attackTypeIdx == 2) {
        attackType = "rand_speed";
    } else if (attackTypeIdx == 3) {
        attackType = "sudden_stop";
    } else if (attackTypeIdx == 4) {
        attackType = "mixed";
    }
}

string createJsonEntry(int architecture, string pattern, string attack, vector<double> returnValues,
                       LegacyParametersArc1 legacyParameters) {
    vector<double> statistics = calcStatistic(returnValues, true);
    TrainResults trainResults = TrainResults{
        legacyParameters.iteration, architecture, pattern, attack, returnValues[0], returnValues[1], returnValues[2],
        returnValues[3], statistics[0], statistics[1], statistics[2], statistics[3], legacyParameters
    };
    nlohmann::json jsonObject = trainResults;
    return jsonObject.dump();
}

string createJsonEntryCatch(int architecture, string pattern, string attack, vector<double> returnValues,
                       CatchParametersArc1 catchParameters) {
    vector<double> statistics = calcStatistic(returnValues, true);
    TrainResultsCatch trainResults = TrainResultsCatch{
        catchParameters.iteration, architecture, pattern, attack, returnValues[0], returnValues[1], returnValues[2],
        returnValues[3], statistics[0], statistics[1], statistics[2], statistics[3], catchParameters
    };
    nlohmann::json jsonObject = trainResults;
    return jsonObject.dump();
}

string createJsonEntrySL(int architecture, string pattern, string attack, vector<double> returnValues,
                       SLParametersArc1 slParameters) {
    vector<double> statistics = calcStatistic(returnValues, true);

    TrainResultsSL trainResults = TrainResultsSL{
        slParameters.iteration, architecture, pattern, attack, returnValues[0], returnValues[1], returnValues[2],
        returnValues[3], statistics[0], statistics[1], statistics[2], statistics[3], slParameters
    };
    nlohmann::json jsonObject = trainResults;
    return jsonObject.dump();
}


// binary MBD
void arc1_legacy_checks(int argc, char *argv[], bool optimizedValues, string path) {
    vector<double> returnValues;
    string pathAllData;
    string pathGroundTruth;
    LinkControl link = LinkControl();
    F2MDParameters params = F2MDParameters();

    LegacyChecks lc = LegacyChecks(&link, &params);
    Catch_checks cc = Catch_checks(&params);
    SLChecks sl = SLChecks(&params);
    LegacyParametersArc1 legacyParameters = LegacyParametersArc1(optimizedValues);
    CatchParametersArc1 catchParameters = CatchParametersArc1(optimizedValues);


    setLegacyParameters(argc, argv, legacyParameters, optimizedValues);

    double tp = 0.0;
    double tn = 0.0;
    double fp = 0.0;
    double fn = 0.0;

    if (attackType == "const_pos" or attackType == "mixed") {
        pathAllData = path + "/train/const_pos/const_pos.bin";
        pathGroundTruth = path + "/train/const_pos/const_pos_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/const_pos/const_pos.bin";
            pathGroundTruth = path + "/test/const_pos/const_pos_gt.bin";
        }

        returnValues = legacyPerformanceAnalysis(argc, argv, lc, cc, train, pathAllData, pathGroundTruth, true,
                                                 legacyParameters, "position", catchParameters);

        string const_pos = createJsonEntry(1, "binary", "const_pos", returnValues, legacyParameters);
        cout << const_pos << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }

    if (attackType == "rand_speed" or attackType == "mixed") {
        pathAllData = path + "/train/rand_speed/rand_speed.bin";
        pathGroundTruth = path + "/train/rand_speed/rand_speed_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/rand_speed/rand_speed.bin";
            pathGroundTruth = path + "/test/rand_speed/rand_speed_gt.bin";
        }

        returnValues = legacyPerformanceAnalysis(argc, argv, lc, cc, train, pathAllData, pathGroundTruth, true,
                                                 legacyParameters, "speed",catchParameters);

        string rand_speed = createJsonEntry(1, "binary", "rand_speed", returnValues, legacyParameters);
        cout << rand_speed << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }


    if (attackType == "sudden_stop" or attackType == "mixed") {
        pathAllData = path + "/train/sudden_stop/sudden_stop.bin";
        pathGroundTruth = path + "/train/sudden_stop/sudden_stop_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/sudden_stop/sudden_stop.bin";
            pathGroundTruth = path + "/test/sudden_stop/sudden_stop_gt.bin";
        }

        returnValues = legacyPerformanceAnalysis(argc, argv, lc, cc, train, pathAllData, pathGroundTruth, true,
                                                 legacyParameters, "position",catchParameters);

        string sudden_stop = createJsonEntry(1, "binary", "sudden_stop", returnValues, legacyParameters);
        cout << sudden_stop << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }

    if (attackType == "mixed") {
        string mixed = createJsonEntry(1, "binary", "mixed", {tp, tn, fp, fn}, legacyParameters);
        cout << mixed << endl;
    }

    if (train == true) {
        vector<double> returnValues = calcStatistic(vector<double>{tp, tn, fp, fn}, train);
        cout << returnValues[1] << endl;
        cout << returnValues[2] << endl;
    }
}


//FLoat MBD
void arc1_catch_checks(int argc, char *argv[], bool optimizedValues, string path) {
    vector<double> returnValues;
    string pathAllData;
    string pathGroundTruth;
    LinkControl link = LinkControl();
    F2MDParameters params = F2MDParameters();

    LegacyChecks lc = LegacyChecks(&link, &params);
    Catch_checks cc = Catch_checks(&params);
    SLChecks sl = SLChecks(&params);
    LegacyParametersArc1 legacyParameters = LegacyParametersArc1(optimizedValues);
    CatchParametersArc1 catchParameters = CatchParametersArc1(optimizedValues);


    setCatchParameters(argc, argv, catchParameters, optimizedValues);

    double tp = 0.0;
    double tn = 0.0;
    double fp = 0.0;
    double fn = 0.0;

    if (attackType == "const_pos" or attackType == "mixed") {
        pathAllData = path + "/train/const_pos/const_pos.bin";
        pathGroundTruth = path + "/train/const_pos/const_pos_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/const_pos/const_pos.bin";
            pathGroundTruth = path + "/test/const_pos/const_pos_gt.bin";
        }

        returnValues = catchPerformanceAnalysis(argc, argv, lc, cc, train, pathAllData, pathGroundTruth, true,
                                                 legacyParameters, "position",catchParameters);

        string const_pos = createJsonEntryCatch(1, "binary", "const_pos", returnValues, catchParameters);
        cout << const_pos << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }

    if (attackType == "rand_speed" or attackType == "mixed") {
        pathAllData = path + "/train/rand_speed/rand_speed.bin";
        pathGroundTruth = path + "/train/rand_speed/rand_speed_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/rand_speed/rand_speed.bin";
            pathGroundTruth = path + "/test/rand_speed/rand_speed_gt.bin";
        }

        returnValues = catchPerformanceAnalysis(argc, argv, lc, cc, train, pathAllData, pathGroundTruth, true,
                                                 legacyParameters, "speed", catchParameters);

        string rand_speed = createJsonEntryCatch(1, "binary", "rand_speed", returnValues, catchParameters);
        cout << rand_speed << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }


    if (attackType == "sudden_stop" or attackType == "mixed") {
        pathAllData = path + "/train/sudden_stop/sudden_stop.bin";
        pathGroundTruth = path + "/train/sudden_stop/sudden_stop_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/sudden_stop/sudden_stop.bin";
            pathGroundTruth = path + "/test/sudden_stop/sudden_stop_gt.bin";
        }

        returnValues = catchPerformanceAnalysis(argc, argv, lc, cc, train, pathAllData, pathGroundTruth, true,
                                                 legacyParameters, "position", catchParameters);

        string sudden_stop = createJsonEntryCatch(1, "binary", "sudden_stop", returnValues, catchParameters);
        cout << sudden_stop << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }

    if (attackType == "mixed") {
        string mixed = createJsonEntryCatch(1, "binary", "mixed", {tp, tn, fp, fn}, catchParameters);
        cout << mixed << endl;
    }

    if (train == true) {
        vector<double> returnValues = calcStatistic(vector<double>{tp, tn, fp, fn}, train);
        cout << returnValues[1] << endl;
        cout << returnValues[2] << endl;
    }
}


//SL MBD
void arc1_sl_checks(int argc, char *argv[], bool optimizedValues, string path) {
    vector<double> returnValues;
    string pathAllData;
    string pathGroundTruth;
    LinkControl link = LinkControl();
    F2MDParameters params = F2MDParameters();

    SLChecks sl = SLChecks(&params);
    SLParametersArc1 slParameters = SLParametersArc1();

    setSLParameters(argc, argv, slParameters);

    double tp = 0.0;
    double tn = 0.0;
    double fp = 0.0;
    double fn = 0.0;

    if (attackType == "const_pos" or attackType == "mixed") {
        pathAllData = path + "/train/const_pos/const_pos.bin";
        pathGroundTruth = path + "/train/const_pos/const_pos_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/const_pos/const_pos.bin";
            pathGroundTruth = path + "/test/const_pos/const_pos_gt.bin";
        }

        returnValues = slPerformanceAnalysis(sl,pathAllData, pathGroundTruth, slParameters, "position");

        string const_pos = createJsonEntrySL(1, "binary", "const_pos", returnValues, slParameters);
        cout << const_pos << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }

    if (attackType == "rand_speed" or attackType == "mixed") {
        pathAllData = path + "/train/rand_speed/rand_speed.bin";
        pathGroundTruth = path + "/train/rand_speed/rand_speed_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/rand_speed/rand_speed.bin";
            pathGroundTruth = path + "/test/rand_speed/rand_speed_gt.bin";
        }

        returnValues = slPerformanceAnalysis(sl,pathAllData, pathGroundTruth, slParameters, "speed");

        string rand_speed = createJsonEntrySL(1, "sl", "rand_speed", returnValues, slParameters);
        cout << rand_speed << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }


    if (attackType == "sudden_stop" or attackType == "mixed") {
        pathAllData = path + "/train/sudden_stop/sudden_stop.bin";
        pathGroundTruth = path + "/train/sudden_stop/sudden_stop_gt.bin";

        if (train == false) {
            //Todo: Set path to test files
            pathAllData = path + "/test/sudden_stop/sudden_stop.bin";
            pathGroundTruth = path + "/test/sudden_stop/sudden_stop_gt.bin";
        }

        returnValues = slPerformanceAnalysis(sl,pathAllData, pathGroundTruth, slParameters, "position");

        string sudden_stop = createJsonEntrySL(1, "sl", "sudden_stop", returnValues, slParameters);
        cout << sudden_stop << endl;

        tp = tp + returnValues[0];
        tn = tn + returnValues[1];
        fp = fp + returnValues[2];
        fn = fn + returnValues[3];
    }

    if (attackType == "mixed") {
        string mixed = createJsonEntrySL(1, "sl", "mixed", {tp, tn, fp, fn}, slParameters);
        cout << mixed << endl;
    }

    if (train == true) {
        vector<double> returnValues = calcStatistic(vector<double>{tp, tn, fp, fn}, train);
        cout << returnValues[1] << endl;
        cout << returnValues[2] << endl;
    }
}




int main(int argc, char *argv[]) {
    std::map<std::string, double> parameters;

    // Parse command-line arguments, expecting pairs of flags and values
    for (int i = 1; i < 6; i += 2) {
        std::string flag(argv[i]);
        double value = std::stod(argv[i + 1]);
        parameters[flag] = value;
    }

    int architecture = int(parameters["-A"]);
    int pattern = int(parameters["-B"]);
    bool optimizedValues = bool(parameters["-C"]);
    string path = argv[8];

    if (pattern == 1) {
        // legacy
        if (architecture == 1) {
            arc1_legacy_checks(argc, argv, optimizedValues, path);
        }
    } else if (pattern == 2) {
        if (architecture == 1) {
            arc1_catch_checks(argc, argv, optimizedValues, path);
        }
    } else if (pattern == 3) {
        if(architecture == 1) {
            arc1_sl_checks(argc, argv, optimizedValues, path);
        }
    }

    return 0;
}
