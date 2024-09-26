#include <string>

using namespace std;

enum VehicleType {
    BICYCLE,
    CAR,
    SCOOTER,
};

const string VehicleTypeStrings[]={"Bicycle", "Car", "Scooter"};

class Vehicle{
public:
    bool operator< (const Vehicle &right) const {
        return id+VehicleTypeStrings[type] < right.id + VehicleTypeStrings[right.type];
    }
protected:
    Vehicle()=delete;
    Vehicle(VehicleType type, string id):id(id), type(type), count(1){}

private:
    mutable unsigned short count=0;
    VehicleType type;
    string id;

    friend class MonitoringSystem;
};

class Bicycle: public Vehicle{
public:
    Bicycle()=delete;
    Bicycle(string id):Vehicle(VehicleType::BICYCLE, id){}
};

class Car: public Vehicle{
public:
    Car()=delete;
    Car(string id):Vehicle(VehicleType::CAR, id){}
};

class Scooter: public Vehicle{
public:
    Scooter()=delete;
    Scooter(string id):Vehicle(VehicleType::SCOOTER, id){}
};