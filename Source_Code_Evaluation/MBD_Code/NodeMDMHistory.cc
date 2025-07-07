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

#include "NodeMDMHistory.h"

#include "BsmCheck.h"

MDMHistory::MDMHistory() {
    nodePseudonym = 0;
    BSMNumV1 = 0;
    BSMNumV2 = 0;
}

int MDMHistory::getMDMNumV1() {
    return BSMNumV1;
}

int MDMHistory::getMDMNum(int version) {
    if (version == 1) {
        return BSMNumV1;
    } else {
        return BSMNumV2;
    }
}

int MDMHistory::getMDMNumV2() {
    return BSMNumV2;
}

MDMHistory::MDMHistory(unsigned long pseudo) {
    nodePseudonym = pseudo;
    BSMNumV1 = 0;
    BSMNumV2 = 0;
}



