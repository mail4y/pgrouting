/*PGR-GNU*****************************************************************

FILE: solution.cpp

Copyright (c) 2015 pgRouting developers
Mail: project@pgrouting.org

------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

 ********************************************************************PGR-GNU*/



#include <string>
#include <vector>

#include "./solution.h"
#include "./pgr_pickDeliver.h"

namespace pgRouting {
namespace vrp {


void
Solution::get_postgres_result(
        std::vector< General_vehicle_orders_t > &result) const {
    /* postgres numbering starts with 1 */
    int i(1);
    for (const auto truck : fleet) {
        std::vector< General_vehicle_orders_t > data;
        truck.get_postgres_result(i, data);
        result.insert(result.end(), data.begin(), data.end());

        ++i;
    }
}



bool
Solution::is_feasable() const {
    for (const auto v : fleet) {
        if (v.is_feasable()) continue;
        return false;
    }
    return true;
}

double
Solution::duration() const {
    double total(0);
    for (const auto v : fleet) {
        total += v.duration();
    }
    return total;
}

int
Solution::twcTot() const {
    int total(0);
    for (const auto v : fleet) {
        total += v.twvTot();
    }
    return total;
}

double
Solution::wait_time() const {
    double total(0);
    for (const auto v : fleet) {
        total += v.total_wait_time();
    }
    return total;
}

int
Solution::cvTot() const {
    int total(0);
    for (const auto v : fleet) {
        total += v.cvTot();
    }
    return total;
}

Solution::Cost
Solution::cost() const {
    double total_duration(0);
    double total_wait_time(0);
    int total_twv(0);
    int total_cv(0);
    for (const auto v : fleet) {
        total_duration += v.duration();
        total_wait_time += v.total_wait_time();
        total_twv += v.twvTot();
        total_cv += v.cvTot();
    }
    return std::make_tuple(
            total_twv, total_cv, fleet.size(),
            total_wait_time, total_duration);
}



std::string
Solution::cost_str() const {
    Cost s_cost(cost());
    std::ostringstream log;

    log << "\n(twv, cv, fleet, wait, duration) = ("
        << std::get<0>(s_cost) << ", "
        << std::get<1>(s_cost) << ", "
        << std::get<2>(s_cost) << ", "
        << std::get<3>(s_cost) << ", "
        << std::get<4>(s_cost) << ")\n";

    return log.str();
}

std::string
Solution::tau(const std::string &title) const {
    Cost s_cost(cost());
    std::ostringstream log;

    log << "\n" << title << ": " << std::endl;
    for (const auto v : fleet) {
        log << "\n" << v.tau();
    }
    log << cost_str();
    return log.str();
}

std::ostream&
operator << (std::ostream &log, const Solution &solution) {
    for (const auto vehicle : solution.fleet) {
        log << vehicle;
    }

    log << "\n SOLUTION:\n\n "
        << solution.tau();

    return log;
}

bool
Solution::operator<(const Solution &s_rhs) const {
    Cost lhs(cost());
    Cost rhs(s_rhs.cost());

    /*
     * capacity violations
     */
    if (std::get<0>(lhs) < std::get<0>(rhs))
        return true;
    if (std::get<0>(lhs) > std::get<0>(rhs))
        return false;

    /*
     * time window violations
     */
    if (std::get<1>(lhs) < std::get<1>(rhs))
        return true;
    if (std::get<1>(lhs) > std::get<1>(rhs))
        return false;

    /*
     * fleet size
     */
    if (std::get<2>(lhs) < std::get<2>(rhs))
        return true;
    if (std::get<2>(lhs) > std::get<2>(rhs))
        return false;

    /*
     * waiting time
     */
    if (std::get<3>(lhs) < std::get<3>(rhs))
        return true;
    if (std::get<3>(lhs) > std::get<3>(rhs))
        return false;

    /*
     * duration
     */
    if (std::get<4>(lhs) < std::get<4>(rhs))
        return true;
    if (std::get<4>(lhs) > std::get<4>(rhs))
        return false;

    return false;
}

}  //  namespace vrp
}  //  namespace pgRouting
