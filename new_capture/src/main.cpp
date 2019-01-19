#include <nuitrack/Nuitrack.h>

#include <iomanip>
#include <iostream>

#include <pthread.h>

#include <time.h>
#include <sys/time.h>

#include "capture.h"
#include "save.h"

#define BUF_SIZE (640*480*3) //huh??
char cBuf[BUF_SIZE]
char dBuf[BUF_SIZE] //should separate buffers for depth and color?
unsigned int gIndex = 0;

time_t timer;
struct timeval myTime;
struct tm *t_st;

int _width = 640;
int _height= 480;

int main(int argc, char* argv[])
{
    showHelpInfo();

    std::string configPath = "";
    if (argc > 1)
        configPath = argv[1];

    // Initialize Nuitrack
    std::cout<<"out\n";
    try
    {
        Nuitrack::init(configPath);
    }
    catch (const Exception& e)
    {
        std::cerr << "Can not initialize Nuitrack (ExceptionType: " << e.type() << ")" << std::endl;
        return EXIT_FAILURE;
    }


    auto depthSensor = DepthSensor::create();
    auto colorSensor = ColorSensor::create();
    pthread_t pthread_d;
    pthread_t pthread_c;
    int mode_d = 0;
    int mode_c = 1;

    depthSensor->connectOnNewFrame(getDepth);
    colorSensor->connectOnNewFrame(getColor);
    pthread_create(&pthread_d, NULL, &save_log, mode_d)
    pthread_create(&pthread_c, NULL, &save_log, mode_c)

    // Start Nuitrack
    try
    {
        Nuitrack::run();
    }
    catch (const Exception& e)
    {
        std::cerr << "Can not start Nuitrack (ExceptionType: " << e.type() << ")" << std::endl;
        return EXIT_FAILURE;
    }

    int errorCode = EXIT_SUCCESS;
    while (true)
    {
        try
        {
            // Wait for new hand tracking data
            Nuitrack::waitUpdate(depthSensor);
            Nuitrack::waitUpdate(colorSensor);
        }
        catch (LicenseNotAcquiredException& e)
        {
            std::cerr << "LicenseNotAcquired exception (ExceptionType: " << e.type() << ")" << std::endl;
            errorCode = EXIT_FAILURE;
            break;
        }
        catch (const Exception& e)
        {
            std::cerr << "Nuitrack update failed (ExceptionType: " << e.type() << ")" << std::endl;
            errorCode = EXIT_FAILURE;
        }
    }

    // Release Nuitrack
    try
    {
        Nuitrack::release();
    }
    catch (const Exception& e)
    {
        std::cerr << "Nuitrack release failed (ExceptionType: " << e.type() << ")" << std::endl;
        errorCode = EXIT_FAILURE;
    }

    return errorCode;
}
