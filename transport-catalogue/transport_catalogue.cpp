#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <unordered_set>
#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {
    void TransportCatalogue::AddStop(detail::Stop stop) noexcept {
        stops_.push_back(move(stop));
        stops_by_names_.insert({stops_.back().name, &stops_.back()});
    }

    void TransportCatalogue::AddStop(const std::string &stop_name, geo::Coordinates coordinate) {
        detail::Stop stop;
        stop.name = stop_name;
        stop.coordinate = coordinate;
        AddStop(stop);
    }

    void TransportCatalogue::AddRoute(detail::Route route) noexcept {
        routes_.push_back(move(route)); // adding a route to the repository and to the index
        string_view route_name = routes_.back().name;
        routes_by_names_.insert({route_name, &routes_.back()});
        for (auto stop : routes_.back().stops) { // adding information about the bus to stops along the route
            buses_on_stops_[stop->name].insert(route_name);
        }
    }
    
    void TransportCatalogue::AddRoute(const string &route_name, RouteType route_type, const vector<string> &stops) {        
        if (route_type == RouteType::CIRCLE) { // checking the ring route the first and last stops are the same
            if (stops.front() != stops.back()) {
                throw std::invalid_argument("In circle route first and last stops must be equal"s);
            }
        }        
        detail::Route route; // forming a route with pointers to the corresponding stops from the catalog
        route.name = route_name;
        route.route_type = route_type;
        for (auto &stop_name : stops) {
            route.stops.push_back(FindStop(stop_name));
        }
        AddRoute(move(route));
    }

    void TransportCatalogue::SetDistance(const std::string &stop_from, const std::string &stop_to, int distance) {
        auto Stop_from = FindStop(stop_from);
        auto Stop_to = FindStop(stop_to);
        distances_[Stop_from->name][Stop_to->name] = distance;
    }

    const detail::Stop* TransportCatalogue::FindStop(const string &stop_name) const {
        if (stops_by_names_.count(stop_name) == 0) {
            throw std::out_of_range("Stop "s + stop_name + " does not exist in catalogue"s);
        }
        return stops_by_names_.at(stop_name);
    }

    const detail::Route* TransportCatalogue::FindRoute(const string &route_name) const {
        if (routes_by_names_.count(route_name) == 0) {
            throw std::out_of_range("Route "s + route_name + " does not exist in catalogue"s);
        }
        return routes_by_names_.at(route_name);
    }

    RouteInfo TransportCatalogue::GetRouteInfo(const string &route_name) const {
        RouteInfo result;
        auto route = FindRoute(route_name);
        result.name = route->name;
        result.route_type = route->route_type;
        result.num_of_stops = detail::CalculateStops(route);
        result.num_of_unique_stops = detail::CalculateUniqueStops(route);
        result.route_length = CalculateRealRouteLength(route);
        result.curvature = result.route_length / detail::CalculateRouteLength(route);
        return result;
    }

    std::optional<std::reference_wrapper<const std::set<std::string_view>>>
    TransportCatalogue::GetBusesOnStop(const std::string &stop_name) const {
        auto found = buses_on_stops_.find(FindStop(stop_name)->name);
        if (found == buses_on_stops_.end()) {
            return std::nullopt;
        } else {
            return std::cref(found->second);
        }
    }

    int TransportCatalogue::GetForwardDistance(const std::string &stop_from, const std::string &stop_to) const {
        if (distances_.count(stop_from) == 0 ||
            distances_.at(stop_from).count(stop_to) == 0) {
            throw std::out_of_range("No information about distance from "s + stop_from + " to "s + stop_to);
        }
        return distances_.at(stop_from).at(stop_to);
    }

    int TransportCatalogue::GetDistance(const std::string &stop_from, const std::string &stop_to) const {
        int result = 0;
        try {
            result = GetForwardDistance(stop_from, stop_to);
        } catch (std::out_of_range&) {
            try {
                result = GetForwardDistance(stop_to, stop_from);
            } catch (std::out_of_range&) {
                throw std::out_of_range("No information about distance between stops "s
                                                    + stop_from + " and "s + stop_to);
            }
        }
        return result;
    }
        
    int TransportCatalogue::CalculateRealRouteLength(const detail::Route *route) const {
        int result = 0;
            
        if (route != nullptr) {
            // по маршруту вперед
            for (auto iter1 = route->stops.begin(), iter2 = iter1+1;
                 iter2 < route->stops.end();
                 ++iter1, ++iter2) {
                result += GetDistance((*iter1)->name, (*iter2)->name);
            }
            // по маршруту назад
            if (route->route_type == RouteType::LINEAR) {
                for (auto iter1 = route->stops.rbegin(), iter2 = iter1+1;
                     iter2 < route->stops.rend();
                     ++iter1, ++iter2) {
                    result += GetDistance((*iter1)->name, (*iter2)->name);
                }
            }
        }
        return result;
    }
    
    namespace detail {
        int CalculateStops(const Route *route) noexcept {
            int result = 0;
            if (route != nullptr) {
                result = static_cast<int>(route->stops.size());
                if (route->route_type == RouteType::LINEAR) {
                    result = result  * 2 - 1;
                }
            }
            return result;
        }
        int CalculateUniqueStops(const Route *route) noexcept {
            int result = 0;
            if (route != nullptr) {
                unordered_set<string_view> uniques;
                for (auto stop : route->stops) {
                    uniques.insert(stop->name);
                }
                result = static_cast<int>(uniques.size());
            }
            return result;
        }
        double CalculateRouteLength(const Route *route) noexcept {
            double result = 0.0;
            if (route != nullptr) {
                for (auto iter1 = route->stops.begin(), iter2 = iter1+1;
                     iter2 < route->stops.end();
                     ++iter1, ++iter2) {
                    result += ComputeDistance((*iter1)->coordinate, (*iter2)->coordinate);
                }
                if (route->route_type == RouteType::LINEAR) {
                    result *= 2;
                }
            }
            return result;
        }
    } 
} 
