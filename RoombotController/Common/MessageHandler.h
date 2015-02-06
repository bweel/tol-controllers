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
    MessageHandler() {
        emitter = NULL;
        receiver = NULL;
    }
    
    MessageHandler(Emitter *em, Receiver *recv) :
        emitter(em),
        receiver(recv)
    {
    }
    
    void send(std::string message){
        if(emitter)
            emitter->send(message.c_str(), message.length()+1);
    }
    
    std::string receive() {
        if(receiver) {
            std::string message((char*)receiver->getData());
            return message;
        } else {
            return NULL;
        }
    }
    
    void next() {
        if(receiver) {
            receiver->nextPacket();
        }
    }
    
    bool hasMessage() {
        if(receiver) {
            return receiver->getQueueLength() > 0;
        } else {
            return false;
        }
    }
    
    void setChannel(int channel) {
        if(emitter) {
            emitter->setChannel(channel);
        }
        if(receiver) {
            receiver->setChannel(channel);
        }
    }
    
    void setEmitterRange(double range) {
        if(emitter)
            emitter->setRange(range);
    }

    void setEmitterChannel(int channel) {
        if(emitter)
            emitter->setChannel(channel);
    }
    
    void setReceiverChannel(int channel) {
        if(receiver)
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
