#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "stk/RtAudio.h"
#include "device_manager.hpp"

unsigned int DeviceManager::flags = 0;

DeviceManager::DeviceManager() {
    device_id = -1;
    
    if (flags & FORCE_SET_DEVICE) {
        device_id = set_default_device();
    }
}

int DeviceManager::get_id_from_name() {
    RtAudio dac;
    unsigned int devicecount = dac.getDeviceCount();
    for (unsigned int i = 0; i < devicecount; i++) {
        if (dac.getDeviceInfo(i).name.compare(device_name) == 0)
        return i;
	}
    return -1;
}

int DeviceManager::set_default_device() {
    RtAudio dac;
    unsigned int devicecount = dac.getDeviceCount();
    for (unsigned int i = 0; i < devicecount; i++) {
		std::cout << dac.getDeviceInfo(i).name << " - " << i << '\n';
	}
    std::cout << "Choose device:[0-" << devicecount << "]" << std::endl;
    int choosen_device;
    std::string input;
    std::cin >> input;
    choosen_device = std::stoi(input);
    std::fstream file(DeviceManager::userconfig_dir, std::ios::out);
    if (file.fail()) {
        std::cout << "deu ruim" << std::endl;
        file.close();
    
        return 0;
    }
    file << dac.getDeviceInfo(choosen_device).name;
    // choosen_device = 2;
    device_id = choosen_device;
    return choosen_device;
}

int DeviceManager::get_device_id() {
    if (device_id < 0) {
        std::fstream file(DeviceManager::userconfig_dir, std::ios::in);
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
