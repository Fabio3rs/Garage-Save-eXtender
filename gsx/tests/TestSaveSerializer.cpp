#include "../CExtraSaveData.h"
#include <gtest/gtest.h>
#include <sstream>

TEST(TestSaveSerializer, SerializeAndUnserializeData) {
    srand(time(nullptr));
    auto &inst = CExtraSaveData::inst();

    std::vector<uint8_t> cmpBytes{0, 1, 2, 3};

    const int CarModel = rand();

    {
        car carData;
        carData.data["licensePlate"].bytes = cmpBytes;
        carData.model = CarModel;
        carData.pos.x = 10.0;
        carData.pos.y = 100.0;
        carData.pos.z = 1000.0;

        inst.data.push_back(carData);
    }

    std::stringstream serializedData;
    inst.saves(0, serializedData);

    inst.data.clear();

    inst.loads(0, serializedData);

    EXPECT_EQ(inst.data.size(), 1);

    if (inst.data.empty()) {
        return;
    }

    const auto &loadedCar = inst.data.at(0);
    const auto &licensePlate = loadedCar.data.at("licensePlate");

    EXPECT_EQ(licensePlate.bytes, cmpBytes);
    EXPECT_EQ(CarModel, loadedCar.model);
    EXPECT_FLOAT_EQ(loadedCar.pos.x, 10.0);
    EXPECT_FLOAT_EQ(loadedCar.pos.y, 100.0);
    EXPECT_FLOAT_EQ(loadedCar.pos.z, 1000.0);
}
