#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <string>
#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

template <class T>
class MessageQueue {
public:
    T receive();
    void send(T &&msg);

private:
    std::deque<T> _messages;
    std::condition_variable _condVariable;
    std::mutex _muteX;
};


class TrafficLight : public TrafficObject {
public:
    // constructor / destructor
    TrafficLight();

    // getters / setters
    void waitForGreen();
    void simulate();

    // typical behaviour methods
    enum class TrafficLightPhase {
        red,
        green
    };

    TrafficLightPhase getCurrentPhase();

private:
    // typical behaviour methods
    void cycleThroughPhases();
    TrafficLightPhase _currentPhase;

    MessageQueue<TrafficLightPhase> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif