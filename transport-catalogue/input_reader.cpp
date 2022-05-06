#include <algorithm>
#include <sstream>

#include "input_reader.h"

using namespace std;

namespace transport_catalogue {
    namespace parsing {
        namespace detail {
            void InputReader::ParseStop(std::string_view line) {
                vector<string> stop_request;
                
                string_view separator = ":"sv;
                string name = string(ParseKey(line, separator));
                
                separator = ","sv;
                string lat = string(ParseKey(line, separator));
                
                separator = ","sv;
                string lng = string(ParseKey(line, separator));
                
                stop_request.push_back(move(name));
                stop_request.push_back(move(lat));
                stop_request.push_back(move(lng));
               
                while (!line.empty()) {
                    separator = "m"sv;
                    string distance = string(ParseKey(line, separator));
                    stop_request.push_back(move(distance));
                    separator = "o"sv;
                    ParseKey(line, separator);
                    separator = ","sv;
                    string stop_name = string(ParseKey(line, separator));
                    stop_request.push_back(move(stop_name));
                }
                stops_.push_back(move(stop_request));
            }
            
            void InputReader::ParseRoute(std::string_view line) {
                RouteData route_request;
                string_view separator = ":"sv;
                string name = string(ParseKey(line, separator));
                route_request.name = move(name);
                separator = ">-"sv;
                string stop = string(ParseKey(line, separator));
                route_request.stops.push_back(move(stop));
                
                if (separator == ">"sv) {
                    route_request.type = RouteType::CIRCLE;
                } else if (separator == "-"sv) {
                    route_request.type = RouteType::LINEAR;
                }
                
                while (!line.empty()) {
                    stop = string(ParseKey(line, separator));
                    route_request.stops.push_back(move(stop));
                }
                routes_.push_back(route_request);
            }
            
            string_view ParseKey(std::string_view &request, std::string_view & separator) {
                auto end_pos = request.find_first_of(separator);
                string_view result = request.substr(0, end_pos);
                RemoveSpaces(result);
                request.remove_prefix(std::min(request.size (), end_pos));
                
                if (!request.empty()) {
                    separator = request.substr(0, 1);
                    request.remove_prefix(1);
                }
                return result;
            }
            
            void InputReader::ParseLine(std::string_view line) {
                string_view separator = " "sv;
                auto command = ParseKey(line, separator);
                
                if (command == "Stop"sv) {
                    ParseStop(line);
                } else if (command == "Bus"sv) {
                    ParseRoute(line);
                }
            }
            
            void InputReader::AddStops(TransportCatalogue &catalogue) {
                for(auto &request : stops_) {
                    string name = request.at(0);
                    geo::Coordinates coordinate;
                    coordinate.lat = stod(request.at(1));
                    coordinate.lng = stod(request.at(2));
                    catalogue.AddStop(name, coordinate);
                }
            }

            void InputReader::AddRoutes(TransportCatalogue &catalogue) {
                for(auto &request : routes_) {
                    catalogue.AddRoute(request.name, request.type, request.stops);
                }
            }
            
            void InputReader::AddDistances(TransportCatalogue &catalogue) {
                for (auto &request : stops_) {
                    // если информации о расстоянии в запросе нет - пропускаем
                    if (request.size() <= 3) {
                        continue;
                    }
                    string stop1_name = request.at(0);
                    for (auto iter = request.begin() + 3; iter < request.end(); iter+=2) {
                        int distance = stoi((*iter));
                        string stop2_name = *(iter+1);
                        catalogue.SetDistance(stop1_name, stop2_name, distance);
                    }
                }
            }
            
            std::string ReadLine(std::istream &in) {
                string s;
                getline(in, s);
                return s;
            }
            
            int ReadLineWithNumber(std::istream &in) {
                int result;
                in >> result;
                ReadLine(in);
                return result;
            }
            
            void RemoveSpaces(std::string_view &request) {
                auto first_no_space = request.find_first_not_of(' ');
                auto last_no_space = request.find_last_not_of(' ');
                auto rcount = last_no_space - first_no_space + 1;
                request = request.substr(first_no_space, rcount);
            }
            
            void ReadLineAndPrint(const TransportCatalogue &catalogue, std::istream &in, std::ostream &out) {
                string request = ReadLine(in);
                string_view req = request;
                string_view separator = " "sv;
                auto command = ParseKey(req, separator);
                RemoveSpaces(req);
                string name = string(req);
                if (command == "Bus"sv) {
                    transport_catalogue::printer::PrintRouteInfo(catalogue, name, out);
                } else if (command == "Stop"sv) {
                    transport_catalogue::printer::PrintStopInfo(catalogue, name, out);
                }
            }
        } 
        
        void ReadData(TransportCatalogue &catalogue, std::istream &in) {
            detail::InputReader reader;
            int count = detail::ReadLineWithNumber(in);
            for (int i = 0; i < count; ++i) {
                string line = detail::ReadLine(in);
                reader.ParseLine(line);
            }
            reader.AddStops(catalogue);
            reader.AddRoutes(catalogue);
            reader.AddDistances(catalogue);
        }
        
        void ReadAllRequestsAndPrint(const TransportCatalogue &catalogue, std::istream &in, std::ostream &out) {
            int count = detail::ReadLineWithNumber(in);
            for (int i = 0; i < count; ++i) {
                detail::ReadLineAndPrint(catalogue, in, out);
            }
        }
    } 
} 
