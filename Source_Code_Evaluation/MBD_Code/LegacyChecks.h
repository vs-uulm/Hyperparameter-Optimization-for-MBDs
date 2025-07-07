//
// Create by Hermann, based on the work form Kamel et al.
//

#ifndef LEGACYCHECKS_H
#define LEGACYCHECKS_H
#include <string>

//#include "Coord.h"
#include "Coord.h"
#include "F2MDParameters.h"
#include "InterTest.h"
#include "NodeTable.h"
#include "MDMLib.h"
//#include "BasicSafetyMessage.cpp"
#include "LinkControl.h"
#include "NodeKinematicData.h"
//using namespace veins;


class LegacyChecks {
private:
    MDMLib mdmLib;
    veins::Coord senderPosition;
    veins::Coord receiverPosition;


    LinkControl* LinkC;



    InterTest MultipleIntersectionCheck(NodeTable * detectedNodes,
        BasicSafetyMessage * bsm);


public:
    //Default-values
    // double MAX_PLAUSIBLE_SPEED = 20;
    // double MAX_PLAUSIBLE_ACCEL = 11.5;
    // double MAX_PLAUSIBLE_DECEL = 16;
    // double MAX_PLAUSIBLE_RANGE = 420;
    // double MAX_HEADING_CHANGE = 90;
    // double MAX_DELTA_INTER = 2.0;
    // double MAX_SA_RANGE = 420;
    // double threshold = 7.9;

    double MAX_PLAUSIBLE_RANGE = 319.77;
    double MAX_PLAUSIBLE_SPEED = 21.71;
    double MAX_PLAUSIBLE_ACCEL = 11.68;
    double MAX_PLAUSIBLE_DECEL = 16.64;
    double MAX_HEADING_CHANGE = 112.60;
    double MAX_SA_RANGE = 462.29;
    double MAX_DELTA_INTER = 2.0;
    double threshold = 5.99;
    double MAX_PLAUSIBLE_DIST = 2;

    LegacyChecks(LinkControl* LinkC, F2MDParameters* params);

    double RangePlausibilityCheck(veins::Coord* senderPosition,
        veins::Coord* receiverPosition, double);
    double PositionPlausibilityCheck(veins::Coord*, double, double, double);
    double SpeedPlausibilityCheck(double,double);
    double PositionConsistancyCheck(veins::Coord*, veins::Coord*, double, double);
    double SpeedConsistancyCheck(double, double, double, double, double);
    double PositionSpeedConsistancyCheck(veins::Coord*, veins::Coord *, double, double,
            double, double, double, double);
    double PositionHeadingConsistancyCheck(veins::Coord* curHeading,
        veins::Coord *curPosition, veins::Coord *oldPosition, double deltaTime,
        double curSpeed, double);
    double IntersectionCheck(veins::Coord nodePosition1, veins::Coord nodeSize1,
    veins::Coord head1, veins::Coord nodePosition2, veins::Coord nodeSize2, veins::Coord head2,
    double deltaTime, double);
    double SuddenAppearenceCheck(veins::Coord*, veins::Coord*, double);
    double MultipleIntersectionCheck2(NodeKinematicData historyData,veins::Coord nodePosition, veins::Coord nodeSize,
    veins::Coord head);
    F2MDParameters * params;




};

#endif //LEGACYCHECKS_H
