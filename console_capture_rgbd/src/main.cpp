#include <nuitrack/Nuitrack.h>

#include <iomanip>
#include <iostream>

#include <time.h>
#include <sys/time.h>

using namespace tdv::nuitrack;

time_t timer;
struct timeval myTime;
struct tm *t_st;


void showHelpInfo()
{
  std::cout << "Usage: nuitrack_console_sample [path/to/nuitrack.config]" << std::endl;
}

void getDepth(DepthFrame::Ptr frame)
{
  if(!frame)
  {
    // No hand data
    std::cout << "No hand data" << std::endl;
    return;
  }

  int _width = 640;
  int _height= 480;

	float wStep = (float)_width / frame->getCols();
	float hStep = (float)_height / frame->getRows();

	const uint16_t* depthPtr = frame->getData();
	float nextVerticalBorder = hStep;

  char d_name[50];// = "depth_sample.csv";
  FILE *fp_d;

	// Create all required Nuitrack modules
  gettimeofday(&myTime,NULL);
  t_st=localtime(&myTime.tv_sec);

  time(&timer);
  t_st=localtime(&timer);

  sprintf(d_name,"rgbd/d-%d%02d%02d-%02d-%02d-%02d-%06d.csv",
    t_st->tm_year+1900,
    t_st->tm_mon+1,
    t_st->tm_mday,
    t_st->tm_hour,
    t_st->tm_min,
    t_st->tm_sec,
    myTime.tv_usec);
  std::cout<<d_name<<"\n";

  if((fp_d = fopen(d_name, "a")) != NULL)
  {
    for (size_t i = 0; i < _height; ++i)
    {
      if (i == (int)nextVerticalBorder)
      {
        nextVerticalBorder += hStep;
        depthPtr += frame->getCols();
      }
      
      int col = 0;
      float nextHorizontalBorder = wStep;
      uint16_t depthValue = *depthPtr >> 5;
      
      for (size_t j = 0; j < _width; ++j)
      {
        if (j == (int)nextHorizontalBorder)
        {
          ++col;
          nextHorizontalBorder += wStep;
          depthValue = *(depthPtr + col) >> 5;
        }
        
        fprintf(fp_d,"%d",depthValue);
        if(j!=_width-1)  fprintf(fp_d,",");//test_d<<",";
      }
      //test_d<<std::endl;
      fprintf(fp_d,"\n");
    }
  }
  fclose(fp_d);
}

// Callback for the hand data update event
void onHandUpdate(HandTrackerData::Ptr handData)
{
    if (!handData)
    {
        // No hand data
        std::cout << "No hand data" << std::endl;
        return;
    }

    auto userHands = handData->getUsersHands();
    if (userHands.empty())
    {
        // No user hands
        return;
    }

    auto rightHand = userHands[0].rightHand;
    if (!rightHand)
    {
        // No right hand
        std::cout << "Right hand of the first user is not found" << std::endl;
        return;
    }

    /*
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Right hand position: "
                 "x = " << rightHand->xReal << ", "
                 "y = " << rightHand->yReal << ", "
                 "z = " << rightHand->zReal << std::endl;
    */
}

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
    
    std::cout<<"out\n";
    // Create HandTracker module, other required modules will be
    // created automatically
    auto handTracker = HandTracker::create();
    auto depthSensor = DepthSensor::create();

    // Connect onHandUpdate callback to receive hand tracking data
    handTracker->connectOnUpdate(onHandUpdate);
    //depthSensor->connectOnNewFrame(std::bind(&NuitrackGLSample::onNewDepthFrame, this, std::placeholders::_1));
    depthSensor->connectOnNewFrame(getDepth);
    //_outputMode = _depthSensor->getOutputMode();

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
            //Nuitrack::waitUpdate(handTracker);

            Nuitrack::waitUpdate(depthSensor);
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
