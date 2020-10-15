//
// Created by Alaric on 2020/10/15.
//

#ifndef HUST_LAB_NETWORK_LOGGER_H
#define HUST_LAB_NETWORK_LOGGER_H

#include <ostream>
#include <list>
class Logger
{
    std::list<std::ostream*> ostreams;
public:
    Logger(const std::initializer_list<std::ostream*>& outstreams);
    void info(const std::string& info);
    void warn(const std::string& warn);
    void error(const std::string& error);
    void log(const std::string& clazz, const std::string& log_info);
};



#endif //HUST_LAB_NETWORK_LOGGER_H
