#ifndef VEHICLES_H
#define VEHICLES_H

enum VehicleType {
    BICYCLE,
    CAR,
    SCOOTER,
};

const string VehicleTypeStrings[]={"Bicycle", "Car", "Scooter"};

class Vehicle{
public:
    Vehicle(){}
    Vehicle(VehicleType type, string id):id(id), type(type), count(1){}
    bool operator< (const Vehicle &right) const;
private:
    mutable unsigned short count=0;
    VehicleType type;
    string id;

    friend class MonitoringSystem; 
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

#endif // VEHICLES_H