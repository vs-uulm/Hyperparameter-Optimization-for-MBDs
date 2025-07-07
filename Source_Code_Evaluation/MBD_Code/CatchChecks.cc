//
// Created by Artur Hermann on 06.06.24.
//

#include "CatchChecks.h"
#include <cmath>

#include "SupportEllipse.h"

//#include "Coord.h"
using namespace veins;

Catch_checks::Catch_checks(F2MDParameters * params)
{
    this->params = params;
    this->MAX_PLAUSIBLE_SPEED1 = 29.62;
    this->MAX_PLAUSIBLE_ACCEL1 = 13.99;
    this->MAX_PLAUSIBLE_DECEL1 = 23.36;

}

double Catch_checks::RangePlausibilityCheck(veins::Coord* receiverPosition,
    veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double maxPlausibleRange)
{


    double distance = mdmLib.calculateDistancePtr(senderPosition,
        receiverPosition);

    double factor = mdmLib.CircleCircleFactor(distance,
        senderPositionConfidence->x, receiverPositionConfidence->x,
        maxPlausibleRange);

    return factor;
}

double berechneKreisabschnittsflaeche(double r, double d) {
    if (d >= r) {
        return 0.0;
    }

    double theta = acos(d / r);
    double sektorFlaeche = r * r * theta;
    double dreieckFlaeche = d * sqrt(r * r - d * d);

    return sektorFlaeche - dreieckFlaeche;
}

double Catch_checks::PositionPlausibilityCheck(veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double senderSpeed,
    double senderSpeedConfidence, double dist, double max_dist) {
    double speedd = senderSpeed - senderSpeedConfidence;
    if (speedd < 0) {
        speedd = 0;
    }

    if (speedd <= params->MAX_NON_ROUTE_SPEED) {
        return 1;
    }

    /*double noise = mdmLib.calculateSpeed(*senderPositionConfidence);
    double aCircle = pi*noise*noise;

    if (dist > 0) {
        double d = noise - dist;
        if(d< 0) {
            d=0;
        }
        double val = berechneKreisabschnittsflaeche(noise,d);
        val = val / aCircle;

        return 1 - val;
    } else {
        double d = noise - fabs(dist);
        if(d< 0) {
            d=0;
        }
        double val = berechneKreisabschnittsflaeche(noise,d);
        val = val / aCircle;

        return val;
    }*/

    double radius = mdmLib.calculateSpeed(*senderPositionConfidence);
    double aCircle = pi*radius*radius;
    double dist_total = radius + dist;

    if (dist_total < max_dist) {
        return 1;
    }
    else if (dist_total >=max_dist and (max_dist-dist)>=0 and (max_dist-dist)<= radius) {
        double d = max_dist-dist;
        double val = berechneKreisabschnittsflaeche(radius,d);
        val = val / aCircle;

        return 1-val;
    }
    else if (dist_total >=max_dist and (max_dist-dist)<0 and (max_dist-dist)>= -radius) {
        double d = dist - max_dist;
        double val = berechneKreisabschnittsflaeche(radius,d);
        val = val / aCircle;

        return val;
    }
    else if (dist_total >=max_dist and (max_dist-dist)<0 and (max_dist-dist)< -radius) {
        return 0;
    }

}

double Catch_checks::SpeedPlausibilityCheck(double speed,
    double speedConfidence, double maxPlausibleSpeed)
{


    if ((fabs(speed) + fabs(speedConfidence) / 2) < maxPlausibleSpeed) {
        return 1;
    }
    else if ((fabs(speed) - fabs(speedConfidence) / 2) > maxPlausibleSpeed) {
        return 0;
    }
    else {
        double factor = (fabs(speedConfidence) / 2 + (maxPlausibleSpeed - fabs(speed))) / fabs(speedConfidence);

        return factor;
    }
}

double Catch_checks::PositionConsistancyCheck(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double time, double maxPlausibleSpeed)
{

    double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
    double factor = mdmLib.CircleCircleFactor(distance,
        curPositionConfidence->x, oldPositionConfidence->x,
        maxPlausibleSpeed * time);

    return factor;
}

double Catch_checks::SpeedConsistancyCheck(double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time, double maxPlausibleAccel, double maxPlausibleDecel)
{


    double speedDelta = curSpeed - oldspeed;

    //    double attFact = mdmLib.gaussianSum(1, 1 / 3);
    //    if (time >= 1) {
    //        attFact = time;
    //    }

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

    return factor;
}

double Catch_checks::PositionSpeedConsistancyCheck(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time, double maxPlausibleAccel, double maxPlausibleDecel, double maxPlausibleSpeed)
{

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

        return std::min(factorMin, factorMax);
    }
    else {
        return 1;
    }
}

double Catch_checks::PositionHeadingConsistancyCheck(veins::Coord* curHeading,
    veins::Coord* curHeadingConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, double deltaTime, double curSpeed,
    double curSpeedConfidence, double maxHeadingChange)
{

    if (deltaTime < params->POS_HEADING_TIME) {
        double distance = mdmLib.calculateDistancePtr(curPosition, oldPosition);
        if (distance < 1) {
            return 1;
        }

        if (curSpeed - curSpeedConfidence < 1) {
            return 1;
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

        //    if(factor<=0.0){
        //
        //        std::cout<<"curPos: "<<curPosition<<'\n';
        //        std::cout<<"oldPos: "<<oldPosition<<'\n';
        //
        //        std::cout<<"relativePos: "<<relativePos<<'\n';
        //
        //        std::cout<<"curFactorLow: "<<curFactorLow<<'\n';
        //        std::cout<<"oldFactorLow: "<<oldFactorLow<<'\n';
        //        std::cout<<"curFactorHigh: "<<curFactorHigh<<'\n';
        //        std::cout<<"oldFactorHigh: "<<oldFactorHigh<<'\n';
        //        std::cout<<"positionAngle: "<<positionAngle<<'\n';
        //        std::cout<<"curHeadingAngle: "<<curHeadingAngle<<'\n';
        //        std::cout<<"angleDelta: "<<angleDelta<<'\n';
        //        std::cout<<"distance: "<<distance<<'\n';
        //        std::cout<<"distance: "<<distance<<'\n';
        //        std::cout<<"xLow: "<<xLow<<'\n';
        //        if(factor == 0){
        //            std::cout<<"ZERO: "<<factor<<'\n';
        //        }else{
        //            std::cout<<"NONZ: "<<factor<<'\n';
        //        }
        //
        //     //   exit(0);
        //    }

        //        factor = (factor - 0.5) * 2;
        //        factor = mdmLib.gaussianSum(factor, (1.0 / 4.5));
        //        if (factor > 0.75) {
        //            factor = 1;
        //        }
        //        if (factor <0.001) {
        //            factor = 0;
        //        }

        return factor;
    }
    else {
        return 1;
    }
}

double Catch_checks::IntersectionCheck(veins::Coord* nodePosition1,
    veins::Coord* nodePositionConfidence1, veins::Coord* nodePosition2,
    veins::Coord* nodePositionConfidence2, veins::Coord* nodeHeading1,
    veins::Coord* nodeHeading2, veins::Coord* nodeSize1, veins::Coord* nodeSize2,
    double deltaTime, double maxDeltaInter)
{

    //    double distance = mdmLib.calculateDistancePtr(nodePosition1, nodePosition2);
    //    double intFactor = mdmLib.CircleIntersectionFactor(
    //            nodePositionConfidence1->x, nodePositionConfidence2->x, distance,
    //            MIN_INT_DIST);
    //
    //    intFactor = intFactor *  ((MAX_DELTA_INTER - deltaTime) / MAX_DELTA_INTER);
    //
    //    intFactor = 1 - intFactor;
    //    return intFactor;


    double intFactor2 = mdmLib.EllipseEllipseIntersectionFactor(*nodePosition1,
        *nodePositionConfidence1, *nodePosition2, *nodePositionConfidence2,
        mdmLib.calculateHeadingAnglePtr(nodeHeading1),
        mdmLib.calculateHeadingAnglePtr(nodeHeading2), *nodeSize1,
        *nodeSize2);

    intFactor2 = 1.01 - intFactor2 * ((maxDeltaInter - deltaTime) / maxDeltaInter);

    if (intFactor2 > 1) {
        intFactor2 = 1;
    }

    if (intFactor2 < 0) {
        intFactor2 = 0;
    }

    return intFactor2;
}

double Catch_checks::SuddenAppearenceCheck(veins::Coord* receiverPosition,
    veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double maxSARange)
{


    double distance = mdmLib.calculateDistancePtr(senderPosition,
        receiverPosition);

    double r2 = maxSARange + receiverPositionConfidence->x;

    double factor = 0;
    if (senderPositionConfidence->x <= 0) {
        if (distance < r2) {
            factor = 0;
        }
        else {
            factor = 1;
        }
    }
    else {
        double area = mdmLib.calculateCircleCircleIntersection(
            senderPositionConfidence->x, r2, distance);

        factor = area / (M_PI * senderPositionConfidence->x * senderPositionConfidence->x);
        factor = 1 - factor;
    }

    return factor;
}