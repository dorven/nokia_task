#include <iostream>
#include <string>
#include <set>

using namespace std;

enum VehicleType {
    BICYCLE,
    CAR,
    SCOOTER
};

const string VehicleTypeStrings[]={"Bicycle", "Car", "Scooter"};

class Vehicle{
public:
    unsigned short count=0;
    VehicleType type;
    string id;
    Vehicle(VehicleType type, string id):id(id),type(type){}
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

set<Vehicle> vehicles;

void Onsignal(Bicycle& b) {
    Vehicle v(VehicleType::BICYCLE, b.id);
    vehicles.insert(v);
}

void Onsignal(Car& c){
    Vehicle v(VehicleType::CAR, c.id);
    vehicles.insert(v);
}

void Onsignal(Scooter& s){
    Vehicle v(VehicleType::SCOOTER, s.id);
    vehicles.insert(v);
}

string GetStatistics() {
    cout<<vehicles.size()<<endl;
    string result;
    for(auto &v: vehicles){
        result+=v.id + " - " + VehicleTypeStrings[(int)v.type] + " (" + to_string(v.count) + ")\n";
    }
    return result;
}

//OnSignal(Reset);
//OnSignal() -> //signals an error

int main(){
    Scooter s1("ABC-001");
    Onsignal(s1);
    Car c1("ABC-001");
    Onsignal(c1);
    Bicycle b1("ABC-001");
    Onsignal(b1);
    Onsignal(b1);
    cout<<GetStatistics()<<endl;

    return 0;
}