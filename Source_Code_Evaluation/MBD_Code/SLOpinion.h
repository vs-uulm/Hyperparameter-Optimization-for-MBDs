//
// Created by Artur Hermann on 15.06.24.
//

#ifndef SLOPINION_H
#define SLOPINION_H

class SLOpinion {
private:


public:
    SLOpinion();
    SLOpinion(double belief, double disbelief, double uncertainty);
    SLOpinion(double belief, double disbelief, double uncertainty, double baserate);
    double projectedProb();
    double belief;
    double disbelief;
    double uncertainty;
    double baserate=0.5;

};

#endif //SLOPINION_H
