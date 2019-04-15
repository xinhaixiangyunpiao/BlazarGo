#ifndef CONVERT_H
#define CONVERT_H
#include <string>

class convey{
public:
    void SetCurrentPercent(int s){
        currentPercent = s;
    }
    int GetCurrentPercent(){
        return currentPercent;
    }
    void SetInformation(std::string s){
        information = s;
    }
    std::string GetInformation(){
        return information;
    }

private:
    int currentPercent = 0;
    std::string information = "";
};

#endif // CONVERT_H
