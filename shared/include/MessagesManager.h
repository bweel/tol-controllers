#ifndef shared_MessagesManager_h
#define shared_MessagesManager_h


class MessagesManager {
public:
    
    static std::string get(std::string message, std::string field)
    {
        std::string start = "*" + field;
        std::string end = field + "*";
        std::string value = message.substr(message.find(start)+start.length(), message.find(end)-(message.find(start)+start.length()));
        return value;
    }
    
    static std::string add(std::string message, std::string field, std::string value)
    {
        std::string start = "*" + field;
        std::string end = field + "*";
        std::string newMessage = message + start + value + end;
        return newMessage;
    }
};

#endif
