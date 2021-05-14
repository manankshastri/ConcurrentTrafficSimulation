#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive() {
     std::unique_lock<std::mutex> uLock(_muteX);
    _condVariable.wait(uLock, [this] {return !_messages.empty(); });

    T msg = std::move(_messages.back());
    _messages.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg) {
    std::lock_guard<std::mutex> uLock(_muteX);

    _messages.clear();
    _messages.push_back(std::move(msg));
    _condVariable.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() {
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen() {
    while (true) {

        std::cout<<" Waiting for GREEN!! "<<std::endl;
        //TrafficLightPhase light = _queue.receive();

        if (_queue.receive() == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase() {
    return _currentPhase;
}

void TrafficLight::simulate() {
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {

    auto time1 = std::chrono::system_clock::now();

    // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c (answer 2)
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<> dist46(4000.0, 6000.0);

    auto cycleDuration = dist46(rng);

    while (true) {

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time1).count();

        if (diffTime >= cycleDuration) {
            if (getCurrentPhase() == TrafficLightPhase::red) {
                _currentPhase = TrafficLightPhase::green;
                time1 = std::chrono::system_clock::now();
            }
            else {
                _currentPhase = TrafficLightPhase::red;
                time1 = std::chrono::system_clock::now();
            }

            //cycleDuration = dist46(rng);
            _queue.send(std::move(_currentPhase));
        }

    }
}

