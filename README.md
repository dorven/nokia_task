# Traffic Monitoring System
### MonitoringSystem Interfaces
MonitoringSystem class should be instantiated first. For example:
```
MonitoringSystem m;
```
Then you can use the exposed methods to perform tasks.
You can send control and vehicle signals with Onsignal method.
It's input parameter can be a vehicle object(Car, Bicycle, Scooter) or a control signal(Start, Stop, Reset).
Onsignal() without parameter indicates an error in the camera system.
```
Onsignal(Car& c);
Onsignal(Bicycle& b);
Onsignal(Scooter& s);

Onsignal(Start);
Onsignal(Stop);
Onsignal(Reset);

Onsignal();
```

To get vehicle statistics use GetStatistics() method with or without VehicleType.
```
GetStatistics()
GetStatistics(VehicleType type)
```

To get error count use GetErrorCount() method.
```
GetErrorCount()
```

### Solution description
Car, Bicycle and Scooter is inherited from Vehicle class. Their implementation can be found in Vehicles.cpp\
I implemented a simple logger which logs to logfile.txt in append mode.\
The MonitoringSystem is implemented in MonitoringSystem.cpp.\
The constructor starts the Periodic reset thread which periodically calls Onsignal(Reset) function.\
The destructor stops the periodic reset thread and with join() it waits until it exits gracefully.
I don't wanted to use deatach() bacause if we don't wait for the thread to exit gracefully than valgrind is complaining about possible memory leaks.
I don't think it is a real memory leak because the OS keeps track of the memory and frees it after the thread exits but CI systems usually don't like possible memory leaks.
This thread resets the system depending on the given time period unless the system is in STOPPED state.\
Reset time period can be configured in the constructor.
```
const int PROD_RESET_INTERVAL_IN_SECONDS = 300;
```
Default value is 300 seconds. It can be changed if provided. For testing the object can be instantiated with much faster reset interval(too fast for human). It can be achived with DEV=true parameter.
```
MonitoringSystem m; // instantiate with normal reset interval
MonitoringSystem m(20); // instantiate with custom reset interval
MonitoringSystem m(1, true); // instantiate with fast reset interval for faster testing

```
The system stores the Vehicles in a set which is an ordered container. Insertion is ~linear complexity but finding a vehicle and increasing its counter is fast(logarithmic complexity).

After MonitoringSystem object is instantiated it will be in INIT state. It does not record Vehicles in this state. We have to start it first via sending a Start signal. It will make the system ACTIVE.
```
Onsignal(Start);
```
Then it will react to vehicle signals. If the vehicle is not yet registered it will register it. If it is already registered it will increase it's counter. It is not possible to register vehicles with empty ID or an ID which only contains spaces.\
GetStatistics() method can be used to return vehicle statistics. If you use it without parameters then it will give back all vehicles.\
If you use it with a VehicleType parameter it will give back all vehicles with the given type.\
The system can be Reseted with Onsignal(Reset). It will clear all vehicle statistics and set error counter to 0.\
It is the same reset which happens periodically. The only difference is that the periodic reset does not impact STOPPED state but this manual reset does.\
ACTIVE system can be STOPPED with Stop signal. In this state it does not handle signals except manual Reset.\
With Onsignal(Error) we can indicate error in the camera system. This will put the system to ERROR state and it will increase the error counter each time it recieves a vehicle signal. It also writes error logs.\
GetErrorCount() method will give back the error count.

### Compile and Run tests:
I used googletest framework to test my code. If you want to execute the tests you have to install googletest framework.\
https://google.github.io/googletest/primer.html\
Then you can compile and execute the tests.
```
g++ TestMonitoringSystem.cpp -Wsign-conversion -Werror -std=c++20 -o TestMonitoringSystem ../gtest/googletest/build/lib/libgtest.a
./TestMonitoringSystem
```

### ManualTester usage:
Manual tester is just a helper what I used to test the MonitoringSystem class. I included it because it might be useful for testing but it is not part of the solution.
It can be compiled and executed with the following commands:
```
g++ ManualTester.cpp -Wsign-conversion -Werror -std=c++20 -o ManualTester
./ManualTester
```
It is a menu driven program which instantiates the MonitoringSystem object and lets you use it's functions with a switch-case based menu.\
Each menu point can be accessed with it's number.
For example if you want to start the system press 2 -> ENTER -> 1 -> ENTER.\
Start belons to "2. Send control signal".
```
Monitoring System Main Menu:
1. Send Vehicle signal
2. Send control signal
3. Get Statistics
4. Get Statistics by Type
5. Get Error Count
6. Quit
```
In the control signal menu it is the first option:
```
Control Signal Menu:
1. Send Start signal
2. Send Stop signal
3. Send Reset signal
4. Send Empty Error signal
5. Cancel
```
You can send other control signals, vehicle signals or read the statistics the same way using the menu.
