#pragma once

#include "transport_catalogue.h"

namespace transport_catalogue {
    namespace printer {
        void PrintRouteInfo(const TransportCatalogue &catalogue, const std::string &route_name, std::ostream &out);
        void PrintStopInfo(const TransportCatalogue &catalogue, const std::string &stop_name, std::ostream &out);
    } 
} 
