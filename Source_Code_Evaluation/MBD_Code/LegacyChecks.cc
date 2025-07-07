//
// Create by Hermann, based on the work form Kamel et al.
//

#include "LegacyChecks.h"

LegacyChecks::LegacyChecks(LinkControl* LinkC, F2MDParameters* params)
{
    this->params = params;

    this->LinkC = LinkC;

    this->MAX_PLAUSIBLE_SPEED = 20;
    this->MAX_PLAUSIBLE_ACCEL = 13.99;
    this->MAX_PLAUSIBLE_DECEL = 23.36;//30;
}


double LegacyChecks::RangePlausibilityCheck(veins::Coord* senderPosition,
    veins::Coord* receiverPosition, double maxPlausibleRange)
{
    double distance = mdmLib.calculateDistancePtr(senderPosition,
        receiverPosition);

    if (distance < maxPlausibleRange) {
        return 1;
    }
    else {
        return 0; //distance
    }
}

double LegacyChecks::PositionPlausibilityCheck(veins::Coord* senderPosition,
    double senderSpeed, double distance, double max_plausible_dist)
{
    if (senderSpeed <= params->MAX_NON_ROUTE_SPEED) {
        return 1;
    }

    if (distance > max_plausible_dist) {
        return 0;
    }
    else {
        return 1;
    }
}

double LegacyChecks::SpeedPlausibilityCheck(double speed, double maxPlausibleSpeed)
{

    if (fabs(speed) < maxPlausibleSpeed) {
        return 1;
    }
    else {
        return 0; // fabs(speed)
    }
}

double LegacyChecks::PositionConsistancyCheck(veins::Coord* curPosition,
    veins::Coord* oldPosition, double time, double maxPlausibleSpeed)
{

    double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);

    if (distance < maxPlausibleSpeed * time) {
        return 1;
    }
    else {
        return 0; //distance
    }
}

double LegacyChecks::SpeedConsistancyCheck(double curSpeed, double oldspeed,
    double time, double maxPlausibleAccel, double maxPlausibleDecel)
{

    double speedDelta = curSpeed - oldspeed;

    //    double attFact = mdmLib.gaussianSum(1, 1 / 3);
    //    if (time >= 1) {
    //        attFact = time;
    //    }

    if (speedDelta > 0) {

        if (speedDelta < maxPlausibleAccel * time) {
            return 1;
        }
        else {
            return 0; //distance
        }
    }
    else {

        if (fabs(speedDelta) < maxPlausibleDecel * time) {
            return 1;
        }
        else {
            return 0; //distance
        }
    }
}

double LegacyChecks::PositionHeadingConsistancyCheck(veins::Coord* curHeading,
    veins::Coord* curPosition, veins::Coord* oldPosition, double deltaTime,
    double curSpeed, double maxHeadingChange)
{

    if (deltaTime < params->POS_HEADING_TIME) {
        double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
        if (distance < 1) {
            return 1;
        }

        if (curSpeed < 1) {
            return 1;
        }

        double curHeadingAngle = mdmLib.calculateHeadingAnglePtr(curHeading);
        veins::Coord relativePos = veins::Coord(curPosition->x - oldPosition->x,
            curPosition->y - oldPosition->y,
            curPosition->z - oldPosition->z);
        double positionAngle = mdmLib.calculateHeadingAngle(relativePos);
        double angleDelta = fabs(curHeadingAngle - positionAngle);
        if (angleDelta > 180) {
            angleDelta = 360 - angleDelta;
        }

        if (maxHeadingChange < angleDelta) {
            return 0; //  angleDelta - MAX_HEADING_CHANGE
        }
        else {
            return 1;
        }
    }
    else {
        return 1;
    }
}

// InterTest LegacyChecks::MultipleIntersectionCheck(NodeTable* detectedNodes,
//     BasicSafetyMessage* bsm)
// {
//
//     unsigned long senderPseudonym = bsm->getSenderPseudonym();
//
//     NodeHistory* varNode;
//
//     double INTScore = 0;
//     InterTest intertTest = InterTest();
//
//     INTScore = IntersectionCheck(myPosition, mySize, myHeading,
//         bsm->getSenderPos(),
//         veins::Coord(bsm->getSenderWidth(), bsm->getSenderLength()),
//         bsm->getSenderHeading(), 0.11,true);
//
//     if (INTScore < 1) {
//         intertTest.addInterValue(myPseudonym, INTScore);
//     }
//
//     for (int var = 0; var < detectedNodes->getNodesNum(); ++var) {
//         if (detectedNodes->getNodePseudo(var) != senderPseudonym) {
//             varNode = detectedNodes->getNodeHistoryAddr(
//                 detectedNodes->getNodePseudo(var));
//             double deltaTime = mdmLib.calculateDeltaTime(
//                 varNode->getLatestBSMAddr(), bsm);
//             if (deltaTime < params->MAX_DELTA_INTER1) {
//
//                 INTScore = IntersectionCheck(varNode->getSenderPos(0),
//                     varNode->getSenderSize(0), varNode->getSenderHeading(0),
//                     bsm->getSenderPos(),
//                     veins::Coord(bsm->getSenderWidth(), bsm->getSenderLength()),
//                     bsm->getSenderHeading(), deltaTime,true);
//
//                 if (INTScore < 1) {
//                     intertTest.addInterValue(myPseudonym, INTScore);
//                 }
//             }
//         }
//     }
//
//     return intertTest;
// }

double LegacyChecks::MultipleIntersectionCheck2(NodeKinematicData historyData,veins::Coord nodePosition, veins::Coord nodeSize,
    veins::Coord head)
{

    return 0.0;
}

double LegacyChecks::PositionSpeedConsistancyCheck(veins::Coord* curPosition,
    veins::Coord* oldPosition, double curSpeed, double oldspeed, double time, double maxPlausibleSpeed,
    double maxPlausibleAccel, double maxPlausibleDecel)
{
    if (time < params->MAX_TIME_DELTA) {
        double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
        double curminspeed = std::min(curSpeed, oldspeed);
        //double theoreticalSpeed = distance / time;
        double retDistance[2];
        mdmLib.calculateMaxMinDist(curSpeed, oldspeed, time,
            maxPlausibleAccel, maxPlausibleDecel, maxPlausibleSpeed,
            retDistance);

        double addon_mgt_range = params->MAX_MGT_RNG_DOWN + 0.3571 * curminspeed - 0.01694 * curminspeed * curminspeed;
        if (addon_mgt_range < 0) {
            addon_mgt_range = 0;
        }

        double deltaMin = distance - retDistance[0] + addon_mgt_range;
        double deltaMax = retDistance[1] - distance + params->MAX_MGT_RNG_UP;

        if (deltaMin < 0 || deltaMax < 0) {
            return 0;
        }
        else {
            return 1;
        }
    }
    else {
        return 1;
    }
}

double LegacyChecks::SuddenAppearenceCheck(veins::Coord* senderPosition,
    veins::Coord* receiverPosition, double maxSARange)
{

    double distance = mdmLib.calculateDistancePtr(senderPosition,
        receiverPosition);

    if (distance < maxSARange) {
        return 0; //distance
    }
    else {
        return 1;
    }
}

double LegacyChecks::IntersectionCheck(veins::Coord nodePosition1, veins::Coord nodeSize1,
    veins::Coord head1, veins::Coord nodePosition2, veins::Coord nodeSize2, veins::Coord head2,
    double deltaTime, double maxDeltaInter)
{


    double heading1 = mdmLib.calculateHeadingAnglePtr(&head1);
    double heading2 = mdmLib.calculateHeadingAnglePtr(&head2);

    //double distance = mdmLib.calculateDistancePtr(nodePosition1, nodePosition2);

    double inter = mdmLib.RectRectFactor(nodePosition1, nodePosition2, heading1,
        heading2, veins::Coord(nodeSize1.x, nodeSize1.y),
        veins::Coord(nodeSize2.x, nodeSize2.y));

    inter = inter * ((maxDeltaInter - deltaTime) / maxDeltaInter);

    if (inter > 0.5) {
        return 0;
    }
    else {
        return 1;
    }
}