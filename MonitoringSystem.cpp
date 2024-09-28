#include <iostream>
#include <string>
#include <set>
#include <thread>
#include <mutex>
#include <atomic>
#include <unistd.h>

#include "Vehicles.cpp"
#include "Logging.cpp"

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
    MonitoringSystem(unsigned int RESET_INTERVAL_IN_SECONDS=300):
        RESET_INTERVAL_IN_SECONDS(RESET_INTERVAL_IN_SECONDS){
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
        static_assert(
            std::is_base_of<Vehicle, V>::value, "The object recieved is not a valid Vehicle"
        );
        if(vehicleSignal.id.erase(0, vehicleSignal.id.find_first_not_of(' ')) == ""){
            logger.log(Error, "Vehicle ID cannot be empty.");
            return;
        }
        mtx.lock();
        if(state == ACTIVE){
            auto found = vehicles.find(vehicleSignal);
            if(found == vehicles.end()){
                if(vehicles.size() < 1000){
                    vehicles.emplace(vehicleSignal);
                }
                else logger.log(
                    Error,
                    "Too many vehicles in the system. Cannot add: " + vehicleSignal.id + " - " +
                        VehicleTypeStrings[(int)vehicleSignal.type]
                );
            }
            else found->count++;
        }
        else if(state == ERROR){
            errorCounter++;
            logger.log(
                Error,
                "Error in the camera system. Cannot add: " + vehicleSignal.id + " - " +
                    VehicleTypeStrings[(int)vehicleSignal.type]
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
        if(state == ACTIVE){
            state = ERROR;
            logger.log(Error, "Error in the camera system. MonitoringSystem is in ERROR state.");
        }
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
    const unsigned int RESET_INTERVAL_IN_SECONDS = 300;
    const unsigned int MIN_RESET_INTERVAL_IN_MICRO_SECONDS = 40000;
    const unsigned int RESET_INTERVAL_MICROSECONDS =
        RESET_INTERVAL_IN_SECONDS < 1 ?
            MIN_RESET_INTERVAL_IN_MICRO_SECONDS : RESET_INTERVAL_IN_SECONDS * 1000000;
    std::mutex mtx; // Periodic reset can collide with user operations so we have to lock
    std::thread resetThread;
    set<Vehicle> vehicles;
    States state = INIT;
    unsigned int errorCounter = 0;
    std::atomic<bool> stopFlag=false;
    void handlePeriodicReset()
    {
        const unsigned int waitTime =
            RESET_INTERVAL_MICROSECONDS == MIN_RESET_INTERVAL_IN_MICRO_SECONDS ? 50000 : 1000000;
        unsigned int emplasedMicroSeconds = 0;
        while(!stopFlag){
            if(state==STOPPED) emplasedMicroSeconds=0;
            if(emplasedMicroSeconds >= RESET_INTERVAL_MICROSECONDS){
                Onsignal(Reset);
                emplasedMicroSeconds = 0;
            }
            usleep(waitTime);
            emplasedMicroSeconds+=waitTime;
        }
    }
    string getPlaceholderForCar(VehicleType type){
        return type == VehicleType::CAR ? "    " : "";
    }
    string getVehicleLine(const Vehicle& v){
        return v.id + " - " + VehicleTypeStrings[(int)v.type] + getPlaceholderForCar(v.type) +
            " (" + to_string(v.count) + ")\n";
    }
};