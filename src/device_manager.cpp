#include "include/device_manager.h"
#include <iostream>
#include <filesystem>



DeviceManager::DeviceManager() {
    // namespace fs = std::filesystem;
    // fs::path f{ "file.txt" };
    // std::cout << "asd" << std::endl;
    
    // if (std::filesystem::exists(std::filesystem::path{"file.txt"})) std::cout << "yes";
    // else               std::cout << "nope";
    // device_id = -1;
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
