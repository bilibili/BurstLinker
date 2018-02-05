//
// Created by succlz123 on 17-9-30.
//

#ifndef BURSTLINKER_GIFLOGGER_H
#define BURSTLINKER_GIFLOGGER_H

#include <cstdint>
#include <string>
#include <sstream>

namespace blk {

    class Logger {

    public:

        static void log(bool show, std::string str);

        template<typename T>
        static std::string toString(T value) {
            std::ostringstream os;
            os << value;
            return os.str();
        }
    };

}

#endif //BURSTLINKER_GIFLOGGER_H
