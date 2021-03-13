#include "requests_tests.h"
#include "requests_json_visitor.h"

using namespace std;
using namespace Json;

void requests::tests::ReadRequestsTest() {
  Json::Dict dict;
  visitors::Keys keys;
  visitors::JSONDeserializeVisitor visitor(dict, keys);
  StopRequest request;
  MapRequest map_request;
  Visit(visitor, request, map_request);
}

void requests::tests::RunTests() {
  TestRunner tr;
  RUN_TEST(tr, ReadRequestsTest);
}
