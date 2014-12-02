//
//  MessagesHandler.h
//  RoombotController
//
//  Created by Berend Weel on 21/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef RoombotController_MessagesHandler_h
#define RoombotController_MessagesHandler_h

#include <webots/Robot.hpp>
using namespace webots;

class MessageHandler {
public:
    MessageHandler(Emitter *em, Receiver *recv) :
        emitter(em),
        receiver(recv)
    {
    }
    
    void send(std::string message){
        emitter->send(message.c_str(), message.length()+1);
    }
    
    std::string receive() {
        std::string message((char*)receiver->getData());
        return message;
    }
    
    void next() {
        receiver->nextPacket();
    }
    
    bool hasMessage() {
        return receiver->getQueueLength() > 0;
    }
    
    void setChannel(int channel) {
        emitter->setChannel(channel);
        receiver->setChannel(channel);
    }
    
    void setEmitterRange(double range) {
        emitter->setRange(range);
    }

    void setEmitterChannel(int channel) {
        emitter->setChannel(channel);
    }
    
    void setReceiverChannel(int channel) {
        receiver->setChannel(channel);
    }
    
    Emitter *getEmitter() {
        return emitter;
    }
    
    Receiver *getReceiver() {
        return receiver;
    }
    
private:
    Emitter *emitter;
    Receiver *receiver;
};

#endif
