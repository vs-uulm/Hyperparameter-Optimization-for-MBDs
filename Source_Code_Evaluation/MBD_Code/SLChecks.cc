//
// Created by Artur Hermann on 15.06.24.
//

#include "SLChecks.h"

#include <map>

#include "NodeKinematicHistoryData.h"
#include "SLOpinion.h"

SLChecks::SLChecks(F2MDParameters * params)
{
    this->params = params;
    //this->MAX_PLAUSIBLE_SPEED = 25;//20;
    //this->MAX_PLAUSIBLE_ACCEL = 11.5;
    //this->MAX_PLAUSIBLE_DECEL = 30;
}

SLOpinion SLChecks::RangePlausibilityCheck(veins::Coord* receiverPosition,
    veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double maxPlausilbeRange, double alpB, double betB, double alpD, double betD) {


    double dist = mdmLib.calculateDistancePtr(receiverPosition,senderPosition);
    SLOpinion opinion = SLOpinion();

    if (dist <= maxPlausilbeRange) {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(dist - maxPlausilbeRange),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }else {
        opinion.belief=0;
        opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(dist - maxPlausilbeRange),2))));
        opinion.uncertainty=1-opinion.disbelief;
    }

    return opinion;
}

SLOpinion SLChecks::RangePlausibilityCheckCatch(veins::Coord* receiverPosition,
    veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double maxPlausibleRange, double alpB, double betB, double alpD, double betD)
{
    SLOpinion opinion = SLOpinion();

    double distance = mdmLib.calculateDistancePtr(senderPosition,
        receiverPosition);

    double factor = mdmLib.CircleCircleFactor(distance,
        senderPositionConfidence->x, receiverPositionConfidence->x,
        maxPlausibleRange);

    if (factor > 0.5) {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(factor - 0.5),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }else {
        opinion.belief=0;
        opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(factor - 0.5),2))));
        opinion.uncertainty=1-opinion.disbelief;
    }

    return opinion;
}

SLOpinion SLChecks::SpeedPlausibilityCheck(double speed,double speedConfidence, double maxPlausibleSpeed, double alpB, double betB, double alpD, double betD)
{
    SLOpinion opinion = SLOpinion();

    if (speed <= maxPlausibleSpeed) {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(speed - maxPlausibleSpeed),2))));
        opinion.disbelief = 0;
        opinion.uncertainty = 1-opinion.belief;
    }else {
        opinion.belief = 0;
        opinion.disbelief = alpD*(1 - exp(-betD*(pow(fabs(speed - maxPlausibleSpeed),2))));
        opinion.uncertainty = 1-opinion.disbelief;
    }

    return opinion;
}

SLOpinion SLChecks::SpeedPlausibilityCheckCatch(double speed,
    double speedConfidence, double maxPlausibleSpeed,double alpB, double betB, double alpD, double betD)
{
    SLOpinion opinion = SLOpinion();


    if ((fabs(speed) + fabs(speedConfidence) / 2) < maxPlausibleSpeed) {
        opinion.belief = alpB;
        opinion.disbelief = 0;
        opinion.uncertainty = 1 - opinion.belief;
        return opinion;
    }
    else if ((fabs(speed) - fabs(speedConfidence) / 2) > maxPlausibleSpeed) {
        opinion.belief = 0;
        opinion.disbelief = alpD;
        opinion.uncertainty = 1 - opinion.disbelief;
        return opinion;
    }
    else {
        double factor = (fabs(speedConfidence) / 2 + (maxPlausibleSpeed - fabs(speed))) / fabs(speedConfidence);

        if (factor > 0.5) {
            opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(factor - 0.5),2))));
            opinion.disbelief=0;
            opinion.uncertainty=1-opinion.belief;
        }else {
            opinion.belief=0;
            opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(factor - 0.5),2))));
            opinion.uncertainty=1-opinion.disbelief;
        }
        return opinion;
    }
}

SLOpinion SLChecks::PositionConsistancyCheck(veins::Coord* curPosition,veins::Coord* oldPosition, double time, double maxPlausibleSpeed, double alpB, double betB, double alpD, double betD)
{

    double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
    //double variance = this->varPositionConsistancy;
    double maxDistance = maxPlausibleSpeed * time;
    SLOpinion opinion = SLOpinion();


    if (distance <= maxDistance) {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(distance - maxDistance),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }
    else {
        opinion.belief = 0;
        opinion.disbelief = alpD*(1 - exp(-betD*(pow(fabs(distance - maxDistance),2))));
        opinion.uncertainty = 1-opinion.disbelief;
    }

    return opinion;
}

SLOpinion SLChecks::PositionConsistancyCheckCatch(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double time, double maxPlausibleSpeed,double alpB, double betB, double alpD, double betD)
{
    SLOpinion opinion = SLOpinion();

    double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
    double factor = mdmLib.CircleCircleFactor(distance,
        curPositionConfidence->x, oldPositionConfidence->x,
        maxPlausibleSpeed * time);

    if (factor > 0.5) {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(factor - 0.5),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }else {
        opinion.belief=0;
        opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(factor - 0.5),2))));
        opinion.uncertainty=1-opinion.disbelief;
    }

    return opinion;
}

SLOpinion SLChecks::SpeedConsistancyCheck(double curSpeed, double oldspeed,double time)
{
    double speedDelta = curSpeed - oldspeed;
    SLOpinion opinion = SLOpinion();
    opinion.uncertainty = 0.1;
    double OK = 0.0;
    double BAD = 10.0;
    if (speedDelta>BAD*0.9) {
        speedDelta = BAD*0.9;
    }

    opinion.belief = 1 - opinion.uncertainty - (speedDelta - OK) / (BAD - OK);
    opinion.disbelief = 1 - opinion.belief - opinion.uncertainty;

    return opinion;
}


SLOpinion SLChecks::SpeedConsistancyCheck2(double curSpeed, double oldspeed,double time, double maxPlausibleAccel, double maxPlausibleDecel, double alpB, double betB, double alpD, double betD) {
    SLOpinion opinion = SLOpinion();
    double speedDelta = curSpeed - oldspeed;
    double variance = this->varSpeedConsistancy;


    if (speedDelta > 0) {
        if (speedDelta < maxPlausibleAccel * time) {
            opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(speedDelta - maxPlausibleAccel * time),2))));
            opinion.disbelief=0;
            opinion.uncertainty=1-opinion.belief;
        }
        else {
            opinion.belief = 0;
            opinion.disbelief= alpD*(1 - exp(-betD*(pow(fabs(speedDelta - maxPlausibleAccel * time),2))));
            opinion.uncertainty=1-opinion.disbelief;
        }
    }
    else {

        if (fabs(speedDelta) < maxPlausibleDecel * time) {
            opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(speedDelta - maxPlausibleDecel * time),2))));
            opinion.disbelief=0;
            opinion.uncertainty=1-opinion.belief;
        }
        else {
            opinion.belief = 0;
            opinion.disbelief= alpD*(1 - exp(-betD*(pow(fabs(speedDelta - maxPlausibleDecel * time),2))));
            opinion.uncertainty=1-opinion.disbelief;
        }
    }

    return opinion;
}

SLOpinion SLChecks::SpeedConsistancyCheckCatch(double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time, double maxPlausibleAccel, double maxPlausibleDecel,double alpB, double betB, double alpD, double betD)
{

    SLOpinion opinion = SLOpinion();

    double speedDelta = curSpeed - oldspeed;


    double factor = 1;
    if (speedDelta > 0) {
        factor = mdmLib.SegmentSegmentFactor(speedDelta, curSpeedConfidence,
            oldSpeedConfidence, maxPlausibleAccel * time);
    }
    else {
        factor = mdmLib.SegmentSegmentFactor(fabs(speedDelta),
            curSpeedConfidence, oldSpeedConfidence,
            maxPlausibleDecel * time);
    }

    if (factor > 0.5) {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(factor - 0.5),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }else {
        opinion.belief=0;
        opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(factor - 0.5),2))));
        opinion.uncertainty=1-opinion.disbelief;
    }

    return opinion;
}


SLOpinion SLChecks::PositionHeadingConsistancyCheck(veins::Coord* curHeading, veins::Coord* curPosition, veins::Coord* oldPosition, double deltaTime,double curSpeed, double maxHeadingChange, double alpB, double betB, double alpD, double betD)
{


    SLOpinion opinion = SLOpinion();
    if (deltaTime < params->POS_HEADING_TIME) {
        double variance = this->varPositionHeadingConsistancy;
        double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);


        if (distance < 1) {
            opinion.belief=0;
            opinion.disbelief=0;
            opinion.uncertainty= 1.0;

            return opinion;
        }

        if (curSpeed < 1) {
            opinion.belief=0;
            opinion.disbelief=0;
            opinion.uncertainty= 1.0;

            return opinion;
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

        if (angleDelta > maxHeadingChange) {
            opinion.belief = 0;
            opinion.disbelief = alpD*(1 - exp(-betD*(pow(fabs(angleDelta - maxHeadingChange),2))));
            opinion.uncertainty = 1-opinion.disbelief;
        }
        else {
            opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(angleDelta - maxHeadingChange),2))));
            opinion.disbelief=0;
            opinion.uncertainty=1-opinion.belief;
        }
    }
    else {
        opinion.belief=0;
        opinion.disbelief=0;
        opinion.uncertainty= 1;
    }

    return opinion;
}

SLOpinion SLChecks::PositionHeadingConsistancyCheckCatch(veins::Coord* curHeading,
    veins::Coord* curHeadingConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, double deltaTime, double curSpeed,
    double curSpeedConfidence, double maxHeadingChange, double alpB, double betB, double alpD, double betD)
{
    SLOpinion opinion = SLOpinion();

    if (deltaTime < params->POS_HEADING_TIME) {
        double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
        if (distance < 1) {
            opinion.belief = 0;
            opinion.disbelief = 0;
            opinion.uncertainty = 1;
            return opinion;
        }

        if (curSpeed - curSpeedConfidence < 1) {
            opinion.belief = 0;
            opinion.disbelief = 0;
            opinion.uncertainty = 1.0;
            return opinion;
        }

        double curHeadingAngle = mdmLib.calculateHeadingAnglePtr(curHeading);

        veins::Coord relativePos = veins::Coord(curPosition->x - oldPosition->x,
            curPosition->y - oldPosition->y,
            curPosition->z - oldPosition->z);
        double positionAngle = mdmLib.calculateHeadingAnglePtr(&relativePos);
        double angleDelta = fabs(curHeadingAngle - positionAngle);
        if (angleDelta > 180) {
            angleDelta = 360 - angleDelta;
        }

        double angleLow = angleDelta - curHeadingConfidence->x;
        if (angleLow < 0) {
            angleLow = 0;
        }

        double angleHigh = angleDelta + curHeadingConfidence->x;
        if (angleHigh > 180) {
            angleHigh = 180;
        }

        double xLow = distance * cos(angleLow * M_PI / 180);

        double curFactorLow = 1;
        if (curPositionConfidence->x == 0) {
            if (angleLow <= maxHeadingChange) {
                curFactorLow = 1;
            }
            else {
                curFactorLow = 0;
            }
        }
        else {
            curFactorLow =
                mdmLib.calculateCircleSegment(curPositionConfidence->x,
                    curPositionConfidence->x + xLow) /
                (M_PI * curPositionConfidence->x * curPositionConfidence->x);
        }

        double oldFactorLow = 1;
        if (oldPositionConfidence->x == 0) {
            if (angleLow <= maxHeadingChange) {
                oldFactorLow = 1;
            }
            else {
                oldFactorLow = 0;
            }
        }
        else {
            oldFactorLow = 1 - mdmLib.calculateCircleSegment(oldPositionConfidence->x, oldPositionConfidence->x - xLow) / (M_PI * oldPositionConfidence->x * oldPositionConfidence->x);
        }

        double xHigh = distance * cos(angleHigh * M_PI / 180);
        double curFactorHigh = 1;
        if (curPositionConfidence->x == 0) {
            if (angleHigh <= maxHeadingChange) {
                curFactorHigh = 1;
            }
            else {
                curFactorHigh = 0;
            }
        }
        else {
            curFactorHigh =
                mdmLib.calculateCircleSegment(curPositionConfidence->x,
                    curPositionConfidence->x + xHigh) /
                (M_PI * curPositionConfidence->x * curPositionConfidence->x);
        }

        double oldFactorHigh = 1;
        if (oldPositionConfidence->x == 0) {
            if (angleHigh <= maxHeadingChange) {
                oldFactorHigh = 1;
            }
            else {
                oldFactorHigh = 0;
            }
        }
        else {
            oldFactorHigh = 1 - mdmLib.calculateCircleSegment(oldPositionConfidence->x, oldPositionConfidence->x - xHigh) / (M_PI * oldPositionConfidence->x * oldPositionConfidence->x);
        }

        double factor = (curFactorLow + oldFactorLow + curFactorHigh + oldFactorHigh) / 4;

        if (factor > 0.5) {
            opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(factor - 0.5),2))));
            opinion.disbelief=0;
            opinion.uncertainty=1-opinion.belief;
        }else {
            opinion.belief=0;
            opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(factor - 0.5),2))));
            opinion.uncertainty=1-opinion.disbelief;
        }
        return opinion;
    }
    else {
        opinion.belief = 0;
        opinion.disbelief = 0;
        opinion.uncertainty = 1.0;
        return opinion;
    }
}

SLOpinion SLChecks::IntersectionCheck(veins::Coord nodePosition1, veins::Coord nodeSize1,
    veins::Coord head1, veins::Coord nodePosition2, veins::Coord nodeSize2, veins::Coord head2,
    double deltaTime, double maxDeltaInter, double thrIC, double alpB, double betB, double alpD, double betD)
{


    double heading1 = mdmLib.calculateHeadingAnglePtr(&head1);
    double heading2 = mdmLib.calculateHeadingAnglePtr(&head2);
    double inter = mdmLib.RectRectFactor(nodePosition1, nodePosition2, heading1,
        heading2, veins::Coord(nodeSize1.x, nodeSize1.y),
        veins::Coord(nodeSize2.x, nodeSize2.y));

    inter = inter * ((maxDeltaInter - deltaTime) / maxDeltaInter);

    SLOpinion opinion = SLOpinion();
    double variance = this->varIntersectionCheck;

    if (inter > thrIC) {
        opinion.belief = 0;
        opinion.disbelief = alpD*(1 - exp(-betD*(pow(fabs(inter - thrIC),2))));
        opinion.uncertainty = 1-opinion.disbelief;
    }
    else {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(inter - thrIC),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }

    /*opinion.disbelief = inter;
    opinion.belief = 1 - opinion.disbelief;
    opinion.uncertainty = 0;*/

    return opinion;
}

SLOpinion SLChecks::IntersectionCheckCatch(veins::Coord* nodePosition1,
    veins::Coord* nodePositionConfidence1, veins::Coord* nodePosition2,
    veins::Coord* nodePositionConfidence2, veins::Coord* nodeHeading1,
    veins::Coord* nodeHeading2, veins::Coord* nodeSize1, veins::Coord* nodeSize2,
    double deltaTime, double maxDeltaInter,double alpB, double betB, double alpD, double betD)
{

    SLOpinion opinion = SLOpinion();


    double intFactor2 = mdmLib.EllipseEllipseIntersectionFactor(*nodePosition1,
        *nodePositionConfidence1, *nodePosition2, *nodePositionConfidence2,
        mdmLib.calculateHeadingAnglePtr(nodeHeading1),
        mdmLib.calculateHeadingAnglePtr(nodeHeading2), *nodeSize1,
        *nodeSize2);

    intFactor2 = 1.01 - intFactor2 * ((maxDeltaInter - deltaTime) / maxDeltaInter);

    if (intFactor2 > 1) {
        intFactor2 = 1;
        opinion.belief = alpB;
        opinion.disbelief = 0;
        opinion.uncertainty = 1 - opinion.belief;
    }

    if (intFactor2 < 0) {
        intFactor2 = 0;
        opinion.belief = 0;
        opinion.disbelief = alpD;
        opinion.uncertainty = 1 - opinion.disbelief;
    }

    if (intFactor2 > 0.5) {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(intFactor2 - 0.5),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }else {
        opinion.belief=0;
        opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(intFactor2 - 0.5),2))));
        opinion.uncertainty=1-opinion.disbelief;
    }
    return opinion;

}

SLOpinion SLChecks::SuddenAppearenceCheck(veins::Coord* senderPosition,
    veins::Coord* receiverPosition, double maxSARange, double alpB, double betB, double alpD, double betD)
{

    double distance = mdmLib.calculateDistancePtr(senderPosition,
        receiverPosition);

    SLOpinion opinion = SLOpinion();
    double variance = this->varSuddenAppearenceCheck;

    if (distance < maxSARange) {
        opinion.belief = 0;
        opinion.disbelief = alpD*(1 - exp(-betD*(pow(fabs(distance - maxSARange),2))));
        opinion.uncertainty = 1-opinion.disbelief;
    }
    else {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(distance - maxSARange),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }

    return opinion;
}

SLOpinion SLChecks::SuddenAppearenceCheckCatch(veins::Coord* receiverPosition,
    veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double maxSARange, double alpB, double betB, double alpD, double betD)
{

    SLOpinion opinion = SLOpinion();

    double distance = mdmLib.calculateDistancePtr(senderPosition,
        receiverPosition);

    double r2 = maxSARange + receiverPositionConfidence->x;

    double factor = 0;
    if (senderPositionConfidence->x <= 0) {
        if (distance < r2) {
            factor = 0;
            opinion.belief = 0;
            opinion.disbelief = alpD;
            opinion.uncertainty = 1 - opinion.disbelief;
        }
        else {
            factor = 1;
            opinion.belief = alpB;
            opinion.disbelief = 0;
            opinion.uncertainty = 1 - opinion.belief;
        }
    }
    else {
        double area = mdmLib.calculateCircleCircleIntersection(
            senderPositionConfidence->x, r2, distance);

        factor = area / (M_PI * senderPositionConfidence->x * senderPositionConfidence->x);
        factor = 1 - factor;
    }

    if (factor > 0.5) {
        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(factor - 0.5),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }else {
        opinion.belief=0;
        opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(factor - 0.5),2))));
        opinion.uncertainty=1-opinion.disbelief;
    }

    return opinion;
}

/*SLOpinion SLChecks::PositionSpeedConsistancyCheck(veins::Coord* curPosition,
    veins::Coord* oldPosition, double curSpeed, double oldspeed, double time)
{
    SLOpinion opinion = SLOpinion();

    if (time < params->MAX_TIME_DELTA) {
        double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
        double curminspeed = std::min(curSpeed, oldspeed);
        //double theoreticalSpeed = distance / time;
        double retDistance[2];
        mdmLib.calculateMaxMinDist(curSpeed, oldspeed, time,
            MAX_PLAUSIBLE_ACCEL, MAX_PLAUSIBLE_DECEL, MAX_PLAUSIBLE_SPEED,
            retDistance);

        double addon_mgt_range = params->MAX_MGT_RNG_DOWN + 0.3571 * curminspeed - 0.01694 * curminspeed * curminspeed;
        if (addon_mgt_range < 0) {
            addon_mgt_range = 0;
        }

        double deltaMin = distance - retDistance[0] + addon_mgt_range;
        double deltaMax = retDistance[1] - distance + params->MAX_MGT_RNG_UP;
        double deltaDist = retDistance[1] - retDistance[0];
        double variance = varPositionSpeedConsistency;

        if (deltaMin < 0 || deltaMax < 0) {
            opinion.belief=0;
            opinion.disbelief=1;
            opinion.uncertainty=0;
        }
        else if (distance < (retDistance[0] + positionSpeedLowThr * deltaDist)) {
            opinion.disbelief = exp(-(pow(abs(distance - retDistance[0]),2)/(2*pow(variance,2))));
            opinion.belief = 1-opinion.disbelief;
            opinion.uncertainty = 0;
        }else if (distance > (retDistance[0] + positionSpeedHighThr * deltaDist)){
            opinion.disbelief = exp(-(pow(abs(retDistance[1] - distance),2)/(2*pow(variance,2))));
            opinion.belief = 1-opinion.disbelief;
            opinion.uncertainty = 0;
        }
        else {
            opinion.belief=1;
            opinion.disbelief=0;
            opinion.uncertainty=0;

        }
        return opinion;
    }
    else {
        opinion.belief=0.9;
        opinion.disbelief=0;
        opinion.uncertainty=0.1;

        return opinion;
    }
}*/

// SLOpinion SLChecks::PositionSpeedConsistancyCheck2(veins::Coord* curPosition,
//     veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
//     veins::Coord* oldPositionConfidence, double curSpeed,
//     double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
//     double time)
// {
//
//     SLOpinion opinion = SLOpinion();
//
//     if (time < params->MAX_TIME_DELTA) {
//         double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
//
//         double curTest_1 = curSpeed + curSpeedConfidence;
//         double oldTest_1 = oldspeed - oldSpeedConfidence;
//
//         double curTest_2 = curSpeed - curSpeedConfidence;
//         double oldTest_2 = oldspeed + oldSpeedConfidence;
//
//         if (curTest_2 < oldTest_2) {
//             curTest_2 = (curSpeed + oldspeed) / 2;
//             oldTest_2 = (curSpeed + oldspeed) / 2;
//         }
//         double minSpeed = std::min(curSpeed, oldspeed);
//
//         double addon_mgt_range = params->MAX_MGT_RNG_DOWN + 0.3571 * minSpeed - 0.01694 * minSpeed * minSpeed;
//         if (addon_mgt_range < 0) {
//             addon_mgt_range = 0;
//         }
//
//         double retDistance_1[2];
//         mdmLib.calculateMaxMinDist(curTest_1, oldTest_1, time,
//             MAX_PLAUSIBLE_ACCEL, MAX_PLAUSIBLE_DECEL, MAX_PLAUSIBLE_SPEED,
//             retDistance_1);
//         double factorMin_1 = 1 - mdmLib.CircleCircleFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_1[0]);
//         double factorMax_1 = mdmLib.OneSidedCircleSegmentFactor(distance,
//             curPositionConfidence->x, oldPositionConfidence->x,
//             retDistance_1[1] + params->MAX_MGT_RNG_UP);
//
//         double retDistance_2[2];
//         mdmLib.calculateMaxMinDist(curTest_2, oldTest_2, time,
//             MAX_PLAUSIBLE_ACCEL, MAX_PLAUSIBLE_DECEL, MAX_PLAUSIBLE_SPEED,
//             retDistance_2);
//         double factorMin_2 = 1 - mdmLib.OneSidedCircleSegmentFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_2[0] - addon_mgt_range);
//         double factorMax_2 = mdmLib.OneSidedCircleSegmentFactor(distance,
//             curPositionConfidence->x, oldPositionConfidence->x,
//             retDistance_2[1] + params->MAX_MGT_RNG_UP);
//
//         double retDistance_0[2];
//         mdmLib.calculateMaxMinDist(curSpeed, oldspeed, time,
//             MAX_PLAUSIBLE_ACCEL, MAX_PLAUSIBLE_DECEL, MAX_PLAUSIBLE_SPEED,
//             retDistance_0);
//         double factorMin_0 = 1 - mdmLib.OneSidedCircleSegmentFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_0[0] - addon_mgt_range);
//         double factorMax_0 = mdmLib.OneSidedCircleSegmentFactor(distance,
//             curPositionConfidence->x, oldPositionConfidence->x,
//             retDistance_0[1] + params->MAX_MGT_RNG_UP);
//
//         //return std::min(factorMin_0, factorMax_0);
//
//         double factorMin = (factorMin_1 + factorMin_0 + factorMin_2) / 3.0;
//         double factorMax = (factorMax_1 + factorMax_0 + factorMax_2) / 3.0;
//
//         double minFactor= std::min(factorMin, factorMax);
//
//         double variance = varPositionSpeedConsistency;
//
//         //opinion.belief = 1 - exp(-(pow(abs(1-minFactor),2)/(2*pow(variance,2))));
//         //opinion.disbelief = 1 - opinion.belief;
//         //opinion.uncertainty = 0;
//
//         opinion.belief = minFactor;
//         opinion.disbelief = 1 - opinion.belief;
//         opinion.uncertainty = 0;
//
//         /*if (minFactor < 0.5) {
//             opinion.belief = 0;
//             opinion.disbelief = (minFactor-0.5) / 0.5;
//             opinion.uncertainty = (1.0-minFactor) / 0.5;
//         } else {
//             opinion.belief = (0.5-minFactor) / 0.5;
//             opinion.disbelief =0;
//             opinion.uncertainty=minFactor/0.5;
//         }*/
//
//     }
//     else {
//         opinion.belief=1;
//         opinion.disbelief=0;
//         opinion.uncertainty = 0;
//     }
//
//     return opinion;
// }



SLOpinion SLChecks::PositionSpeedConsistancyCheck3(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time, double maxPlausibleAccel, double maxPlausibleDecel, double maxPlausibleSpeed, double thrPSC, double alpB, double betB, double alpD, double betD)
{


    SLOpinion opinion = SLOpinion();

    if (time < params->MAX_TIME_DELTA) {
        double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);

        double curTest_1 = curSpeed + curSpeedConfidence;
        double oldTest_1 = oldspeed - oldSpeedConfidence;

        double curTest_2 = curSpeed - curSpeedConfidence;
        double oldTest_2 = oldspeed + oldSpeedConfidence;

        if (curTest_2 < oldTest_2) {
            curTest_2 = (curSpeed + oldspeed) / 2;
            oldTest_2 = (curSpeed + oldspeed) / 2;
        }
        double minSpeed = std::min(curSpeed, oldspeed);

        double addon_mgt_range = params->MAX_MGT_RNG_DOWN + 0.3571 * minSpeed - 0.01694 * minSpeed * minSpeed;
        if (addon_mgt_range < 0) {
            addon_mgt_range = 0;
        }

        double retDistance_1[2];
        mdmLib.calculateMaxMinDist(curTest_1, oldTest_1, time,
            maxPlausibleAccel, maxPlausibleDecel, maxPlausibleSpeed,
            retDistance_1);
        double factorMin_1 = 1 - mdmLib.CircleCircleFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_1[0]);
        double factorMax_1 = mdmLib.OneSidedCircleSegmentFactor(distance,
            curPositionConfidence->x, oldPositionConfidence->x,
            retDistance_1[1] + params->MAX_MGT_RNG_UP);

        double retDistance_2[2];
        mdmLib.calculateMaxMinDist(curTest_2, oldTest_2, time,
            maxPlausibleAccel, maxPlausibleDecel, maxPlausibleSpeed,
            retDistance_2);
        double factorMin_2 = 1 - mdmLib.OneSidedCircleSegmentFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_2[0] - addon_mgt_range);
        double factorMax_2 = mdmLib.OneSidedCircleSegmentFactor(distance,
            curPositionConfidence->x, oldPositionConfidence->x,
            retDistance_2[1] + params->MAX_MGT_RNG_UP);

        double retDistance_0[2];
        mdmLib.calculateMaxMinDist(curSpeed, oldspeed, time,
            maxPlausibleAccel, maxPlausibleDecel, maxPlausibleSpeed,
            retDistance_0);
        double factorMin_0 = 1 - mdmLib.OneSidedCircleSegmentFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_0[0] - addon_mgt_range);
        double factorMax_0 = mdmLib.OneSidedCircleSegmentFactor(distance,
            curPositionConfidence->x, oldPositionConfidence->x,
            retDistance_0[1] + params->MAX_MGT_RNG_UP);

        //return std::min(factorMin_0, factorMax_0);

        double factorMin = (factorMin_1 + factorMin_0 + factorMin_2) / 3.0;
        double factorMax = (factorMax_1 + factorMax_0 + factorMax_2) / 3.0;

        double minFactor= std::min(factorMin, factorMax);

        //double variance = varPositionSpeedConsistency;


        //opinion.belief = minFactor;
        //opinion.disbelief = 1 - opinion.belief;
        //opinion.uncertainty = 0;

        if (minFactor < thrPSC) {
            opinion.belief = 0;
            opinion.disbelief = alpB*(1 - exp(-betB*(pow(fabs(thrPSC-minFactor),2))));
            opinion.uncertainty = 1 - opinion.disbelief;
        } else {
            opinion.belief = alpD*(1 - exp(-betD*(pow(fabs(thrPSC-minFactor),2))));
            opinion.disbelief =0;
            opinion.uncertainty=1-opinion.belief;
        }
    }
    else {
        opinion.belief=alpB;
        opinion.disbelief=0;
        opinion.uncertainty = 1 - opinion.belief;
    }

    return opinion;
}


SLOpinion SLChecks::PositionSpeedConsistancyCheckCatch(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time, double maxPlausibleAccel, double maxPlausibleDecel, double maxPlausibleSpeed, double alpB, double betB, double alpD, double betD)
{

    SLOpinion opinion = SLOpinion();

    if (time < params->MAX_TIME_DELTA) {
        double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);

        double curTest_1 = curSpeed + curSpeedConfidence;
        double oldTest_1 = oldspeed - oldSpeedConfidence;

        double curTest_2 = curSpeed - curSpeedConfidence;
        double oldTest_2 = oldspeed + oldSpeedConfidence;

        if (curTest_2 < oldTest_2) {
            curTest_2 = (curSpeed + oldspeed) / 2;
            oldTest_2 = (curSpeed + oldspeed) / 2;
        }
        double minSpeed = std::min(curSpeed, oldspeed);

        double addon_mgt_range = params->MAX_MGT_RNG_DOWN + 0.3571 * minSpeed - 0.01694 * minSpeed * minSpeed;
        if (addon_mgt_range < 0) {
            addon_mgt_range = 0;
        }

        double retDistance_1[2];
        mdmLib.calculateMaxMinDist(curTest_1, oldTest_1, time,
            maxPlausibleAccel, maxPlausibleDecel, maxPlausibleSpeed,
            retDistance_1);
        double factorMin_1 = 1 - mdmLib.CircleCircleFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_1[0]);
        double factorMax_1 = mdmLib.OneSidedCircleSegmentFactor(distance,
            curPositionConfidence->x, oldPositionConfidence->x,
            retDistance_1[1] + params->MAX_MGT_RNG_UP);

        double retDistance_2[2];
        mdmLib.calculateMaxMinDist(curTest_2, oldTest_2, time,
            maxPlausibleAccel, maxPlausibleDecel, maxPlausibleSpeed,
            retDistance_2);
        double factorMin_2 = 1 - mdmLib.OneSidedCircleSegmentFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_2[0] - addon_mgt_range);
        double factorMax_2 = mdmLib.OneSidedCircleSegmentFactor(distance,
            curPositionConfidence->x, oldPositionConfidence->x,
            retDistance_2[1] + params->MAX_MGT_RNG_UP);

        double retDistance_0[2];
        mdmLib.calculateMaxMinDist(curSpeed, oldspeed, time,
            maxPlausibleAccel, maxPlausibleDecel, maxPlausibleSpeed,
            retDistance_0);
        double factorMin_0 = 1 - mdmLib.OneSidedCircleSegmentFactor(distance, curPositionConfidence->x, oldPositionConfidence->x, retDistance_0[0] - addon_mgt_range);
        double factorMax_0 = mdmLib.OneSidedCircleSegmentFactor(distance,
            curPositionConfidence->x, oldPositionConfidence->x,
            retDistance_0[1] + params->MAX_MGT_RNG_UP);

        //return std::min(factorMin_0, factorMax_0);

        double factorMin = (factorMin_1 + factorMin_0 + factorMin_2) / 3.0;
        double factorMax = (factorMax_1 + factorMax_0 + factorMax_2) / 3.0;

        double minimum =  std::min(factorMin, factorMax);

        if (minimum > 0.5) {
            opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(minimum - 0.5),2))));
            opinion.disbelief=0;
            opinion.uncertainty=1-opinion.belief;
        }else {
            opinion.belief=0;
            opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(minimum - 0.5),2))));
            opinion.uncertainty=1-opinion.disbelief;
        }
    }
    else {
        opinion.belief = 0;
        opinion.disbelief = 0;
        opinion.uncertainty = 1;
    }
    return opinion;
}

double berechneKreisabschnittsflaeche2(double r, double d) {
    if (d >= r) {
        return 0.0;
    }

    double theta = acos(d / r);
    double sektorFlaeche = r * r * theta;
    double dreieckFlaeche = d * sqrt(r * r - d * d);

    return sektorFlaeche - dreieckFlaeche;
}

SLOpinion SLChecks::PositionPlausibilityCheck(veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double senderSpeed,
    double senderSpeedConfidence, double dist, double max_dist, double alpB, double betB, double alpD, double betD) {
    double speedd = senderSpeed - senderSpeedConfidence;
    if (speedd < 0) {
        speedd = 0;
    }

    if (speedd <= params->MAX_NON_ROUTE_SPEED) {
        return SLOpinion(0,0.0,1);
    }

    double radius = mdmLib.calculateSpeed(*senderPositionConfidence);
    double aCircle = M_PI*radius*radius;
    double dist_total = radius + dist;

    SLOpinion opinion = SLOpinion();

    if (dist_total < max_dist) {
        opinion.belief = alpB;
        opinion.disbelief = 0.0;
        opinion.uncertainty = 1 - alpB;
    }
    else if (dist_total >=max_dist and (max_dist-dist)>=0 and (max_dist-dist)<= radius) {
        double d = max_dist-dist;
        double val = berechneKreisabschnittsflaeche2(radius,d);
        val = val / aCircle;

        val = 1 - val;

        opinion.belief = alpB*(1 - exp(-betB*(pow(fabs(val - 0.5),2))));
        opinion.disbelief=0;
        opinion.uncertainty=1-opinion.belief;
    }
    else if (dist_total >=max_dist and (max_dist-dist)<0 and (max_dist-dist)>= -radius) {
        double d = dist - max_dist;
        double val = berechneKreisabschnittsflaeche2(radius,d);
        val = val / aCircle;

        val = 1 - val;

        opinion.belief = 0;
        opinion.disbelief=alpD*(1 - exp(-betD*(pow(fabs(val - 0.5),2))));
        opinion.uncertainty=1-opinion.disbelief;
    }
    else if (dist_total >=max_dist and (max_dist-dist)<0 and (max_dist-dist)< -radius) {
        opinion.belief = 0.0;
        opinion.disbelief = alpD;
        opinion.uncertainty = 1 - alpD;
    }

    return opinion;

}
