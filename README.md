# Traffic Monitoring System
### Compile and run:
g++ MonitoringSystem.cpp -std=c++20 -o MonitoringSystem
./MonitoringSystem
### Compile and Run tests:
Note that I used googletest to test my code. If you want to execute them you have to install googletest first.\
https://google.github.io/googletest/primer.html\
g++ TestMonitoringSystem.cpp -std=c++20 -o TestMonitoringSystem ../gtest/googletest/build/lib/libgtest.a\
./TestMonitoringSystem\