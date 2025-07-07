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

#ifndef __VEINS_NODETABLE_H_
#define __VEINS_NODETABLE_H_

#include "NodeHistory.h"
#include "NodeMDMHistory.h"
#include "GeneralLib.h"
#include "MDMLib.h"
#include "ProtocolEnforcer.h"
//#include "BasicSafetyMessage.h"


class NodeTable {

private:
    int nodesNum;
    NodeHistory nodeHistoryList[MAX_NODES_LENGTH];
    MDMHistory mdmHistoryList[MAX_NODES_LENGTH];
    unsigned long nodePseudos[MAX_NODES_LENGTH];

    NodeHistory nullNode = NodeHistory();
    MDMHistory nullMDMNode = MDMHistory();

    int getOldestNode(ProtocolEnforcer*, ProtocolEnforcer*);

public:
    NodeTable();
    int getNodesNum();
    NodeHistory* getNodeHistoryList();
    unsigned long getNodePseudo(int);
    void put(unsigned long, NodeHistory, MDMHistory, ProtocolEnforcer*, ProtocolEnforcer*);
    NodeHistory* getNodeHistoryAddr(unsigned long nodePseudonym);
    MDMHistory* getMDMHistoryAddr(unsigned long nodePseudonym);

    bool includes(unsigned long nodePseudonym);

    double getDeltaTime(unsigned long nodePseudo1, unsigned long nodePseudo2);

    BasicSafetyMessage* getRandomBSM();
    BasicSafetyMessage* getNextAttackedBsm( veins::Coord myPosition, unsigned long bsmNode, double bsmTime);
};

#endif
