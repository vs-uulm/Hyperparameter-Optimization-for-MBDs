/*******************************************************************************
 * @author  Joseph Kamel
 * @email   josephekamel@gmail.com
 * @date    28/11/2018
 * @version 2.0
 *
 * SCA (Secure Cooperative Autonomous systems)
 * Copyright (c) 2013, 2018 Institut de Recherche Technologique SystemX
 * All rights reserved.
 *******************************************************************************/

#ifndef __VEINS_MDMHistory_H_
#define __VEINS_MDMHistory_H_

//#include <omnetpp.h>
#include "BsmCheck.h"
#include "F2MDParameters.h"

//#include "BasicSafetyMessage.cpp"


class BasicSafetyMessage;
//using namespace veins;

class MDMHistory {
private:
    unsigned long nodePseudonym;
    int BSMNumV1;
    int BSMNumV2;



public:
    MDMHistory();
    MDMHistory(unsigned long);
    int getMDMNumV1();
    int getMDMNum(int version);
    int getMDMNumV2();

    BsmCheck getBsmCheck(int index, int version);
    BsmCheck* getLatestBsmCheckAddr(int version);

    void addBsmCheck(BsmCheck bsmCheck, int version);

    void initKalman(BasicSafetyMessage * bsm, int version);

    void setBsmCheck(int index, BsmCheck bsmCheck, int version);


};

#endif
