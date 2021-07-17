#ifndef HANDLER_H
#define HANDLER_H

#include "config.h"
#include "state.h"

class Handler {
public:
    Handler(Config* config, State* state);
    virtual ~Handler();
    virtual void run(float deltatime);

protected:
    HANDLER_TYPE type;

    Config* m_config;
    State* m_state;
};

class Handler_test : public Handler {
public:
    Handler_test(Config* config, State* state);
    ~Handler_test();
    void run(float deltatime) override;

};

class Handler_basic : public Handler {
public:
    Handler_basic(Config* config, State* state);
    ~Handler_basic();
    void run(float deltatime) override;

};

class Handler_pid : public Handler {
public:
    Handler_pid(Config* config, State* state);
    ~Handler_pid();
    void run(float deltatime) override;
private:
    float previous_error = 0;
    float integral = 0;

    const float maxIntegralError = 5.0f;

    float Kp = 10.0f;
    float Ki = 0.01f;
    float Kd = 1000.0f;
};


#endif
