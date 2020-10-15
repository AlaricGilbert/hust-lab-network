#include "string_utils.h"
#include <algorithm>
#include <ctime>
std::string& trim(std::string& s, const std::string &d)
{

    if (s.empty())
        return s;
    s.erase(0, s.find_first_not_of(d));
    s.erase(s.find_last_not_of(d) + 1);
    return s;
}

std::string& upper(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), toupper);
    return s;
}
std::string& lower(std::string& s) {

    std::transform(s.begin(), s.end(), s.begin(), tolower);
    return s;
}
std::string get_time_string() {
    time_t now = time(0);
    tm tm;
    char stime[128]{ 0 };
    gmtime_s(&tm, &now);
    asctime_s(stime, &tm);
    std::string time_str(stime);
    return trim(time_str, "\n");
}