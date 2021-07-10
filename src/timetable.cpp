
#include "timetable.h"


Timetable::Timetable(String start, String end, float value) {
    this->m_start = this->parse_hour(start);
    this->m_end = this->parse_hour(end);
    // if 00:00 convert to 24:00
    if(this->m_end == 0) this->m_end = 24 * 60;
    this->m_value = value;
}

float Timetable::get_value() { return m_value; };

bool Timetable::is_in_interval() {
    tm time_info;
    getLocalTime(&time_info);
    int minutes_now = time_info.tm_hour * 60 + time_info.tm_min;
    if (m_start < minutes_now && m_end > minutes_now) {
        return true;
    }
    return false;
}


int Timetable::parse_hour(String hour) {
    return (char_to_int(hour[0]) * 10 + char_to_int(hour[1])) * 60 + char_to_int(hour[3]) * 10 + char_to_int(hour[4]);
}

inline int Timetable::char_to_int(char c) {
    return c - '0';
}
