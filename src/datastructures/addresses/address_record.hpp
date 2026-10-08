
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <regex>

#include "../../utils/string_util.hpp"
#include "../../utils/constants.hpp"
#include "../../utils/response.hpp"

#pragma once

namespace address {

    struct Address {
        std::string street;
        std::string housenumber; 
        uint32_t postcode;
        std::string city;
        int64_t entryNode;

        Address() : street(""), housenumber(""), postcode(invalid_id), city(""), entryNode(invalid_id) {}

        bool empty() {
            return street == "" && housenumber == "" && postcode == invalid_id && city == "" && entryNode == invalid_id;
        }

        std::string to_string() {
            return street + " " + housenumber + " " + std::to_string(postcode) + " " + city;
        }
    };

    class AddressRecord {

    public:
        AddressRecord(const std::vector<std::string> &addressList) {
            std::vector<Address> deferedAddresses;
            std::unordered_set<uint32_t> foundPostcodes;

            for (const auto addressStr : addressList) {
                const auto a = buildAddressFromString(addressStr);

                if (a.postcode == invalid_id || a.city == "") {
                    deferedAddresses.push_back(a);
                    continue;
                }

                if (foundPostcodes.count(a.postcode)) {
                    streetToAddress[a.street].push_back(a);
                } else {
                    foundPostcodes.insert(a.postcode);
                    cityToPostcodes[a.city].push_back(a.postcode);
                    postcodeToCity[a.postcode] = a.city;
                    streetToAddress[a.street].push_back(a);
                }
            }

            for (auto a : deferedAddresses) {
                if (a.city == "" && a.postcode == invalid_id)
                    continue;
                
                if (a.city == "") {
                    if (postcodeToCity[a.postcode] == "")
                        continue;

                    a.city = postcodeToCity[a.postcode];
                    streetToAddress[a.street].push_back(a);
                    continue;
                }

                if (a.postcode == invalid_id) {
                    if (cityToPostcodes[a.city].size() == 0)
                        continue;

                    if (cityToPostcodes[a.city].size() == 1) {
                        a.postcode = cityToPostcodes[a.city][0];
                        streetToAddress[a.street].push_back(a);
                        continue;
                    }

                    streetToAddress[a.street].push_back(a);                    
                }
            }
        }

        Response query(std::vector<std::string> address) {
            lastAddress = {};

            if (address.size() < 1) {
                return {true, "An adress consits of <street> <houseno> <postcode> <city>, but at least <street>."};
            }

            // Permitted formats
            // <street> <houseno> <postcode> <city>
            // <street> <houseno> <postcode>
            // <street> <houseno> <city>
            // <street> <postcode>
            // <street> <city>
            // <street>

            // Am Störenäcker Bad Wildbad
            // 


            bool containsHouseno = false;
            bool containsPostcode = false;

            size_t housenoIdx = -1;
            size_t postcodeIdx = -1;


            for (size_t i = 0; i < address.size(); i++) {
                if (!containsHouseno && isHousenumber(address[i])) {
                    housenoIdx = i;
                    containsHouseno = true;
                }

                if (!containsPostcode && isPostcode(address[i])) {
                    postcodeIdx = i;
                    containsPostcode = true;
                }
            }

            if (containsHouseno && containsPostcode && housenoIdx == postcodeIdx) {
                containsHouseno = false;
            }

            std::string street = "";
            std::string houseno = "";
            std::string city = "";
            uint32_t postcode = invalid_id;
            
            if (containsHouseno) {
                houseno = address[housenoIdx];

                size_t i = 0;
                std::string sep = "";
                while (i < housenoIdx) {
                    street += sep;
                    street += address[i];
                    sep = " ";
                    ++i;
                }
            }

            if (!containsHouseno && containsPostcode) {
                size_t i = 0;
                std::string sep = "";
                while (i < postcodeIdx) {
                    street += sep;
                    street += address[i];
                    sep = " ";
                    ++i;
                }
            }

            if (containsPostcode)
                postcode = std::stoul(address[postcodeIdx]);

            if (containsPostcode && postcodeIdx != address.size() - 1) {
                // ... <postcode> <city>
                
                size_t i = postcodeIdx + 1;
                std::string sep = "";
                while (i < address.size()) {
                    street += sep;
                    street += address[i];
                    sep = " ";
                    ++i;
                }
            }

            if (containsHouseno && !containsPostcode) {
                size_t i = housenoIdx + 1;
                std::string sep = "";
                while (i < address.size()) {
                    city += sep;
                    city += address[i];
                    sep = " ";
                    ++i;
                }
            }

            // Last (hardest) Case: <street> <city>
            // We need to try all combinations of cities and streets
            // TODO: 
            std::vector<std::string> potentialStreets;
            std::vector<std::string> potentialCities;
            if (!containsHouseno && !containsPostcode) {
                return {true, "Unsupported format <street> <city>. Please specify a postcode."};
                
                // size_t i = 0;
                // std::string sep = "";
                // while (i < address.size()) {
                //     street += sep;
                //     street += address[i];
                //     sep = " ";
                //     ++i;

                //     if (streetToAddress.count(street))
                //         potentialStreets.push_back(street);
                // }
            }

            // Housenumber Postcode -> <street> <houseno> <postcode> <city> / <street> <houseno> <postcode>
            // No Housenumber Postcode -> <street> <postcode> / <street> <postcode> <city>
            // Housenumber No Postcode -> <street> <houseno> <city> / <street> <houseno>
            // No Housenumber No Postcode -> <street> / <street> <city> 

            if (street == "" && potentialStreets.size() == 0)
                return {true, "Please specify a street."};

            // Try to match all the potential addresses
            return match(street, potentialStreets, houseno, postcode, city, potentialCities);
        }

        Address getLastAddress() const {
            return lastAddress;
        }

    private:

        Response match(std::string street, std::vector<std::string> potentialStreets, std::string houseno, uint32_t postcode, std::string city, std::vector<std::string> potentialCities) {
            std::vector<Address> potentialAddresses;

            // std::cout << "Match this: " << street << " " << houseno << " " << std::to_string(postcode) << " " << city << "\n";
            
            if (street != "")
                potentialAddresses = streetToAddress[street];
            
            for (const auto street : potentialStreets) {
                potentialAddresses.insert(potentialAddresses.end(), streetToAddress[street].begin(), streetToAddress[street].end());
            }

            std::vector<Address> matches;
            for (const auto address : potentialAddresses) {
                if (houseno != "" && address.housenumber != houseno) {
                    // std::cout << address.housenumber << " " << houseno << "\n";
                    continue;
                }

                if (postcode != invalid_id && address.postcode != postcode) {
                    // std::cout << address.postcode << " " << postcode << "\n";
                    continue;
                }   

                if (city != "" && address.city != city) {
                    // std::cout << address.city << " " << city << "\n";
                    continue;
                }

                matches.push_back(address);
            }

            if (matches.size() == 1) {
                lastAddress = matches[0];
                return {false, "Match successful."};
            }

            if (matches.size() == 0) {
                return {true, "No matching address found."};
            }

            // if (houseno == "" && postcode != invalid_id) {
            //     // Find the average housenumber -> Housenumber is string...
            //     for (const auto match : matches) {
                    
            //     }
            // }

            // Disambiguate
            std::cout << "Multiple matches:\n";
            for (const auto match : matches) {
                std::cout << match.street << " " << match.housenumber << " " << match.postcode << " " << match.city << "\n";
                // std::cout << match.street << " " << match.housenumber << " " << match.postcode << " " << match.city << " -> " << match.entryNode << "\n";
            }

            return {true, "Multiple matches found. Please specify the address further."};
        }

        bool isPostcode(std::string s) {
            const std::regex postcode("[0-9]{5}");
            return std::regex_match(s, postcode);
        }

        bool isHousenumber(std::string s) {
            const std::regex housenumber("[1-9][0-9]*[a-z]*[-]*([1-9][0-9]*[a-z]*)*");
            return std::regex_match(s, housenumber);
        }

        Address buildAddressFromString(std::string addressStr) const {
            const auto parts = split_string(addressStr, ";-;");
            assert(parts.size() == 5);
            Address a;
            a.street = parts[0];
            a.housenumber = parts[1];
            a.city = parts[3];
            const auto postcodeStr = parts[2];
            const auto osmIdStr = parts[4];

            a.postcode = postcodeStr != "" ? static_cast<uint32_t>(std::stoul(postcodeStr)) : invalid_id;
            a.entryNode = static_cast<int64_t>(std::atoll(osmIdStr.c_str()));

            return a;
        }

        Address lastAddress;

        std::unordered_map<uint32_t, std::string> postcodeToCity;
        std::unordered_map<std::string, std::vector<uint32_t>> cityToPostcodes;

        std::unordered_map<std::string, std::vector<Address>> streetToAddress;
    };
}
