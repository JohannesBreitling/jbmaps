
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "./datastructures/addresses/address_record.hpp"

#include "./utils/constants.hpp"
#include "./utils/response.hpp"
#include "./utils/timer.hpp"

#pragma once

template <typename SSSPQueryT, typename BackendT>
class RoutingEngine {

public:
    RoutingEngine(std::unordered_map<int64_t, uint32_t> &osmNodeIdToVertexIdx_, SSSPQueryT &ssspQuery_, BackendT &backend_, address::AddressRecord &record_) : osmNodeIdToVertexIdx(osmNodeIdToVertexIdx_), ssspQuery(ssspQuery_), backend(backend_), addressRecord(record_) {}

    void printUsage() {
        std::cout << "Check the readme.md for help.\n";
    }

    Response setSourceByAddress(std::vector<std::string> address) {
        const auto res = addressRecord.query(address);
        const auto lastAddress = addressRecord.getLastAddress();
        
        if (res.error)
            return res;

        source = lastAddress;

        std::string msg = "Source set successfully! " + source.street + " " + source.housenumber + " " + std::to_string(source.postcode) + " " + source.city;
        return {false, msg};
    }

    Response setTargetByAddress(std::vector<std::string> address) {
        const auto res = addressRecord.query(address);
        const auto lastAddress = addressRecord.getLastAddress();
        
        if (res.error)
            return res;

        target = lastAddress;

        std::string msg = "Target set successfully! " + target.street + " " + target.housenumber + " " + std::to_string(target.postcode) + " " + target.city;
        return {false, msg};
    }
    
    Response printAddresses() {
        std::cout << "Source: " << (source.empty() ? "No source specified. Specify with source-address." : source.to_string()) << "\n";
        std::cout << "Target: " << (target.empty() ?  "No target specified. Specify with target-address." : target.to_string()) << "\n";
        return {false, ""};
    }

    Response SSSP() {
        if (source.empty()) {
            return {false, "Specify a source first."};
        }

        if (target.empty()) {
            return {false, "Specify a target first."};
        }

        Timer timer;
        const auto s = osmNodeIdToVertexIdx[source.entryNode];
        const auto t = osmNodeIdToVertexIdx[target.entryNode];
        
        ssspQuery.run(s, t);
        const auto mics = timer.elapseMicros();
        const auto path = ssspQuery.getPath();
        const auto upSS = ssspQuery.getUpSearchSpace();
        const auto downSS = ssspQuery.getDownSearchSpace();

        backend.setConfig({true, true, true});

        backend.processSearchSpace(upSS, "up");
        backend.processSearchSpace(downSS, "down");
        backend.processPath(path);

        backend.visualize();
        
        return {false, "Route computed successfully (" + std::to_string(mics) + "mics)."};
    }

private:
    address::Address source = {};
    address::Address target = {};

    std::unordered_map<int64_t, uint32_t> &osmNodeIdToVertexIdx;
    SSSPQueryT &ssspQuery;
    BackendT &backend;

    address::AddressRecord addressRecord;

};