#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>

#include "description.h"
#include "json.h"
#include "profile.h"
#include "requests.h"
#include "test_runner.h"
#include "transport_manager.h"

#include "description_tests.h"

using namespace std;

void SimpleJSONTest() {
  ifstream input("input.json");

  ASSERT(input);

  auto document = Json::Load(input);

  auto json = document.GetRoot().AsMap();
  TransportManager transport_manager(
      Descriptions::ReadDescriptions(json["base_requests"].AsArray()),
      json["routing_settings"].AsMap(), json["render_settings"].AsMap());

  auto data =
      Requests::ProcessAll(json["stat_requests"].AsArray(), transport_manager);

  cout << data << endl;

  ofstream output("output.json");
  output << data;
  output.close();
}

int main() {
  TestRunner tr;

  Descriptions::Tests::RunTests();

  tr.RunTest(SimpleJSONTest, "SimpleJSON");
  cout.precision(25);

  auto document = Json::Load(std::cin);

  auto json = document.GetRoot().AsMap();
  TransportManager transport_manager(
      Descriptions::ReadDescriptions(json["base_requests"].AsArray()),
      json["routing_settings"].AsMap(), json["render_settings"].AsMap());

  auto data =
      Requests::ProcessAll(json["stat_requests"].AsArray(), transport_manager);

  cout << data << endl;
}