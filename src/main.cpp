#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string_view>

#include "descriptions.h"
#include "json.h"
#include "profile.h"
#include "requests.h"
#include "test_runner.h"
#include "transport_manager.h"
#include "configuration.h"

#include "description_tests.h"
#include "requests_tests.h"

using namespace std;

auto PrepareJSONRequestKeys() {
  return configuration::json::RequestsKeys{
      .id = "id",
      .type = "type",
      .name = "name",
      .from = "from",
      .to = "to"
  };
}

auto PrepareJSONDescriptionKeys() {
  return configuration::json::DescriptionsKeys{
      .type = "type",
      .name = "name",
      .lat = "latitude",
      .lon = "longitude",
      .distances = "road_distances",
      .stop_list = "stops",
      .is_roundtrip = "is_roundtrip"
  };
}

auto PrepareJSONConfig() {
  return configuration::json::JSONConfig{}.
      SetRequestsKeys(PrepareJSONRequestKeys()).
      SetDescriptionsKeys(PrepareJSONDescriptionKeys());
}

void SimpleJSONTest() {
  ifstream input("input.json");

  configuration::GeneralConfig config;
  config.SetJSONConfig(PrepareJSONConfig());
  auto json_config = config.GetJSONConfig();

  ASSERT(input);

  auto document = json::Load(input);

  auto json = document.GetRoot().AsMap();
  auto desc = descriptions::ReadDescriptions(json["base_requests"].AsArray(), config.GetJSONConfig().GetDescriptionsKeys());
  const auto& req_keys = json_config.GetRequestsKeys();
  auto req = requests::ReadRequests(json["stat_requests"].AsArray(), req_keys);



//  DataType transport_manager({/* fill this! */},
//                                     json["routing_settings"].AsMap(), json["render_settings"].AsMap());
//
//  auto data =
//      requests::ProcessAll(json["stat_requests"].AsArray(), transport_manager);
//
//  cout << data << endl;
//
//  ofstream output("output.json");
//  output << data;
//  output.close();
}



int main() {
  TestRunner tr;


// отключены в файлах
//  descriptions::tests::RunTests();
//  requests::tests::RunTests();

  tr.RunTest(SimpleJSONTest, "SimpleJSON");
  cout.precision(25);

//  auto document = json::Load(std::cin);
//
//  auto json = document.GetRoot().AsMap();
//  DataType transport_manager(
//      descriptions::ReadDescriptions(json["base_requests"].AsArray()),
//      json["routing_settings"].AsMap(), json["render_settings"].AsMap());
//
//  auto data =
//      requests::ProcessAll(json["stat_requests"].AsArray(), transport_manager);

//  cout << data << endl;

  return 0;
}