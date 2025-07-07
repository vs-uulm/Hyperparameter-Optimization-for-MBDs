//
// Created by Artur Hermann on 06.05.24.
//

#include "BasicSafetyMessage.h"

unsigned long BasicSafetyMessage::getSenderPseudonym() {
    return this->senderPseudonym;
}
Coord BasicSafetyMessage::getSenderPos() {
    return this->senderPos;
}
Coord* BasicSafetyMessage::getSenderPosPointer() {
    return &(this->senderPos);
}
double BasicSafetyMessage::getSenderWidth() {
    return this->senderWidth;
}
double BasicSafetyMessage::getSenderLength() {
    return this->senderLength;
}
Coord BasicSafetyMessage::getSenderHeading() {
    return this->senderHeading;
}
double BasicSafetyMessage::getArrivalTime() {
    return this->arrivalTime;
}

double BasicSafetyMessage::getSendingTime() {
    return this->sendingTime;
}

Coord BasicSafetyMessage::getSenderSpeed() {
    return this->senderSpeed;
}