#ifndef TIMETABLE_H
#define TIMETABLE_H

#include <Arduino.h>
#include <sys/time.h>

class Timetable {
public:
    Timetable(String start, String end, float value);
    float get_value();
    bool is_in_interval();

private:
    // minute of the day
    int m_start;
    int m_end;
    float m_value;

    int parse_hour(String hour);
    inline int char_to_int(char c);
};


#endif
