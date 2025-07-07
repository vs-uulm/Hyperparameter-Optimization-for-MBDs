//
// Created by Artur Hermann on 12.05.24.
//

#include "NodeKinematicHistoryData.h"

void NodeKinematicHistoryData::addKinematicData(NodeKinematicData data) {
    this->dataHistory[1] = this->dataHistory[0];
    this->dataHistory[0] = data;
}
