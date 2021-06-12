#include "status.h"

Status::Status() {}

void Status::init(Config* config) { m_config = config; }

Config* Status::getConfig() { return m_config; }

void Status::update(void) {
  this->temperatures.clear();
  for (auto sensor : m_config->sensors) {
    float t = sensor->getValue();
    this->temperatures.push_back(t);
  }
  if (this->temperatures.size() >= 2) {
    this->avg_temperature = 0;
    for (auto t : this->temperatures) {
      this->avg_temperature += t;
    }
    this->avg_temperature /= this->temperatures.size();
  }
  else {
    for (auto t : this->temperatures) {
      this->avg_temperature = t;
    }
  }
}

DISPLAY_STATUS Status::getDisplayStatus() { return m_display_status; }

void Status::nextDisplayStatus() {
  if (m_display_status == DISPLAY_STATUS::OFF) {
    m_display_status = (DISPLAY_STATUS)0;
  }
  else {
    m_display_status = (DISPLAY_STATUS)(((int)m_display_status) + 1);
  }
}
