#include "include/device_manager.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include "stk/RtAudio.h"
#include <string>

DeviceManager::DeviceManager() {
    device_id = -1;
    // device_id = (int*) malloc(sizeof(int));
    // device_id = nullptr;
    // namespace fs = std::filesystem;
    // fs::path f{ "file.txt" };
    // std::cout << "asd" << std::endl;
    
    // if (std::filesystem::exists(std::filesystem::path{"file.txt"})) std::cout << "yes";
    // else               std::cout << "nope";
    // device_id = -1;
}

int DeviceManager::get_id_from_name() {
    RtAudio dac;
    int devicecount = dac.getDeviceCount();
    for (unsigned int i = 0; i < devicecount; i++) {
        if (dac.getDeviceInfo(i).name.compare(device_name) == 0)
        return i;
	}
    return -1;
}

int DeviceManager::set_default_device() {
    RtAudio dac;
    int devicecount = dac.getDeviceCount();
    for (unsigned int i = 0; i < devicecount; i++) {
		std::cout << dac.getDeviceInfo(i).name << " - " << i << '\n';
	}
    std::cout << "Choose device:[0-" << devicecount << "]" << std::endl;
    int choosen_device;
    std::string input;
    std::cin >> input;
    choosen_device = std::stoi(input);
    std::fstream file("userconfig.txt", std::ios::out);
    if (file.fail()) {
        std::cout << "deu ruim" << std::endl;
        file.close();
    
        return 0;
    }
    file << dac.getDeviceInfo(choosen_device).name;
    return choosen_device;
}

int DeviceManager::get_device_id() {
    if (device_id < 0) {
        std::fstream file("userconfig.txt", std::ios::in);
        if (file.fail()) {
            file.close();
            set_default_device();
            return 0;
        }
        getline(file, device_name);

        device_id = get_id_from_name();
        // getline(file, device_name);
        file.close();
    }
            
    return device_id;
}

// /**
//  * The Singleton class defines the `GetInstance` method that serves as an
//  * alternative to constructor and lets clients access the same instance of this
//  * class over and over.
//  */
// class Singleton
// {
// private:
//     static Singleton * pinstance_;
//     static std::mutex mutex_;

// protected:
//     Singleton(const std::string value): value_(value)
//     {
//     }
//     ~Singleton() {}
//     std::string value_;

// public:
//     /**
//      * Singletons should not be cloneable.
//      */
//     Singleton(Singleton &other) = delete;
//     /**
//      * Singletons should not be assignable.
//      */
//     void operator=(const Singleton &) = delete;
//     /**
//      * This is the static method that controls the access to the singleton
//      * instance. On the first run, it creates a singleton object and places it
//      * into the static field. On subsequent runs, it returns the client existing
//      * object stored in the static field.
//      */

//     static Singleton *GetInstance(const std::string& value);
//     /**
//      * Finally, any singleton should define some business logic, which can be
//      * executed on its instance.
//      */
//     void SomeBusinessLogic()
//     {
//         // ...
//     }
    
//     std::string value() const{
//         return value_;
//     } 
// };

// /**
//  * Static methods should be defined outside the class.
//  */

// Singleton* Singleton::pinstance_{nullptr};
// std::mutex Singleton::mutex_;

// /**
//  * The first time we call GetInstance we will lock the storage location
//  *      and then we make sure again that the variable is null and then we
//  *      set the value. RU:
//  */
// Singleton *Singleton::GetInstance(const std::string& value)
// {
//     std::lock_guard<std::mutex> lock(mutex_);
//     if (pinstance_ == nullptr)
//     {
//         pinstance_ = new Singleton(value);
//     }
//     return pinstance_;
// }
