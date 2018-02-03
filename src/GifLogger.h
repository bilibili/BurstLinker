//
// Created by succlz123 on 17-9-30.
//

#ifndef BURSTLINKER_GIFLOGGER_H
#define BURSTLINKER_GIFLOGGER_H


#include <cstdint>
#include <string>
#include <sstream>

using namespace std;

class GifLogger {

public:

    static void log(bool show, string str);

    template<typename T>
    static string toString(T value) {
        ostringstream os;
        os << value;
        return os.str();
    }
};


#endif //BURSTLINKER_GIFLOGGER_H
