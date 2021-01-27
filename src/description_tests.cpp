#include "description_tests.h"

using namespace std;
using namespace Json;

namespace Descriptions::Tests {

void ReadStopTests() {
  stringstream input(R"({
      "type": "Stop",
      "name": "Улица Лизы Чайкиной",
      "latitude": 43.590317,
      "longitude": 39.746833,
      "road_distances": {
        "Электросети": 4300,
        "Улица Докучаева": 2000
      }
    })");

  Stop expected{
      .name = "Улица Лизы Чайкиной",
      .position = Sphere::Point{Sphere::ConvertToRadians(43.590317),
                                Sphere::ConvertToRadians(39.746833)},
      .distances = {{"Электросети", 4300}, {"Улица Докучаева", 2000}}};

  auto attrs = Load(input).GetRoot().AsMap();

  auto desc = ParseDescription(attrs);

  ASSERT(holds_alternative<Stop>(desc));

  auto stop = get<Stop>(desc);
  ASSERT_EQUAL(stop.name, expected.name);
  ASSERT_EQUAL(stop.position, expected.position);
  ASSERT_EQUAL(stop.distances, expected.distances);
}

void ReadBusesTests() {
  {
    stringstream input(R"({
      "type": "Bus",
      "name": "24",
      "stops": [
        "Улица Докучаева",
        "Параллельная улица",
        "Электросети",
        "Санаторий Родина"
      ],
      "is_roundtrip": false
    })");

    Bus expected{.name = "24",
                 .stop_list = {"Улица Докучаева", "Параллельная улица",
                               "Электросети", "Санаторий Родина", "Электросети",
                               "Параллельная улица", "Улица Докучаева"}};

    auto attrs = Load(input).GetRoot().AsMap();

    auto desc = ParseDescription(attrs);

    ASSERT(holds_alternative<Bus>(desc));

    auto bus = get<Bus>(desc);
    ASSERT_EQUAL(bus.name, expected.name);
    ASSERT_EQUAL(bus.stop_list, expected.stop_list);
  }

  {
    stringstream input(R"({
      "type": "Bus",
      "name": "14",
      "stops": [
        "Улица Лизы Чайкиной",
        "Электросети",
        "Ривьерский мост",
        "Гостиница Сочи",
        "Кубанская улица",
        "По требованию",
        "Улица Докучаева",
        "Улица Лизы Чайкиной"
      ],
      "is_roundtrip": true
    })");

    Bus expected{
        .name = "14",
        .stop_list = {"Улица Лизы Чайкиной", "Электросети", "Ривьерский мост",
                      "Гостиница Сочи", "Кубанская улица", "По требованию",
                      "Улица Докучаева", "Улица Лизы Чайкиной"}};

    auto attrs = Load(input).GetRoot().AsMap();

    auto desc = ParseDescription(attrs);

    ASSERT(holds_alternative<Bus>(desc));

    auto bus = get<Bus>(desc);
    ASSERT_EQUAL(bus.name, expected.name);
    ASSERT_EQUAL(bus.stop_list, expected.stop_list);
  }
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, ReadStopTests);
}
}  // namespace Descriptions::Tests
