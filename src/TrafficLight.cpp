#include <iostream>
#include <random>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

     std::unique_lock<std::mutex> uLock(_muteX);
    _condVariable.wait(uLock, [this] {return !_messages.empty(); });

    T msg = std::move(_messages.back());
    _messages.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.

    std::lock_guard<std::mutex> uLock(_muteX);

    _messages.clear();
    _messages.push_back(std::move(msg));
    _condVariable.notify_one();

}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.

    while (true) {

        std::cout<<" Waiting for GREEN!! "<<std::endl;
        //TrafficLightPhase light = _queue.receive();

        if (_queue.receive() == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLight::TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 

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

