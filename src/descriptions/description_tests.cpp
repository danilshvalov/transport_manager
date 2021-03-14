#include "description_tests.h"

using namespace std;
using namespace json;

namespace descriptions::tests {

void JSONStopDescriptionParsing() {
//  StopDescription
//      expected = {
//      .name = "Some stop",
//      .position ={
//          .lat = 43.59,
//          .lon = 39.74,
//      },
//      .distances = {
//          {"First stop", 4300},
//          {"Second stop", 2000}
//      }
//  };
//  StopDescription testing;
//  stringstream input(R"(
//    {
//        "type": "Stop",
//        "name": "Some stop",
//        "latitude": 43.59,
//        "longitude": 39.74,
//        "road_distances": {
//            "First stop": 4300,
//            "Second stop": 2000
//        }
//    }
//  )");
//  auto json_dict = json::Load(input).GetRoot().AsMap();
//  visitors::JSONDeserializeVisitor visitor(json_dict);
//
//  Visit(visitor, testing);
//
//  ASSERT_EQUAL(testing.name, expected.name);
//  ASSERT_EQUAL(testing.position, expected.position);
//  ASSERT_EQUAL(testing.distances, expected.distances);
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, JSONStopDescriptionParsing);
}
}  // namespace descriptions::tests
