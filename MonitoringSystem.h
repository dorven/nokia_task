#ifndef MONITORING_SYSTEM_H
#define MONITORING_SYSTEM_H

#include <string>
#include <set>
#include <thread>
#include <mutex>
#include <atomic> //17

#include "logging.cpp"
#include "Vehicles.h"

using namespace std;

Logger logger("logfile.txt");

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

