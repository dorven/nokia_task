#ifndef MONITORING_SYSTEM_H
#define MONITORING_SYSTEM_H

#include <string>
#include <set>
#include <thread>
#include <mutex>
#include <atomic> //17

#include "logging.cpp"

using namespace std;

Logger logger("logfile.txt");

enum VehicleType {
    BICYCLE,
    CAR,
    SCOOTER,
};

const string VehicleTypeStrings[]={"Bicycle", "Car", "Scooter"};

class Vehicle{
public:
    mutable unsigned short count=0;
    VehicleType type;
    string id;
    Vehicle(){}
    Vehicle(VehicleType type, string id):id(id), type(type), count(1){}
    bool operator< (const Vehicle &right) const;
};

class Bicycle: public Vehicle{
public:
    Bicycle(string id):Vehicle(VehicleType::BICYCLE, id){}
};

class Car: public Vehicle{
public:
    Car(string id):Vehicle(VehicleType::CAR, id){}
};

class Scooter: public Vehicle{
public:
    Scooter(string id):Vehicle(VehicleType::SCOOTER, id){}
};

enum OperationalSignal {
    RESET,
    START,
    STOP
}

const Start=OperationalSignal::START;
const auto Stop=OperationalSignal::STOP;
const auto Reset=OperationalSignal::RESET;

enum States{
    INIT,
    ACTIVE,
    STOPPED,
    ERROR
};

class MonitoringSystem{
    unsigned int PERIODIC_RESET_INTERVAL = 60;
    std::mutex mtx; // Periodic reset can collide with user operations so we have to lock
    std::thread resetThread;
    set<Vehicle> vehicles;
    States state = INIT;
    unsigned int errorCounter = 0;
    std::atomic<bool> stopFlag=false;
    string getPlaceholderForCar(VehicleType type);
    string getVehicleLine(const Vehicle& v);
public:
    MonitoringSystem();
    ~MonitoringSystem();
    void setPeriodicResetInterval(int interval);

    template <class V>
    void Onsignal(V& vehicleSignal);
    void Onsignal(OperationalSignal operationalSignal);
    void Onsignal();

    string GetStatistics();

    string GetStatistics(VehicleType type);

    unsigned int GetErrorCount();

    void handlePeriodicReset();
};

#endif // MONITORING_SYSTEM_H

