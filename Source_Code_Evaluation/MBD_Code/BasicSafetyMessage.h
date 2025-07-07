//
// Created by Artur Hermann on 06.05.24.
//

#ifndef BASICSAFETYMESSAGE_H
#define BASICSAFETYMESSAGE_H
#include "Coord.h"

using namespace veins;

class BasicSafetyMessage {
private:
    Coord senderPos;
    Coord senderPosConfidence;
    Coord senderSpeed;
    Coord senderSpeedConfidence;
    Coord senderHeading;
    Coord senderHeadingConfidence;
    Coord senderAccel;
    Coord senderAccelConfidence;
    double senderWidth;
    double senderLength;
    int senderMbType;
    int senderAttackType;
    Coord senderGpsCoordinates;
    double arrivalTime;
    double sendingTime;
    //LAddress::L2Type senderRealId;
    unsigned long senderPseudonym;
public:
    unsigned long getSenderPseudonym();
    Coord getSenderPos();
    Coord* getSenderPosPointer();
    double getSenderWidth();
    double getSenderLength();
    Coord getSenderHeading();
    double getArrivalTime();

    double getSendingTime();

    Coord getSenderSpeed();
};

#endif //BASICSAFETYMESSAGE_H
