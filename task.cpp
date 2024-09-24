#include <iostream>
#include <string>
#include <set>
#include <thread>

using namespace std;

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
    bool operator< (const Vehicle &right) const {
        return id+VehicleTypeStrings[type] < right.id + VehicleTypeStrings[right.type];
    }
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
    ERROR,
    RESETING
};

class MonitoringSystem{
    const unsigned int RESET_INTERVAL = 1;
    std::thread resetThread;
    set<Vehicle> vehicles;
    States state = INIT;
    unsigned int errorCounter = 0;
    std::atomic<bool> stopFlag=false;
    string getPlaceholderForCar(VehicleType type){
        return type == VehicleType::CAR ? "    ":"";
    }
    string getVehicleLine(auto v){
        return v.id + " - " + VehicleTypeStrings[(int)v.type] + getPlaceholderForCar(v.type) + " (" + to_string(v.count) + ")\n";
    }
public:
    MonitoringSystem(){
        resetThread = std::thread(&MonitoringSystem::handlePeriodicReset, this);
    }
    ~MonitoringSystem(){
        stopFlag = true;
        resetThread.join();
    }
    template <class V>
    void Onsignal(V& vehicleSignal) {
        if(state == ACTIVE){
            auto found = vehicles.find(vehicleSignal);
            if(found == vehicles.end()) vehicles.insert(vehicleSignal);
            else found->count++;
        }
        else if(state == ERROR){
            errorCounter++;
        }
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
                if(state != RESETING){
                    state = RESETING;
                    usleep(100);
                    errorCounter = 0;
                    vehicles.clear();
                    state = ACTIVE;
                    break;
                }
            case RESETING:
                cout<<"Reset is already in progress.\n";
                break;
            default:
                cout<<"Error: Invalid operational signal "<<(int)operationalSignal<<endl;
                break;
        }
    }

    void Onsignal() {
        state = ERROR;
    }

    string GetStatistics() {
        if(state == RESETING) return "Currently being reseted...";
        string result;
        for(auto &v: vehicles){
            result+=getVehicleLine(v);
        }
        return result;
    }

    string GetStatistics(VehicleType type) {
        if(state == RESETING) return "Currently being reseted...";
        string result;
        for(auto &v: vehicles){
            if(type == v.type) {
                result+=getVehicleLine(v);
            }
        }
        return result;
    }

    unsigned int GetErrorCount() {
        return errorCounter;
    }

    void handlePeriodicReset()
    {
        int emplasedSeconds = 0;
        while(!stopFlag){
            sleep(1);
            if(emplasedSeconds >= RESET_INTERVAL){
                cout << "Reseting..." << endl;
                Onsignal(Reset);
                emplasedSeconds = 0;
            }
            emplasedSeconds++;
        }
    }
};

int main(){
    for(int i=0; i<1; i++){
    MonitoringSystem m;
    m.Onsignal(Start);
    Scooter s1("ABC-001");
    m.Onsignal(s1);
    sleep(2);
    m.Onsignal(Reset);
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
}