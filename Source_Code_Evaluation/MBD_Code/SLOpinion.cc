//
// Created by Artur Hermann on 16.06.24.
//

#include "SLOpinion.h"

SLOpinion::SLOpinion() {

}

SLOpinion::SLOpinion(double belief, double disbelief, double uncertainty) {
    this->belief = belief;
    this->disbelief = disbelief;
    this->uncertainty = uncertainty;
}

SLOpinion::SLOpinion(double belief, double disbelief, double uncertainty, double baserate) {
    this->belief = belief;
    this->disbelief = disbelief;
    this->uncertainty = uncertainty;
    this->baserate = baserate;
}

double SLOpinion::projectedProb() {
    return this->belief+this->baserate*this->uncertainty;
}



