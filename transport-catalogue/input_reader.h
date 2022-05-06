#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "stat_reader.h"
#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace parsing {
            namespace detail {
                class InputReader {
                public:
                    struct RouteData {
                        RouteType type;
                        std::string name;
                        std::vector<std::string> stops;
                    };
                private:
                    std::vector<std::vector<std::string>> stops_;
                    std::vector<RouteData> routes_;

                    void ParseStop(std::string_view line);
                    void ParseRoute(std::string_view line);

                public:
                    void AddStops(TransportCatalogue &catalogue);
                    void AddRoutes(TransportCatalogue &catalogue);
                    void AddDistances(TransportCatalogue &catalogue);
                    
                    void ParseLine(std::string_view line);
                };
                
                std::string ReadLine(std::istream &in);
                int ReadLineWithNumber(std::istream &in);
                
                std::string_view ParseKey(std::string_view &request, std::string_view & separator); // all spaces at the beginning and end of the key are ignored
                void RemoveSpaces(std::string_view &request);
                void ReadLineAndPrint(const TransportCatalogue &catalogue, std::istream &in, std::ostream &out);
            } 
        
        void ReadData(TransportCatalogue &catalogue, std::istream &in);
        void ReadAllRequestsAndPrint(const TransportCatalogue &catalogue, std::istream &in, std::ostream &out);
    } 
} 
