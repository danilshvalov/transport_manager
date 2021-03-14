#pragma once

#include "descriptions_types.h"

namespace descriptions::parsers {
using DistancesContainer = StopDescription::DistancesContainer;
using StopListContainer = BusDescription::StopListContainer;

sphere::Point ParsePosition(const json::Dict& dict, const JsonDescriptionsKeys& keys);
DistancesContainer ParseRoadDistances(const json::Dict& dict, const JsonDescriptionsKeys& keys);
StopListContainer ParseStopList(const json::Dict& dict, const JsonDescriptionsKeys& keys, bool is_roundtrip);
std::string ParseName(const json::Dict& dict, const JsonDescriptionsKeys& keys);
bool ParseIsRoundTrip(const json::Dict& dict, const JsonDescriptionsKeys& keys);
}