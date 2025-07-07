//
// Created by Artur Hermann on 07.05.24.
//

#ifndef NODEKINEMATICDATA_H
#define NODEKINEMATICDATA_H
#include "Coord.h"

class NodeKinematicData {
private:
public:
    int type;
    double rcvTime;
    double sendTime;
    int sender;
    int senderPseudo;
    int messageID;
    double dist;
    veins::Coord pos;
    veins::Coord pos_noise;
    veins::Coord spd;
    veins::Coord spd_noise;
    veins::Coord acl;
    veins::Coord acl_noise;
    veins::Coord hed;
    veins::Coord hed_noise;

    void serialize(std::ostream& os) const {
        os.write(reinterpret_cast<const char*>(&type), sizeof(type));
        os.write(reinterpret_cast<const char*>(&rcvTime), sizeof(rcvTime));
        os.write(reinterpret_cast<const char*>(&sendTime), sizeof(sendTime));
        os.write(reinterpret_cast<const char*>(&sender), sizeof(sender));
        os.write(reinterpret_cast<const char*>(&senderPseudo), sizeof(senderPseudo));
        os.write(reinterpret_cast<const char*>(&messageID), sizeof(messageID));
        os.write(reinterpret_cast<const char*>(&dist), sizeof(dist));
        pos.serialize(os);
        pos_noise.serialize(os);
        spd.serialize(os);
        spd_noise.serialize(os);
        acl.serialize(os);
        acl_noise.serialize(os);
        hed.serialize(os);
        hed_noise.serialize(os);
    }

    void deserialize(std::istream& is) {
        is.read(reinterpret_cast<char*>(&type), sizeof(type));
        is.read(reinterpret_cast<char*>(&rcvTime), sizeof(rcvTime));
        is.read(reinterpret_cast<char*>(&sendTime), sizeof(sendTime));
        is.read(reinterpret_cast<char*>(&sender), sizeof(sender));
        is.read(reinterpret_cast<char*>(&senderPseudo), sizeof(senderPseudo));
        is.read(reinterpret_cast<char*>(&messageID), sizeof(messageID));
        is.read(reinterpret_cast<char*>(&dist), sizeof(dist));
        pos.deserialize(is);
        pos_noise.deserialize(is);
        spd.deserialize(is);
        spd_noise.deserialize(is);
        acl.deserialize(is);
        acl_noise.deserialize(is);
        hed.deserialize(is);
        hed_noise.deserialize(is);
    }

};

#endif //NODEKINEMATICDATA_H
