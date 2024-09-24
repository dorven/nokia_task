#include <iostream>
#include <string>
#include <set>

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
    Vehicle(VehicleType type, string id):id(id), type(type), count(0){}
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
    STARTED,
    STOPPED,
    ERROR
};

class MonitoringSystem{
    set<Vehicle> vehicles;
    int time_period = 60;
    string getPlaceholderForCar(VehicleType type){
        return type == VehicleType::CAR ? "    ":"";
    }
public:
    template <class V>
    void Onsignal(V& vehicle_signal) {
        auto found = vehicles.find(vehicle_signal);
        if(found == vehicles.end()) vehicles.insert(vehicle_signal);
        else found->count++;
    }

    void Onsignal(OperationalSignal operational_signal) {
        cout<<"Operational signal recieved:"<< operational_signal<<endl;
    }

    string GetStatistics() {
        string result;
        for(auto &v: vehicles){
            result+=v.id + " - " + VehicleTypeStrings[(int)v.type] + getPlaceholderForCar(v.type) + " (" + to_string(v.count+1) + ")\n";
        }
        return result;
    }
};

//OnSignal(Reset);
//OnSignal() -> //signals an error

int main(){
    MonitoringSystem m;
    Scooter s1("ABC-001");
    m.Onsignal(s1);
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
    m.Onsignal(Reset);
    m.Onsignal(Start);
    m.Onsignal(Stop);

    return 0;
}