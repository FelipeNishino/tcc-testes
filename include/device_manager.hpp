#ifndef TCC_DEVICEMANAGER_H
#define TCC_DEVICEMANAGER_H

#include <string>

class DeviceManager {
    private:
        inline static std::string userconfig_dir = "data/userconfig.txt";
        static unsigned int flags;
        int device_id;
        std::string device_name;
    public:
        enum FLAGS {
            FORCE_SET_DEVICE = 1
        };
        static void set_flag(FLAGS flag) {flags |= flag;};
        DeviceManager();
        int set_default_device();
        int get_device_id();   
        int get_id_from_name();
};



#endif //TCC_DEVICEMANAGER_H