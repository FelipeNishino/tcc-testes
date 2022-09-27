#ifndef TCC_DEVICEMANAGER_H
#define TCC_DEVICEMANAGER_H
#include <string>
// #include <filesystem>
#include <fstream>
#include <iostream>
// class Device {
    
// };

class DeviceManager {
    private:
        int device_id;
        std::string device_name;
    public:
        DeviceManager();
        int set_default_device();
        int get_device_id();   
        int get_id_from_name();
};

#endif //TCC_DEVICEMANAGER_H