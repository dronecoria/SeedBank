#include "handler.h"

Handler::Handler(Config* config, State* state) {
    this->m_config = config;
    this->m_state = state;
}
Handler::~Handler() {}


Handler_basic::Handler_basic(Config* config, State* state) : Handler(config, state){}
Handler_basic::~Handler_basic() {}

void Handler_basic::run(float deltatime)
{
    Serial.println("BASIC HANDLER");

    float t_avg = m_state->get_avg_temperature();
    float t_reference = m_config->get_temperature_reference();

    float t_threshold = 0.25f;
    float t_max_diff = 0;
    for (auto s : m_config->sensors) {
        if (s->is_temperature()) {
            float t = s->get_value();
            t_max_diff = max(t_max_diff, abs(t - t_avg));
        }
    }

    if (m_config->cold != nullptr) {
        if ((t_reference + t_threshold) < t_avg) {
            m_config->cold->enable();
        } else {
            m_config->cold->disable();
        }
    }
    if (m_config->heat != nullptr) {
        if ((t_reference - t_threshold) > t_avg) {
            m_config->heat->enable();
        }
        else {
            m_config->heat->disable();
        }
    }
    if (m_config->fan != nullptr) {
        if (t_max_diff > 0.25f) {
            //this->m_config->fan->set_value(t_diff);
            m_config->fan->enable();
        }
        else {
            m_config->fan->disable();
        }
    }
    if (m_config->light != nullptr) {
        m_config->light->set_value(t_max_diff);
    }

    // DEBUG INFO
    Serial.print("Decisor::loop");
    Serial.print(" - Avg Temp: ");
    Serial.print(t_avg);
    Serial.print(" - All: ");
    m_state->print_all_temperatures();

    Serial.print("   REF: ");
    Serial.print(t_reference);

    Serial.print("   MAX DIFF: ");
    Serial.print(t_max_diff);

    Serial.println("");

}

Handler_test::Handler_test(Config* config, State* state) : Handler(config, state) {}
Handler_test::~Handler_test() {}
void Handler_test::run(float deltatime) {}


Handler_pid::Handler_pid(Config* config, State* state) : Handler(config, state) {}
Handler_pid::~Handler_pid() {}
void Handler_pid::run(float deltatime) {
    deltatime /= 1000; //in seconds
    float t_avg = m_state->get_avg_temperature();
    float t_reference = m_config->get_temperature_reference();

    float error = t_reference - t_avg;
    float P = error;
    integral += (error * deltatime);
    // https://www.picuino.com/es/arduprog/control-pid-digital.html#control-anti-windup-integral
    if (abs(error) > maxIntegralError){
        integral = 0;
    }
    float D = (error - previous_error) / deltatime;
    float output = Kp * P + Ki * integral + Kd * D;
    output = output / (5.0f * Kp); // scale for values between 0 and 1
    Serial.println("  error:" + String(error) + "    P error:" + String(previous_error) );

    previous_error = error;


    if (m_config->cold != nullptr) {
        if (output < -0.5f) {
            m_config->cold->enable();
        } else if(m_config->cold->is_active()){
            m_config->cold->disable();
        }
    }

    if (m_config->heat != nullptr) {
        if (output > 0) {
            m_config->heat->set_value(output);
        } else if (m_config->heat->is_active()) {
            m_config->heat->disable();
        }
    }

    if (m_config->fan != nullptr) {
        float t_max_diff = 0;
        for (auto s : m_config->sensors) {
            if (s->is_temperature()) {
                float t = s->get_value();
                t_max_diff = max(t_max_diff, abs(t - t_avg));
            }
        }
        if (t_max_diff > 0.25f) {
            //this->m_config->fan->set_value(t_diff);
            m_config->fan->enable();
        }
        else {
            m_config->fan->disable();
        }
    }

    Serial.println("PID  P:"+String(P)+"   I:"+String(integral)+ "   D:"+String(D)+ "     OUTPUT:"+String(output) + "         dt:"+String(deltatime));
}
