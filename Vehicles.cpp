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
    Vehicle(VehicleType type, string id):type(type),id(id),count(1){}

private:
    VehicleType type;
    string id;
    mutable unsigned int count=0;

    friend class MonitoringSystem;
};

class Bicycle: public Vehicle{
public:
    Bicycle()=delete;
    Bicycle(const string id):Vehicle(VehicleType::BICYCLE, id){}
};

class Car: public Vehicle{
public:
    Car()=delete;
    Car(const string id):Vehicle(VehicleType::CAR, id){}
};

class Scooter: public Vehicle{
public:
    Scooter()=delete;
    Scooter(const string id):Vehicle(VehicleType::SCOOTER, id){}
};