//
// Created by Artur Hermann on 06.06.24.
//

#ifndef CATCHCHECKS_H
#define CATCHCHECKS_H
#include "Coord.h"
#include "F2MDParameters.h"
//#include "MDMLib.h"
#include "MDMLib.h"

using namespace veins;


class Catch_checks {
private:
    MDMLib mdmLib;
    double MAX_PLAUSIBLE_SPEED1 = 20;
    double MAX_PLAUSIBLE_SPEED2 = 20;
    double MAX_PLAUSIBLE_ACCEL1 = 11.5;
    double MAX_PLAUSIBLE_ACCEL2 = 11.5;
    double MAX_PLAUSIBLE_DECEL1 = 30;
    double MAX_PLAUSIBLE_DECEL2 = 30;
    double MAX_PLAUSIBLE_DIST = 2;




public:
    Catch_checks(F2MDParameters * params);
    double RangePlausibilityCheck(veins::Coord* receiverPosition,veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,veins::Coord* senderPositionConfidence, double maxPlausibleRange);
    double SpeedPlausibilityCheck(double speed,double speedConfidence, double maxPlausibleSpeed);
    double PositionConsistancyCheck(veins::Coord* curPosition,veins::Coord* curPositionConfidence, veins::Coord* oldPosition,veins::Coord* oldPositionConfidence, double time, double maxPlausibleSpeed);
    double SpeedConsistancyCheck(double curSpeed,double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,double time, double maxPlausibleAccel, double maxPlausibleDecel);
    double PositionSpeedConsistancyCheck(veins::Coord* curPosition,veins::Coord* curPositionConfidence, veins::Coord* oldPosition,veins::Coord* oldPositionConfidence, double curSpeed,double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,double time,double maxPlausibleAccel, double maxPlausibleDecel, double maxPlausibleSpeed);
    double PositionHeadingConsistancyCheck(veins::Coord* curHeading,veins::Coord* curHeadingConfidence, veins::Coord* oldPosition,veins::Coord* oldPositionConfidence, veins::Coord* curPosition,veins::Coord* curPositionConfidence, double deltaTime, double curSpeed,double curSpeedConfidence, double maxHeadingChange);
    double IntersectionCheck(veins::Coord* nodePosition1,veins::Coord* nodePositionConfidence1, veins::Coord* nodePosition2,veins::Coord* nodePositionConfidence2, veins::Coord* nodeHeading1,veins::Coord* nodeHeading2, veins::Coord* nodeSize1, veins::Coord* nodeSize2,double deltaTime, double maxDeltaInter);
    double SuddenAppearenceCheck(veins::Coord* receiverPosition,
    veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double maxSARange);
    double PositionPlausibilityCheck(veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double senderSpeed,
    double senderSpeedConfidence, double dist, double max_dist);
    F2MDParameters * params;







};


#endif //CATCHCHECKS_H
