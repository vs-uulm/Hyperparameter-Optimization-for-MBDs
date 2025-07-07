#include <iostream>
#include <map>
#include <fstream>
#include <filesystem>
#include "NodeKinematicData.h"
#include </opt/homebrew/Cellar/rapidjson/1.1.0/include/rapidjson/document.h>


namespace fs = std::filesystem;

std::unordered_map<int, NodeKinematicData> messagesGroundTruth;



std::vector<std::vector<NodeKinematicData>> readAllData(const std::string& path, bool train) {
    std::vector<std::vector<NodeKinematicData>> allFileData; // To hold arrays for each file


    for (const auto& entry : std::filesystem::directory_iterator(path)) {


        if (entry.path().filename() == ".DS_Store" ||
            entry.path().filename().string().rfind("traceGroundTruthJSON", 0) == 0) {
            continue; // Skip macOS system files and ground truth files
        }

        if (entry.is_regular_file()) {
            std::ifstream inputFile(entry.path().string());
            if (!inputFile.is_open()) {
                std::cerr << "Failed to open file: " << entry.path().string() << std::endl;
                continue;
            }

            std::vector<NodeKinematicData> fileData; // Array for this file's data
            std::string line;
            while (std::getline(inputFile, line)) {

                // Parse JSON
                rapidjson::Document d;
                d.Parse(line.c_str());
                if (d.HasParseError()) {
                    std::cerr << "Error parsing JSON in file: " << entry.path().string() << std::endl;
                    continue;
                }

                NodeKinematicData data;
                data.type = d.HasMember("type") ? d["type"].GetInt() : -1;
                data.rcvTime = d.HasMember("rcvTime") ? d["rcvTime"].GetDouble() : 0.0;
                data.sendTime = d.HasMember("sendTime") ? d["sendTime"].GetDouble() : 0.0;
                data.sender = d.HasMember("sender") ? d["sender"].GetInt() : -1;
                data.senderPseudo = d.HasMember("senderPseudo") ? d["senderPseudo"].GetInt() : -1;
                data.messageID = d.HasMember("messageID") ? d["messageID"].GetInt() : -1;
                data.dist = d.HasMember("dist") ? d["dist"].GetDouble() : 0.0;

                if (d.HasMember("pos") && d["pos"].IsArray()) {
                    data.pos = {d["pos"][0].GetDouble(), d["pos"][1].GetDouble(), d["pos"][2].GetDouble()};
                }
                if (d.HasMember("pos_noise") && d["pos_noise"].IsArray()) {
                    data.pos_noise = {d["pos_noise"][0].GetDouble(), d["pos_noise"][1].GetDouble(), d["pos_noise"][2].GetDouble()};
                }
                if (d.HasMember("spd") && d["spd"].IsArray()) {
                    data.spd = {d["spd"][0].GetDouble(), d["spd"][1].GetDouble(), d["spd"][2].GetDouble()};
                }
                if (d.HasMember("spd_noise") && d["spd_noise"].IsArray()) {
                    data.spd_noise = {d["spd_noise"][0].GetDouble(), d["spd_noise"][1].GetDouble(), d["spd_noise"][2].GetDouble()};
                }
                if (d.HasMember("acl") && d["acl"].IsArray()) {
                    data.acl = {d["acl"][0].GetDouble(), d["acl"][1].GetDouble(), d["acl"][2].GetDouble()};
                }
                if (d.HasMember("acl_noise") && d["acl_noise"].IsArray()) {
                    data.acl_noise = {d["acl_noise"][0].GetDouble(), d["acl_noise"][1].GetDouble(), d["acl_noise"][2].GetDouble()};
                }
                if (d.HasMember("hed") && d["hed"].IsArray()) {
                    data.hed = {d["hed"][0].GetDouble(), d["hed"][1].GetDouble(), d["hed"][2].GetDouble()};
                }
                if (d.HasMember("hed_noise") && d["hed_noise"].IsArray()) {
                    data.hed_noise = {d["hed_noise"][0].GetDouble(), d["hed_noise"][1].GetDouble(), d["hed_noise"][2].GetDouble()};
                }

                // Add parsed data to this file's array
                fileData.push_back(data);
            }

            // Add this file's data to the overall array
            allFileData.push_back(fileData);
        }
    }

    return allFileData;
}

int readAndStoreGroundTruth(std::string inputFilePath) {
    messagesGroundTruth.clear();
    std::ifstream inputFile(inputFilePath);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    std::pmr::string line;
    while (getline(inputFile, line)) {
        rapidjson::Document d;
        d.Parse(line.c_str());

        if (d.HasParseError()) {
            std::cerr << "Error parsing JSON: " << d.GetParseError() << std::endl;
            continue;
        }

        NodeKinematicData m;
        m.type = d.HasMember("type") ? d["type"].GetInt() : -1;
        m.rcvTime = d.HasMember("rcvTime") ? d["rcvTime"].GetDouble() : 0.0;
        m.sendTime = d.HasMember("sendTime") ? d["sendTime"].GetDouble() : 0.0;
        m.sender = d.HasMember("sender") ? d["sender"].GetInt() : -1;
        m.senderPseudo = d.HasMember("senderPseudo") ? d["senderPseudo"].GetInt() : -1;
        m.messageID = d.HasMember("messageID") ? d["messageID"].GetInt() : -1;
        m.dist = d.HasMember("dist") ? d["dist"].GetDouble() : 0.0;

        m.pos = {d["pos"][0].GetDouble(), d["pos"][1].GetDouble(), d["pos"][2].GetDouble()};
        m.pos_noise = {d["pos_noise"][0].GetDouble(), d["pos_noise"][1].GetDouble(), d["pos_noise"][2].GetDouble()};
        m.spd = {d["spd"][0].GetDouble(), d["spd"][1].GetDouble(), d["spd"][2].GetDouble()};
        m.spd_noise = {d["spd_noise"][0].GetDouble(), d["spd_noise"][1].GetDouble(), d["spd_noise"][2].GetDouble()};
        m.acl = {d["acl"][0].GetDouble(), d["acl"][1].GetDouble(), d["acl"][2].GetDouble()};
        m.acl_noise = {d["acl_noise"][0].GetDouble(), d["acl_noise"][1].GetDouble(), d["acl_noise"][2].GetDouble()};
        m.hed = {d["hed"][0].GetDouble(), d["hed"][1].GetDouble(), d["hed"][2].GetDouble()};
        m.hed_noise = {d["hed_noise"][0].GetDouble(), d["hed_noise"][1].GetDouble(), d["hed_noise"][2].GetDouble()};

        // Store the message based on its sender ID
        messagesGroundTruth[m.messageID] = m;
    }

    inputFile.close();

    return 0;
}



void serializeAllFileData(const std::vector<std::vector<NodeKinematicData>>& allFileData, const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    // Write the outer vector size
    size_t outerSize = allFileData.size();
    ofs.write(reinterpret_cast<const char*>(&outerSize), sizeof(outerSize));

    // Write each inner vector
    for (const auto& innerVector : allFileData) {
        size_t innerSize = innerVector.size();
        ofs.write(reinterpret_cast<const char*>(&innerSize), sizeof(innerSize));

        for (const auto& data : innerVector) {
            data.serialize(ofs);
        }
    }
}

void serializeMessagesGroundTruth(
    const std::unordered_map<int, NodeKinematicData>& messagesGroundTruth,
    const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        throw std::runtime_error("Failed to open file for writing.");
    }

    // Write the size of the unordered_map
    size_t mapSize = messagesGroundTruth.size();
    ofs.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

    // Write each key-value pair
    for (const auto& [key, value] : messagesGroundTruth) {
        // Serialize the key
        ofs.write(reinterpret_cast<const char*>(&key), sizeof(key));

        // Serialize the value (NodeKinematicData)
        value.serialize(ofs);
    }
}

void createBinaryGroundTruth(std::string path, std::string filename) {
    readAndStoreGroundTruth(path);
    serializeMessagesGroundTruth(messagesGroundTruth,filename);
}

void createBinaryFile(std::string path, std::string filename) {
    std::vector<std::vector<NodeKinematicData>> alldata= readAllData(path,false);
    serializeAllFileData(alldata,path+"/"+filename);
}

void createBinaries(std::string path) {
    std::string attackPath = path+"/train/const_pos";
    fs::path file_path = fs::path(attackPath) / "const_pos.bin";

    createBinaryFile(attackPath, "const_pos.bin");

    file_path = fs::path(attackPath) / "const_pos_gt.bin";

    createBinaryGroundTruth(attackPath+"/traceGroundTruthJSON-7.json",attackPath+"/"+"const_pos_gt.bin");

    attackPath = path+"/test/const_pos";
    file_path = fs::path(attackPath) / "const_pos.bin";

    createBinaryFile(attackPath,"const_pos.bin");

    file_path = fs::path(attackPath) / "const_pos_gt.bin";

    createBinaryGroundTruth(attackPath+"/traceGroundTruthJSON-7.json",attackPath+"/"+"const_pos_gt.bin");

    attackPath = path+"/train/rand_speed";
    file_path = fs::path(attackPath) / "rand_speed.bin";

    createBinaryFile(attackPath,"rand_speed.bin");

    file_path = fs::path(attackPath) / "rand_speed_gt.bin";

    createBinaryGroundTruth(attackPath+"/traceGroundTruthJSON-7.json",attackPath+"/"+"rand_speed_gt.bin");

    attackPath = path+"/test/rand_speed";
    file_path = fs::path(attackPath) / "rand_speed.bin";

    createBinaryFile(attackPath,"rand_speed.bin");

    file_path = fs::path(attackPath) / "rand_speed_gt.bin";

    createBinaryGroundTruth(attackPath+"/traceGroundTruthJSON-7.json",attackPath+"/"+"rand_speed_gt.bin");

    attackPath = path+"/train/sudden_stop";
    file_path = fs::path(attackPath) / "sudden_stop.bin";

    createBinaryFile(attackPath,"sudden_stop.bin");

    file_path = fs::path(attackPath) / "sudden_stop_gt.bin";

    createBinaryGroundTruth(attackPath+"/traceGroundTruthJSON-7.json",attackPath+"/"+"sudden_stop_gt.bin");

    attackPath = path+"/test/sudden_stop";
    file_path = fs::path(attackPath) / "sudden_stop.bin";

    createBinaryFile(attackPath,"sudden_stop.bin");

    file_path = fs::path(attackPath) / "sudden_stop_gt.bin";

    createBinaryGroundTruth(attackPath+"/traceGroundTruthJSON-7.json",attackPath+"/"+"sudden_stop_gt.bin");
}

int main(int argc, char *argv[])
{
    std::map<std::string, double> parameters;

    std::string path = argv[2];

    createBinaries(path);

    return 0;
}
