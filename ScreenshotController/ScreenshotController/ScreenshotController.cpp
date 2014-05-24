#include "ScreenshotController.h"

ScreenshotController::ScreenshotController() : Supervisor()
{
    receiver = getReceiver(RECEIVER_NAME);
    receiver->setChannel(SCREENSHOT_CHANNEL);
}

ScreenshotController::~ScreenshotController() {
    
}

void ScreenshotController::run()
{
    receiver->enable(TIME_STEP);
    
    // wait unitil EnvironmentModifierController sais the environment is ok
    bool environmentOk = false;
    while (step(TIME_STEP) != -1 && !environmentOk)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            if (message.substr(0,14).compare("ENVIRONMENT_OK") == 0)
            {
                environmentOk = true;
                simulationDateAndTime = message.substr(14,message.length());
            }
            receiver->nextPacket();
        }
    }
    
    boost::filesystem::path dirpath(getProjectPath());
    dirpath /= "Results";
    dirpath /= simulationDateAndTime;
    dirpath /= "screenshots";
    
    if(!boost::filesystem::exists(dirpath)){
        boost::filesystem::create_directories(dirpath);
    }
    
    unsigned int counter = 1;
    int numberOfDigits = 15;
    
    while (step(TIME_STEP) != -1) {
        double now = getTime();
        if(lastScreenshot < 0 || now - lastScreenshot > SCREENSHOT_INTERVAL)
        {
            boost::filesystem::path filepath = dirpath;
            
            int actualDigits = 1;
            int tmp = counter;
            while (tmp/10 > 0)
            {
                actualDigits++;
                tmp = tmp / 10;
            }
            int zeroDigits = numberOfDigits - actualDigits;
            std::string name = "";
            for (int i = 0; i < zeroDigits; i++)
            {
                name = name + "0";
            }
            name = name + std::to_string(counter);
            
            filepath /= name + ".jpg";
//            filepath /= boost::lexical_cast<std::string>(now) + ".jpg";
            
//            std::cout << "Screenshot taken." << std::endl;
            exportImage(filepath.string(), 100);
            lastScreenshot = now;
            
            counter++;
        }
    }
}