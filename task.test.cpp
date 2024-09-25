#include <gtest/gtest.h>
#include <string>
#include "task.cpp"

using namespace std;
// g++ task.test.cpp -std=c++20 -o proba /home/krencz/cpp/gtest/googletest/build/lib/libgtest.a

const string LOGFILE="logfile.txt";

TEST(MonitoringSystemTest, EmptyStatistics) {
    MonitoringSystem m;
    m.Onsignal(Start);
    EXPECT_EQ(m.GetErrorCount(), 0);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_EQ(m.GetStatistics(VehicleType::BICYCLE), "");
    EXPECT_EQ(m.GetStatistics(VehicleType::CAR), "");
    EXPECT_EQ(m.GetStatistics(VehicleType::SCOOTER), "");
}

const string ABC001Plate="ABC-001";
const string ABC002Plate="ABC-002";
const string ABC003Plate="ABC-003";

TEST(MonitoringSystemTest, RegisterVehiclesInOrder) {
    MonitoringSystem m;
    m.Onsignal(Start);

    Bicycle b1(ABC002Plate);
    m.Onsignal(b1);
    const string B1=ABC002Plate+" - Bicycle (1)\n";
    EXPECT_EQ(m.GetStatistics(), B1);

    Car c1(ABC001Plate);
    m.Onsignal(c1);
    const string C1=ABC001Plate+" - Car     (1)\n";
    EXPECT_EQ(m.GetStatistics(), C1+B1);

    Scooter s1(ABC001Plate);
    m.Onsignal(s1);
    const string S1=ABC001Plate+" - Scooter (1)\n";
    EXPECT_EQ(m.GetStatistics(), C1+S1+B1);

    Bicycle b2(ABC001Plate);
    m.Onsignal(b2);
    const string B2=ABC001Plate+" - Bicycle (1)\n";
    EXPECT_EQ(m.GetStatistics(), B2+C1+S1+B1);

    const string ZZZ001Plate="ZZZ-001";
    Car c2(ZZZ001Plate);
    m.Onsignal(c2);
    const string C2=ZZZ001Plate+" - Car     (1)\n";
    EXPECT_EQ(m.GetStatistics(), B2+C1+S1+B1+C2);

    Scooter s2(ABC002Plate);
    m.Onsignal(s2);
    const string S2=ABC002Plate+" - Scooter (1)\n";
    EXPECT_EQ(m.GetStatistics(), B2+C1+S1+B1+S2+C2);

    EXPECT_EQ(m.GetStatistics(VehicleType::BICYCLE), B2+B1);
    EXPECT_EQ(m.GetStatistics(VehicleType::CAR), C1+C2);
    EXPECT_EQ(m.GetStatistics(VehicleType::SCOOTER), S1+S2);
    EXPECT_EQ(m.GetErrorCount(), 0);
}

TEST(MonitoringSystemTest, CountVehicles) {
    MonitoringSystem m;
    m.Onsignal(Start);

    Bicycle bike(ABC002Plate);
    m.Onsignal(bike);
    m.Onsignal(bike);
    Bicycle sameBike(ABC002Plate);
    m.Onsignal(sameBike);
    string BIKE=ABC002Plate+" - Bicycle (3)\n";
    EXPECT_EQ(m.GetStatistics(), BIKE);

    Car car(ABC001Plate);
    m.Onsignal(car);
    string CAR=ABC001Plate+" - Car     (1)\n";
    EXPECT_EQ(m.GetStatistics(), CAR+BIKE);

    Scooter scooter(ABC001Plate);
    for(int i=0; i<15; i++){
        m.Onsignal(scooter);
        m.Onsignal(car);
        m.Onsignal(bike);
    }
    BIKE=ABC002Plate+" - Bicycle (18)\n";
    CAR=ABC001Plate+" - Car     (16)\n";
    const string SCOOTER=ABC001Plate+" - Scooter (15)\n";
    EXPECT_EQ(m.GetStatistics(), CAR+SCOOTER+BIKE);
    EXPECT_EQ(m.GetStatistics(VehicleType::BICYCLE), BIKE);
    EXPECT_EQ(m.GetStatistics(VehicleType::CAR), CAR);
    EXPECT_EQ(m.GetStatistics(VehicleType::SCOOTER), SCOOTER);
    EXPECT_EQ(m.GetErrorCount(), 0);
}

string asseblePlate(int i){
    string numberPart = to_string(i);
    int lengthOfNumberPart=numberPart.length();
    string plateTemplate="ABC-000";
    string plate = plateTemplate.substr(0, plateTemplate.size()-lengthOfNumberPart)+to_string(i);
    return plate;
}

const int MAX_NUMBER_OF_VEHICLES = 1000;

bool checkLastLogLineForEntry(string subStr){
    ifstream inputFile("logfile.txt");
    EXPECT_TRUE(inputFile.is_open());
    string logLine="";
    string lastMeaningfulLine="";
    bool found = false;
    while (getline(inputFile, logLine)) {
        if(logLine !=""){
            lastMeaningfulLine = logLine;
        }
    }
    if(lastMeaningfulLine.find(subStr) != string::npos) {
        found = true;
    }
    inputFile.close();
    return found;
}

TEST(MonitoringSystemTest, TryToRegisterTooManyVehicles) {
    MonitoringSystem m;
    m.Onsignal(Start);
    for(int i=0; i<MAX_NUMBER_OF_VEHICLES+1; i++) {
        Bicycle b(asseblePlate(i));
        m.Onsignal(b);
    }
    const string result = m.GetStatistics();
    std::stringstream ss(result);
    int i=0;
    string line;
    while(std::getline(ss, line, '\n')){
        EXPECT_EQ(line, asseblePlate(i++)+" - Bicycle (1)");
    }
    EXPECT_EQ(i, MAX_NUMBER_OF_VEHICLES);
    EXPECT_TRUE(checkLastLogLineForEntry("ERROR: Too many vehicles in the system. Cannot add: ABC1000 - Bicycle"));
}

TEST(MonitoringSystemTest, RegisterMaxAmountOfVehicles) {
    MonitoringSystem m;
    m.Onsignal(Start);
    for(int i=0; i<MAX_NUMBER_OF_VEHICLES; i++) {
        Bicycle b(asseblePlate(i));
        m.Onsignal(b);
    }
    const string result = m.GetStatistics();
    std::stringstream ss(result);
    int i=0;
    string line;
    while(std::getline(ss, line, '\n')){
        EXPECT_EQ(line, asseblePlate(i++)+" - Bicycle (1)");
    }
    EXPECT_EQ(i, MAX_NUMBER_OF_VEHICLES);
    EXPECT_FALSE(checkLastLogLineForEntry("ERROR: Too many vehicles in the system. Cannot add:"));
}


int main(){
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}