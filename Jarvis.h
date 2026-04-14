#ifndef Jarvis_H
#define Jarvis_H
#include <string>

class Jarvis {
    public:
        Jarvis();

        void listen();
        void greet();

        std::string getWeather(std::string location);
};


#endif