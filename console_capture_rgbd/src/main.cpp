#include <nuitrack/Nuitrack.h>

#include <iomanip>
#include <iostream>

#include <time.h>
#include <sys/time.h>

using namespace tdv::nuitrack;

time_t timer;
struct timeval myTime;
struct tm *t_st;

int _width = 640;
int _height= 480;

void showHelpInfo()
{
  std::cout << "Usage: nuitrack_console_sample [path/to/nuitrack.config]" << std::endl;
}

void name_file(char *t_name, int mode)
{

	// Create all required Nuitrack modules
  gettimeofday(&myTime,NULL);
  t_st=localtime(&myTime.tv_sec);

  time(&timer);
  t_st=localtime(&timer);

  sprintf(t_name,"../rgbd/%d-%d%02d%02d-%02d-%02d-%02d-%06d.csv",
    mode,
    t_st->tm_year+1900,
    t_st->tm_mon+1,
    t_st->tm_mday,
    t_st->tm_hour,
    t_st->tm_min,
    t_st->tm_sec,
    myTime.tv_usec);

}

void getColor(RGBFrame::Ptr frame)
{
  const tdv::nuitrack::Color3* colorPtr = frame->getData();

  float wStep = (float)_width / frame->getCols();
  float hStep = (float)_height / frame->getRows();

  char r_name[50];
//  char g_name[50];
//  char b_name[50];
  FILE *fp_r;
//  FILE *fp_g;
//  FILE *fp_b;

  name_file(r_name,0);
  std::cout<<r_name<<"\n";
//  name_file(g_name,1);
//  name_file(b_name,2);

  float nextVerticalBorder = hStep;

  if(
    (fp_r = fopen(r_name, "a")) != NULL){

/*
    (fp_r = fopen(r_name, "a")) != NULL&&
    (fp_g = fopen(g_name, "a")) != NULL&&
    (fp_b = fopen(b_name, "a")) != NULL){
*/

    for (size_t i = 0; i < _height; ++i)
    {
      if (i == (int)nextVerticalBorder)
      {
        nextVerticalBorder += hStep;
        colorPtr += frame->getCols();
        //depthPtr += frame->getCols();
      }

      int col = 0;
      float nextHorizontalBorder = wStep;
      //uint16_t depthValue = *depthPtr >> 5;

      //for (size_t j = 0; j < _width; ++j, texturePtr += 3)
      for (size_t j = 0; j < _width; ++j)
      {
        if (j == (int)nextHorizontalBorder)
        {
          ++col;
          nextHorizontalBorder += wStep;
        }

        int r = (int)(colorPtr + col)->red;
        int g = (int)(colorPtr + col)->green;
        int b = (int)(colorPtr + col)->blue;

        fprintf(fp_r,"%d,%d,%d",r,g,b);
//        fprintf(fp_r,"%d",r);
 //       fprintf(fp_g,"%d",g);
 //       fprintf(fp_b,"%d",b);

        if(j!=_width-1){
          fprintf(fp_r,",");
          //fprintf(fp_g,",");
          //fprintf(fp_b,",");
        }
      }
      fprintf(fp_r,"\n");
      //fprintf(fp_g,"\n");
      //fprintf(fp_b,"\n");
    }
  }
  fclose(fp_r);
  //fclose(fp_g);
  //fclose(fp_b);
}

void getDepth(DepthFrame::Ptr frame)
{
  if(!frame)
  {
    // No hand data
    std::cout << "No hand data" << std::endl;
    return;
  }

	float wStep = (float)_width / frame->getCols();
	float hStep = (float)_height / frame->getRows();

	const uint16_t* depthPtr = frame->getData();
	float nextVerticalBorder = hStep;

  char d_name[50];// = "depth_sample.csv";
  FILE *fp_d;

  int mode = 3;

  name_file(d_name,mode);

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
    //auto handTracker = HandTracker::create();
    auto depthSensor = DepthSensor::create();
    auto colorSensor = ColorSensor::create();

    // Connect onHandUpdate callback to receive hand tracking data
    //handTracker->connectOnUpdate(onHandUpdate);
    //depthSensor->connectOnNewFrame(getDepth);
    colorSensor->connectOnNewFrame(getColor);

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
