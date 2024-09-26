#include <iostream>
#include <string>
#include <thread>
#include <unistd.h> //17

#include "MonitoringSystem.h"

// g++ task.cpp -std=c++20 -o MonitoringSystem

using namespace std;

bool Vehicle::operator< (const Vehicle &right) const {
    return id+VehicleTypeStrings[type] < right.id + VehicleTypeStrings[right.type];
}

string MonitoringSystem::getPlaceholderForCar(VehicleType type){
    return type == VehicleType::CAR ? "    ":"";
}

string MonitoringSystem::getVehicleLine(const Vehicle& v){
    return v.id + " - " + VehicleTypeStrings[(int)v.type] + getPlaceholderForCar(v.type) + " (" + to_string(v.count) + ")\n";
}

MonitoringSystem::MonitoringSystem(){
    logger.log(Info, "MonitoringSystem started.");
    resetThread = std::thread(&MonitoringSystem::handlePeriodicReset, this);
}

MonitoringSystem::~MonitoringSystem(){
    stopFlag = true;
    // deatach gives false positive valgrind findings.
    // use resetThread.join(); if it is a problem. it will make the destructor a bit slower
    // because it has to wait for the thread to exit gracefully.
    resetThread.detach();
    logger.log(Info, "MonitoringSystem stopped.");
}

void MonitoringSystem::setPeriodicResetInterval(int interval){
    if(interval<1 || interval>INT32_MAX-1){
        logger.log(Error, "Invalid periodic reset interval. It should be between 1 and 2147483646.");
        return;
    }
    PERIODIC_RESET_INTERVAL = interval;
    logger.log(Info, "Periodic reset interval set to " + to_string(PERIODIC_RESET_INTERVAL) + " seconds.");
}

template <class V>
void MonitoringSystem::Onsignal(V& vehicleSignal) {
    mtx.lock();
    if(state == ACTIVE){
        auto found = vehicles.find(vehicleSignal);
        if(found == vehicles.end()){
            if(vehicles.size() < 1000){
                vehicles.insert(vehicleSignal);
            }
            else logger.log(
                Error,
                "Too many vehicles in the system. Cannot add: " + vehicleSignal.id + " - " + VehicleTypeStrings[(int)vehicleSignal.type]
            );
        }
        else found->count++;
    }
    else if(state == ERROR){
        errorCounter++;
        logger.log(
            Error,
            "Error in the camera system. Cannot add: " + vehicleSignal.id + " - " + VehicleTypeStrings[(int)vehicleSignal.type]
        );
    }
    mtx.unlock();
}

void MonitoringSystem::Onsignal(OperationalSignal operationalSignal) {
    switch(operationalSignal){
        case START:
            if(state == INIT) state = ACTIVE;
            break;
        case STOP:
            if(state == ACTIVE) state = STOPPED;
            break;
        case RESET:
            mtx.lock();
            errorCounter = 0;
            vehicles.clear();
            state = ACTIVE;
            mtx.unlock();
            break;
        default:
            cout<<"Error: Invalid operational signal "<<(int)operationalSignal<<endl;
            break;
    }
}

void MonitoringSystem::Onsignal() {
    state = ERROR;
    logger.log(Error, "Error in the camera system. MonitoringSystem is in ERROR state.");
}

string MonitoringSystem::GetStatistics() {
    mtx.lock();
    string result;
    for(auto &v: vehicles){
        result+=getVehicleLine(v);
    }
    mtx.unlock();
    return result;
}
string MonitoringSystem::GetStatistics(VehicleType type) {
    mtx.lock();
    string result;
    for(auto &v: vehicles){
        if(type == v.type) {
            result+=getVehicleLine(v);
        }
    }
    mtx.unlock();
    return result;
}
unsigned int MonitoringSystem::GetErrorCount() {
    return errorCounter;
}
void MonitoringSystem::handlePeriodicReset()
{
    int emplasedSeconds = 0;
    while(!stopFlag){
        if(state==STOPPED) emplasedSeconds=0;
        if(emplasedSeconds >= PERIODIC_RESET_INTERVAL){
            Onsignal(Reset);
            emplasedSeconds = 0;
        }
        sleep(1);
        emplasedSeconds++;
    }
}

/*int main(){
    MonitoringSystem m;
    for(int i=0; i<1; i++){
    m.Onsignal(Start);
    Scooter s1("ABC-001");
    m.Onsignal(s1);
    sleep(2);
    //m.Onsignal(Reset);
    m.Onsignal(s1);
    Car c1("ABC-001");
    m.Onsignal(c1);
    Bicycle b1("ABC-001");
    m.Onsignal(b1);
    Bicycle b2("ABC-002");
    m.Onsignal(b2);
    m.Onsignal(s1);
    Scooter s11("ABC-001");
    m.Onsignal(s11);
    Bicycle b22("ABC-002");
    m.Onsignal(b22);
    cout<<m.GetStatistics()<<endl;
    cout<<m.GetStatistics(VehicleType::BICYCLE)<<endl;
    cout<<"Error count: "<<m.GetErrorCount()<<endl;
    }

    return 0;
}*/