# Traffic Monitoring System
### Compile and run ManualTester:
g++ ManualTester.cpp -std=c++20 -o ManualTester
./ManualTester
### Compile and Run tests:
Note that I used googletest to test my code. If you want to execute them you have to install googletest first.\
https://google.github.io/googletest/primer.html\
g++ TestMonitoringSystem.cpp -std=c++20 -o TestMonitoringSystem ../gtest/googletest/build/lib/libgtest.a\
./TestMonitoringSystem
### MonitoringSystem Usage
MonitoringSystem class should be instantiated first.
```
MonitoringSystem m;
```
Then you can use the exposed methods to perform tasks.
You can send control and vehicle signals with Onsignal method.
It's input parameter can be a vehicle object(Car, Bicycle, Scooter) or a control signal(Start, Stop, Reset).
Onsignal() without parameter indicates an error in the camera system.
```
Onsignal(Car& c);
Onsignal(Bicycle& c);
Onsignal(Scooter& c);



```

### Solution description
