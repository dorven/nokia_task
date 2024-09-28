#include <gtest/gtest.h>
#include <string>

#include "MonitoringSystem.cpp"

using namespace std;
// g++ TestMonitoringSystem.cpp -Wsign-conversion -Werror -std=c++20 -o TestMonitoringSystem ../gtest/googletest/build/lib/libgtest.a

const string LOGFILE="logfile.txt";

TEST(BasicFunctionality, EmptyStatistics) {
    MonitoringSystem m(0);
    EXPECT_EQ(m.GetErrorCount(), 0);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_EQ(m.GetStatistics(VehicleType::BICYCLE), "");
    EXPECT_EQ(m.GetStatistics(VehicleType::CAR), "");
    EXPECT_EQ(m.GetStatistics(VehicleType::SCOOTER), "");
}

const string ABC001Plate="ABC-001";
const string ABC002Plate="ABC-002";
const string ABC003Plate="ABC-003";

TEST(BasicFunctionality, RegisterVehiclesInOrder) {
    MonitoringSystem m(0);
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

TEST(BasicFunctionality, CountVehicles) {
    MonitoringSystem m(0);
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
    long unsigned int lengthOfNumberPart=numberPart.length();
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

TEST(MaxNumberOfVehicles, TryToRegisterTooManyVehicles) {
    MonitoringSystem m(0);
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

TEST(MaxNumberOfVehicles, RegisterMaxAmountOfVehicles) {
    MonitoringSystem m(0);
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

TEST(StatesAndStateChanges, NotRegisterVehiclesInInitState) {
    MonitoringSystem m(0);
    Bicycle bike(ABC001Plate);
    m.Onsignal(bike);

    EXPECT_EQ(m.GetErrorCount(), 0);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_EQ(m.GetStatistics(VehicleType::BICYCLE), "");
}

TEST(StatesAndStateChanges, NotRegisterOrCountVehiclesInStoppedState) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    Bicycle bike(ABC001Plate);
    m.Onsignal(bike);
    const string BIKE=ABC001Plate+" - Bicycle (1)\n";
    EXPECT_EQ(m.GetStatistics(), BIKE);

    //Further registration is not possible because the system is stopped
    m.Onsignal(Stop);
    Bicycle bike2(ABC002Plate);
    m.Onsignal(bike2);
    m.Onsignal(bike);

    // There's only one vehicle present. The one we added before stopping the system.
    EXPECT_EQ(m.GetStatistics(), BIKE);
    EXPECT_EQ(m.GetErrorCount(), 0);
}

TEST(StatesAndStateChanges, ResetShouldClearVehicleStatisticsAndActivate) {
    MonitoringSystem m(0);
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

    // Reset should clear vehicles
    m.Onsignal(Reset);
    EXPECT_EQ(m.GetStatistics(), "");

    //After Reset we must be able to register vehicles again
    m.Onsignal(c1);
    m.Onsignal(b1);
    m.Onsignal(s1);
    EXPECT_EQ(m.GetStatistics(), C1+S1+B1);
}

TEST(StatesAndStateChanges, ResetShouldActivateIfStopped) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    m.Onsignal(Stop);

    // Check if it's really stopped. No registration possible.
    Bicycle b1(ABC002Plate);
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), "");

    // After Reset vehicle registration should work
    m.Onsignal(Reset);
    m.Onsignal(b1);
    const string B1=ABC002Plate+" - Bicycle (1)\n";
    EXPECT_EQ(m.GetStatistics(), B1);
}

TEST(StatesAndStateChanges, ResetShouldActivateIfInit) {
    MonitoringSystem m(0);

    // Check if it's really init. No registration possible.
    Bicycle b1(ABC002Plate);
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), "");

    // After Reset vehicle registration should work.
    m.Onsignal(Reset);
    m.Onsignal(b1);
    const string B1=ABC002Plate+" - Bicycle (1)\n";
    EXPECT_EQ(m.GetStatistics(), B1);
}

TEST(StatesAndStateChanges, ErrorStateShouldCountErrors) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    m.Onsignal();
    EXPECT_TRUE(checkLastLogLineForEntry("Error in the camera system. MonitoringSystem is in ERROR state."));
    Bicycle b1(ABC002Plate);
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_EQ(m.GetErrorCount(), 1);
    EXPECT_TRUE(checkLastLogLineForEntry("Error in the camera system. Cannot add: ABC-002 - Bicycle"));
}

TEST(StatesAndStateChanges, StopStartCannotFixErrorState) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    m.Onsignal();
    EXPECT_TRUE(checkLastLogLineForEntry("Error in the camera system. MonitoringSystem is in ERROR state."));

    m.Onsignal(Stop);
    m.Onsignal(Start);

    Bicycle b1(ABC002Plate);
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_EQ(m.GetErrorCount(), 1);
    EXPECT_TRUE(checkLastLogLineForEntry("Error in the camera system. Cannot add: ABC-002 - Bicycle"));
}

TEST(StatesAndStateChanges, ResetCanFixErrorState) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    m.Onsignal();
    EXPECT_TRUE(checkLastLogLineForEntry("Error in the camera system. MonitoringSystem is in ERROR state."));

    m.Onsignal(Reset);

    Bicycle b1(ABC002Plate);
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), ABC002Plate+" - Bicycle (1)\n");
    EXPECT_EQ(m.GetErrorCount(), 0);
    EXPECT_FALSE(checkLastLogLineForEntry("Error in the camera system. Cannot add: ABC-002 - Bicycle"));
}

TEST(StatesAndStateChanges, ShouldNotRecordErrorSignalInStopped) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    m.Onsignal(Stop);
    m.Onsignal();
    Bicycle b1(ABC001Plate);
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_EQ(m.GetErrorCount(), 0);
}

TEST(StatesAndStateChanges, ShouldNotRecordErrorSignalInInit) {
    MonitoringSystem m(0);
    m.Onsignal();
    Bicycle b1(ABC001Plate);
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_EQ(m.GetErrorCount(), 0);
}

TEST(PeriodicReset, ShouldResetPeriodically) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    Bicycle b1(ABC002Plate);
    m.Onsignal(b1);
    m.Onsignal();
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), ABC002Plate+" - Bicycle (1)\n");
    EXPECT_EQ(m.GetErrorCount(), 1);

    //After having a vehicle and an error in the system let's wait for the periodic reset to clear it
    usleep(60000);
    EXPECT_EQ(m.GetErrorCount(), 0);
    EXPECT_EQ(m.GetStatistics(), "");
}

TEST(PeriodicReset, ShouldNotResetPeriodicallyIfStopped) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    Bicycle b1(ABC002Plate);
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), ABC002Plate+" - Bicycle (1)\n");
    m.Onsignal(Stop);

    // After having a vehicle and an error in the system let's wait for the periodic reset
    // The system is stopped so it is not going to reset
    usleep(60000);
    EXPECT_EQ(m.GetStatistics(), ABC002Plate+" - Bicycle (1)\n");
}


TEST(Edgecases, EmptyVehicleIdIsNotAllowed) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    Bicycle b1("");
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_TRUE(checkLastLogLineForEntry("Vehicle ID cannot be empty."));

}

TEST(Edgecases, OnlySpaceVehicleIdIsNotAllowed) {
    MonitoringSystem m(0);
    m.Onsignal(Start);
    Bicycle b1("   ");
    m.Onsignal(b1);
    EXPECT_EQ(m.GetStatistics(), "");
    EXPECT_TRUE(checkLastLogLineForEntry("Vehicle ID cannot be empty."));
}

int main(){
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}