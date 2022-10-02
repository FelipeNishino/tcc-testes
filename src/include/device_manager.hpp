#ifndef TCC_DEVICEMANAGER_H
#define TCC_DEVICEMANAGER_H
#include <string>
#include <fstream>
#include <iostream>
#include <sys/types.h>

class DeviceManager {
    private:
        
        static unsigned int flags;
        int device_id;
        std::string device_name;
    public:
        enum DEVICE_MANAGER_FLAGS {
            FORCE_SET_DEVICE = 1
        };
        static void set_flag(DEVICE_MANAGER_FLAGS flag) {flags |= flag;};
        DeviceManager();
        int set_default_device();
        int get_device_id();   
        int get_id_from_name();
};

#endif //TCC_DEVICEMANAGER_H