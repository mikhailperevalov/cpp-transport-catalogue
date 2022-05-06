#include <iomanip>
#include "stat_reader.h"

using namespace std;

namespace transport_catalogue {
    namespace printer {
        void PrintRouteInfo(const TransportCatalogue &catalogue, const std::string &route_name, std::ostream &out) {
            out << "Bus "s << route_name << ": "s;
            try {
                auto route_info = catalogue.GetRouteInfo(route_name);
                out << setprecision(6) << route_info.num_of_stops << " stops on route, "s
                    <<  route_info.num_of_unique_stops << " unique stops, "s
                    << route_info.route_length << " route length, "s
                    << route_info.curvature << " curvature" << endl;
            }  catch (std::out_of_range&) {
                out << "not found"s << endl;
            }
        }
        void PrintStopInfo(const TransportCatalogue &catalogue, const std::string &stop_name, std::ostream &out) {
            out << "Stop "s << stop_name << ": "s;
            try {
                auto buses_on_stop = catalogue.GetBusesOnStop(stop_name);
                if (!buses_on_stop) {
                    out << "no buses"s << endl;
                } else {
                    out << "buses";
                    for (auto name : buses_on_stop.value().get ()) {
                        out << " "s << name;
                    }
                    out << endl;
                }
            }  catch (std::out_of_range&) {
                out << "not found"s << endl;
            }
        }
    } 
} 
