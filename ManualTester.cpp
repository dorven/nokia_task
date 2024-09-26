#include <iostream>

#include "MonitoringSystem.cpp"

using namespace std;

void sendCarSignal(MonitoringSystem& m){
    cout<<"Enter license plate:";
    string plate;
    cin>>plate;
    Car c(plate);
    m.Onsignal(c);
}

void sendBikeSignal(MonitoringSystem& m){
    cout<<"Enter license plate:";
    string plate;
    cin>>plate;
    Bicycle b(plate);
    m.Onsignal(b);
}

void sendScooterSignal(MonitoringSystem& m){
    cout<<"Enter license plate:";
    string plate;
    cin>>plate;
    Scooter s(plate);
    m.Onsignal(s);
}

void showVehicleSignalMenu(MonitoringSystem& m){
    cout<<"Vehicle Signal Menu:"<<endl;
    cout<<"1. Send Car signal"<<endl;
    cout<<"2. Send Bicycle signal"<<endl;
    cout<<"3. Send Scooter signal"<<endl;
    cout<<"4. Cancel"<<endl;
    cout<<"Enter your choice: ";
    int choice;
    cin>>choice;
    string plate="";
    switch(choice){
        case 1:
            sendCarSignal(m);
            break;
        case 2:
            sendBikeSignal(m);
            break;
        case 3:
            sendScooterSignal(m);
            break;
        case 4:
            return;
        default:
            cout<<"Invalid choice!"<<endl;
            break;
    }
}

void showControlSignalMenu(MonitoringSystem& m){
    cout<<"Vehicle Signal Menu:"<<endl;
    cout<<"1. Send Start signal"<<endl;
    cout<<"2. Send Stop signal"<<endl;
    cout<<"3. Send Reset signal"<<endl;
    cout<<"4. Send Empty Error signal"<<endl;
    cout<<"5. Cancel"<<endl;
    cout<<"Enter your choice: ";
    int choice;
    cin>>choice;
    string plate="";
    switch(choice){
        case 1:
            m.Onsignal(Start);
            break;
        case 2:
            m.Onsignal(Stop);
            break;
        case 3:
            m.Onsignal(Reset);
            break;
        case 4:
            m.Onsignal();
            break;
        case 5:
            return;
        default:
            cout<<"Invalid choice!"<<endl;
            break;
    }
}

void showStatistics(MonitoringSystem& m){
    cout<<endl<<"Vehicle Statistics:"<<endl;
    cout<<m.GetStatistics()<<endl;
}

void setPeriodicResetInterval(MonitoringSystem& m){
    cout<<"Please enter reset interval in secods:"<<endl;
    int interval=3600;
    cin >> interval;
    m.setPeriodicResetInterval(interval);
}

void showStatisticsByType(MonitoringSystem& m){
    cout<<endl<<"Which type of vehicle statistics you want to see?"<<endl;
    cout<<"1. Bicycle"<<endl;
    cout<<"2. Car"<<endl;
    cout<<"3. Scooter"<<endl;
    cout<<"Enter your choice: ";
    int choice;
    cin>>choice;
    if(choice<1 || choice > 3){cout<<"Invalid choice!"<<endl; return;}
    cout<<VehicleTypeStrings[choice-1]<<" statistics:"<<endl;
    switch(choice){
        case 1:
            cout<<m.GetStatistics(VehicleType::BICYCLE)<<endl;
            break;
        case 2:
            cout<<m.GetStatistics(VehicleType::CAR)<<endl;
            break;
        case 3:
            cout<<m.GetStatistics(VehicleType::SCOOTER)<<endl;
            break;
        default:
            cout<<"Invalid choice!"<<endl;
            break;
    }
}

void showMainMenu(MonitoringSystem& m){
    bool exit=false;
    while(!exit){
        cout<<"Monitoring System Main Menu:"<<endl;
        cout<<"1. Send Vehicle signal"<<endl;
        cout<<"2. Send control signal"<<endl;
        cout<<"3. Set Periodic Reset Interval"<<endl;
        cout<<"4. Get Statistics"<<endl;
        cout<<"5. Get Statistics by Type"<<endl;
        cout<<"6. Get Error Count"<<endl;
        cout<<"7. Quit"<<endl;
        cout<<"Enter your choice: ";
        int choice;
        cin>>choice;
        switch(choice){
            case 1:
                showVehicleSignalMenu(m);
                break;
            case 2:
                showControlSignalMenu(m);
                break;
            case 3:
                setPeriodicResetInterval(m);
                break;
            case 4:
                showStatistics(m);
                break;
            case 5:
                showStatisticsByType(m);
                break;
            case 6:
                cout<<endl<<"Error Count: "<<m.GetErrorCount()<<endl;
                break;
            case 7:
                exit=true;
                break;
            default:
                cout<<"Invalid choice!"<<endl;
                break;
        }
    }
}

int main(){
    MonitoringSystem m;
    showMainMenu(m);
    return 0;
}