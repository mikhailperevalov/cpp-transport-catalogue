#pragma once

#include <deque>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "geo.h"

namespace transport_catalogue {
    enum class RouteType {
        LINEAR,
        CIRCLE,
        UNKNOWN,
    };

    struct RouteInfo {
        std::string name;
        RouteType route_type;
        int num_of_stops = 0;
        int num_of_unique_stops = 0;
        int route_length = 0;
        double curvature = 0.0;
    };
    
    namespace detail {
        struct Stop {
            std::string name;
            geo::Coordinates coordinate;

        };

        struct Route {
            std::string name;
            RouteType route_type = RouteType::UNKNOWN;
            std::vector<const Stop*> stops; 
        };
    } 
    
    class TransportCatalogue {        
        std::deque<detail::Stop> stops_;
        std::unordered_map<std::string_view, const detail::Stop*> stops_by_names_;
        
        std::unordered_map<std::string_view, std::set<std::string_view>> buses_on_stops_;
        
        std::deque<detail::Route> routes_;
        std::unordered_map<std::string_view, const detail::Route*> routes_by_names_;
        
        std::unordered_map<std::string_view, std::unordered_map<std::string_view, int>> distances_;

    public:
        void AddStop(const std::string &stop_name, geo::Coordinates coordinate);
        void AddRoute(const std::string &route_name, RouteType route_type, const std::vector<std::string> &stops);        
        void SetDistance(const std::string &stop_from, const std::string &stop_to, int distance);
       
        RouteInfo GetRouteInfo(const std::string &route_name) const;
        std::optional<std::reference_wrapper<const std::set<std::string_view>>> GetBusesOnStop(const std::string &stop_name) const;
        int GetDistance(const std::string &stop_from, const std::string &stop_to) const;

    private:
        void AddStop(detail::Stop stop) noexcept;
        void AddRoute(detail::Route route) noexcept;

        const detail::Stop* FindStop(const std::string &stop_name) const;
        const detail::Route* FindRoute(const std::string &route_name) const;

        int GetForwardDistance(const std::string &stop_from, const std::string &stop_to) const;

        int CalculateRealRouteLength(const detail::Route* route) const;
    };
    namespace detail {        
        int CalculateStops(const detail::Route* route) noexcept;
        int CalculateUniqueStops(const detail::Route* route) noexcept;
        double CalculateRouteLength(const detail::Route* route) noexcept;
    } 
} 
