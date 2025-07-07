//
// Created by Artur Hermann on 10.05.24.
//

#ifndef NODEKINEMATICHISTORYDATA_H
#define NODEKINEMATICHISTORYDATA_H

#include "NodeKinematicData.h"

class NodeKinematicHistoryData {
private:
public:
    NodeKinematicData dataHistory[2];   //dataHistory[0] is most up to data nodekinematicdata
    void addKinematicData(NodeKinematicData data);
};

#endif //NODEKINEMATICHISTORYDATA_H
