//
// Created by Artur Hermann on 15.06.24.
//

#ifndef SLCHECKS_H
#define SLCHECKS_H

#include <map>

#include "Coord.h"
#include "F2MDParameters.h"
#include "MDMLib.h"
#include "NodeKinematicHistoryData.h"
#include "SLOpinion.h"

class SLChecks {
private:
    MDMLib mdmLib;

public:
    F2MDParameters * params;

    double weightRangePlausibility = 0.227;//0.17;
    double varSpeedPlausibility = 1.93;//0.02;
    double weightSpeedPlausibilty = 0.106;//0.22;
    double varPositionConsistancy = 5.44;//0.2;
    double weightPositionConsistancy = 0.11;//0.17;
    double varSpeedConsistancy = 3.18;//0.02;
    double weightSpeedConsistancy = 0.20;//0.06;
    double varPositionHeadingConsistancy = 10;
    double weightPositionHeadingConsistancy =  0.12;//0.02;
    double varIntersectionCheck = 0.12;//0.001;

    double weightIntersectionCheck = 0.10;//0.06;
    double varSuddenAppearenceCheck = 0.35;//5.0;
    double weightSuddenAppearenceCheck = 0.014;//0.28;
    double varPositionSpeedConsistency = 0.485;//4.89;

    double positionSpeedLowThr=0.01;
    double positionSpeedHighThr=0.8;
    double expoentialAverageAlpha=0.011;
    double varRTL = 0.8;

    double alp1RP =0.840;
    double alp2RP = 0.374;
    double bet1RP = 9160.335;
    double bet2RP = 1517.110;

    double alp1SP = 0.460;
    double alp2SP = 0.380;
    double bet1SP = 4249.854;
    double bet2SP = 12297.784;

    double alp1PC = 0.847;
    double alp2PC = 0.677;
    double bet1PC = 21670.111;
    double bet2PC = 36610.037;

    double alp1SC = 0.496;
    double alp2SC = 0.042;
    double bet1SC = 23044.12;
    double bet2SC = 30132.062;

    double alp1PS = 0.056;
    double alp2PS = 0.486;
    double bet1PS = 4126.848;
    double bet2PS = 34707.501;

    double alp1PH = 0.0442;
    double alp2PH = 0.583;
    double bet1PH = 107.838;
    double bet2PH = 18681.014;

    double alp1IC = 0.705;
    double alp2IC = 0.454;
    double bet1IC = 3587.097;
    double bet2IC = 45493.401;

    double alp1SA = 0.846;
    double alp2SA = 0.413;
    double bet1SA = 15568.880;
    double bet2SA = 1242.757;

    double MAX_PLAUSIBLE_RANGE = 401.416;
    double MAX_PLAUSIBLE_SPEED = 27.696;
    double MAX_PLAUSIBLE_ACCEL = 13.671;
    double MAX_PLAUSIBLE_DECEL = 28.925;
    double thrPositionSpeedConsistency = 0.5286;
    double MAX_HEADING_CHANGE = 98.117;
    double thrIntersectionCheck = 0.412;
    double MAX_SA_RANGE = 284.165;


    SLChecks(F2MDParameters * params);
    SLOpinion RangePlausibilityCheck(veins::Coord* receiverPosition,veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,veins::Coord* senderPositionConfidence,double maxPlausilbeRange, double alpB, double betB, double alpD, double betD);
    SLOpinion SpeedPlausibilityCheck(double speed,double speedConfidence,double maxPlausibleSpeed, double alpB, double betB, double alpD, double betD);
    SLOpinion PositionConsistancyCheck(veins::Coord* curPosition,veins::Coord* oldPosition, double time,double maxPlausibleSpeed, double alp1B, double bet1B, double alp2D, double bet2D);
    SLOpinion SpeedConsistancyCheck(double curSpeed, double oldspeed,double time);
    SLOpinion SpeedConsistancyCheck2(double curSpeed, double oldspeed,double time,double maxPlausibleAccel, double maxPlausibleDecel, double alpB, double betB, double alpD, double betD);
    SLOpinion PositionHeadingConsistancyCheck(veins::Coord* curHeading,veins::Coord* curPosition, veins::Coord* oldPosition, double deltaTime,double curSpeed,double maxHeadingChange, double alpB, double betB, double alpD, double betD);
    SLOpinion PositionSpeedConsistancyCheck(veins::Coord*, veins::Coord *, double, double,
        double);
    SLOpinion PositionSpeedConsistancyCheck2(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time);
    SLOpinion PositionSpeedConsistancyCheck3(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time,double maxPlausibleAccel, double maxPlausibleDecel, double maxPlausibleSpeed, double thrPSC, double alpB, double betB, double alpD, double betD);
    SLOpinion IntersectionCheck(veins::Coord nodePosition1, veins::Coord nodeSize1,veins::Coord head1, veins::Coord nodePosition2, veins::Coord nodeSize2, veins::Coord head2,double deltaTime,double maxDeltaInter, double thrIC, double alpB, double betB, double alpD, double betD);
    SLOpinion SuddenAppearenceCheck(veins::Coord* senderPosition, veins::Coord* receiverPosition, double maxSARange, double alpB, double betB, double alpD, double betD);
    SLOpinion RangePlausibilityCheckCatch(veins::Coord* receiverPosition,veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,veins::Coord* senderPositionConfidence, double maxPlausibleRange, double alpB, double betB, double alpD, double betD);
    SLOpinion SpeedPlausibilityCheckCatch(double speed,
    double speedConfidence, double maxPlausibleSpeed,double alpB, double betB, double alpD, double betD);
    SLOpinion PositionConsistancyCheckCatch(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double time, double maxPlausibleSpeed,double alpB, double betB, double alpD, double betD);
    SLOpinion SpeedConsistancyCheckCatch(double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time, double maxPlausibleAccel, double maxPlausibleDecel,double alpB, double betB, double alpD, double betD);
    SLOpinion PositionHeadingConsistancyCheckCatch(veins::Coord* curHeading,
    veins::Coord* curHeadingConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, double deltaTime, double curSpeed,
    double curSpeedConfidence, double maxHeadingChange, double alpB, double betB, double alpD, double betD);
    SLOpinion IntersectionCheckCatch(veins::Coord* nodePosition1,
        veins::Coord* nodePositionConfidence1, veins::Coord* nodePosition2,
        veins::Coord* nodePositionConfidence2, veins::Coord* nodeHeading1,
        veins::Coord* nodeHeading2, veins::Coord* nodeSize1, veins::Coord* nodeSize2,
        double deltaTime, double maxDeltaInter,double alpB, double betB, double alpD, double betD);
    SLOpinion SuddenAppearenceCheckCatch(veins::Coord* receiverPosition,
    veins::Coord* receiverPositionConfidence, veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double maxSARange, double alpB, double betB, double alpD, double betD);
    SLOpinion PositionSpeedConsistancyCheckCatch(veins::Coord* curPosition,
    veins::Coord* curPositionConfidence, veins::Coord* oldPosition,
    veins::Coord* oldPositionConfidence, double curSpeed,
    double curSpeedConfidence, double oldspeed, double oldSpeedConfidence,
    double time, double maxPlausibleAccel, double maxPlausibleDecel, double maxPlausibleSpeed, double alpB, double betB, double alpD, double betD);
    SLOpinion PositionPlausibilityCheck(veins::Coord* senderPosition,
    veins::Coord* senderPositionConfidence, double senderSpeed,
    double senderSpeedConfidence, double dist, double max_dist, double alpB, double betB, double alpD, double betD);
};

#endif //SLCHECKS_H
