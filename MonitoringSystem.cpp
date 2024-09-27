#include <iostream>
#include <string>
#include <set>
#include <thread>
#include <mutex>
#include <atomic> //17
#include <unistd.h> //17

#include "Vehicles.cpp"
#include "Logging.cpp"

// g++ MonitoringSystem.cpp -std=c++20 -o MonitoringSystem

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
public:
    MonitoringSystem(bool DEV=false):DEV(DEV){
        logger.log(Info, "MonitoringSystem started.");
        resetThread = std::thread(&MonitoringSystem::handlePeriodicReset, this);
    }
    ~MonitoringSystem(){
        stopFlag = true;
        resetThread.join();
        logger.log(Info, "MonitoringSystem stopped.");
    }

    template <class V>
    void Onsignal(V& vehicleSignal) {
        //Give meaningful compile error if the input parameter is not a valid vehicle
        static_assert(std::is_base_of<Vehicle, V>::value, "The object recieved is not a valid Vehicle");
        if(vehicleSignal.id == ""){
            logger.log(Error, "Vehicle ID cannot be empty.");
            return;
        }
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

    void Onsignal(OperationalSignal operationalSignal) {
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
                logger.log(Error,"Error: Invalid operational signal "+(int)operationalSignal);
                break;
        }
    }

    void Onsignal() {
        state = ERROR;
        logger.log(Error, "Error in the camera system. MonitoringSystem is in ERROR state.");
    }

    string GetStatistics() {
        mtx.lock();
        string result;
        for(auto &v: vehicles){
            result+=getVehicleLine(v);
        }
        mtx.unlock();
        return result;
    }

    string GetStatistics(VehicleType type) {
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

    unsigned int GetErrorCount() {
        return errorCounter;
    }

private:
    bool DEV=false;  // instantiate with fast reset interval for faster testing
    const unsigned int PERIODIC_RESET_INTERVAL = DEV ? 1 : 3600;
    std::mutex mtx; // Periodic reset can collide with user operations so we have to lock
    std::thread resetThread;
    set<Vehicle> vehicles;
    States state = INIT;
    unsigned int errorCounter = 0;
    std::atomic<bool> stopFlag=false;
    void handlePeriodicReset()
    {
        const int waitTime = DEV ? 50000 : 1000000;
        int emplasedSeconds = 0;
        while(!stopFlag){
            if(state==STOPPED) emplasedSeconds=0;
            if(emplasedSeconds >= PERIODIC_RESET_INTERVAL){
                Onsignal(Reset);
                emplasedSeconds = 0;
            }
            usleep(waitTime);
            emplasedSeconds++;
        }
    }
    string getPlaceholderForCar(VehicleType type){
        return type == VehicleType::CAR ? "    ":"";
    }
    string getVehicleLine(const Vehicle& v){
        return v.id + " - " + VehicleTypeStrings[(int)v.type] + getPlaceholderForCar(v.type) + " (" + to_string(v.count) + ")\n";
    }
};