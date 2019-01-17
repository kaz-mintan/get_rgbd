#include "NuitrackGLSample.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <string>

using namespace tdv::nuitrack;

time_t timer;
struct timeval myTime;
struct tm *t_st;


NuitrackGLSample::NuitrackGLSample() :
	_textureID(0),
	_textureBuffer(0),
	_width(640),
	_height(480),
	_viewMode(RGB_MODE),
//DEPTH_SEGMENT_MODE
//RGB_MODE
	_modesNumber(1),
	_isInitialized(false)
{}

NuitrackGLSample::~NuitrackGLSample()
{
	try
	{
		Nuitrack::release();
	}
	catch (const Exception& e)
	{
		// Do nothing
	}
}

void NuitrackGLSample::init(const std::string& config)
{
	// Initialize Nuitrack first, then create Nuitrack modules
	try
	{
		Nuitrack::init(config);
	}
	catch (const Exception& e)
	{
		std::cerr << "Can not initialize Nuitrack (ExceptionType: " << e.type() << ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	
	// Create all required Nuitrack modules
  gettimeofday(&myTime,NULL);
  t_st=localtime(&myTime.tv_sec);

	_depthSensor = DepthSensor::create();
	// Bind to event new frame
	_depthSensor->connectOnNewFrame(std::bind(&NuitrackGLSample::onNewDepthFrame, this, std::placeholders::_1));

	_colorSensor = ColorSensor::create();
	// Bind to event new frame
	_colorSensor->connectOnNewFrame(std::bind(&NuitrackGLSample::onNewRGBFrame, this, std::placeholders::_1));

	_outputMode = _depthSensor->getOutputMode();
	OutputMode colorOutputMode = _colorSensor->getOutputMode();
	if (colorOutputMode.xres > _outputMode.xres)
		_outputMode.xres = colorOutputMode.xres;
	if (colorOutputMode.yres > _outputMode.yres)
		_outputMode.yres = colorOutputMode.yres;

	_width = _outputMode.xres;
	_height = _outputMode.yres;

	_userTracker = UserTracker::create();
	// Bind to event update user tracker
	_userTracker->connectOnUpdate(std::bind(&NuitrackGLSample::onUserUpdate, this, std::placeholders::_1));

	_skeletonTracker = SkeletonTracker::create();
	// Bind to event update skeleton tracker
//	_skeletonTracker->connectOnUpdate(std::bind(&NuitrackGLSample::onSkeletonUpdate, this, std::placeholders::_1));

}

bool NuitrackGLSample::update()
{
	if (!_isInitialized)
	{
		// Create texture by DepthSensor output mode
		initTexture(_width, _height);

		// When Nuitrack modules are created, we need to call Nuitrack::run() to start processing all modules
		try
		{
			Nuitrack::run();
		}
		catch (const Exception& e)
		{
			std::cerr << "Can not start Nuitrack (ExceptionType: " << e.type() << ")" << std::endl;
			release();
			exit(EXIT_FAILURE);
		}
		_isInitialized = true;
	}
	try
	{
		// Wait and update Nuitrack data
		Nuitrack::waitUpdate(_skeletonTracker);
		
		renderTexture();
	}
	catch (const LicenseNotAcquiredException& e)
	{
		// Update failed, negative result
		std::cerr << "LicenseNotAcquired exception (ExceptionType: " << e.type() << ")" << std::endl;
		return false;
	}
	catch (const Exception& e)
	{
		// Update failed, negative result
		std::cerr << "Nuitrack update failed (ExceptionType: " << e.type() << ")" << std::endl;
		return false;
	}
	
	return true;
}

void NuitrackGLSample::release()
{
	// Release Nuitrack and remove all modules
	try
	{
		Nuitrack::release();
	}
	catch (const Exception& e)
	{
		std::cerr << "Nuitrack release failed (ExceptionType: " << e.type() << ")" << std::endl;
	}

	_isInitialized = false;
	// Free texture buffer
	if (_textureBuffer)
	{
		delete[] _textureBuffer;
		_textureBuffer = 0;
	}
}

// Copy depth frame data, received from Nuitrack, to texture to visualize
void NuitrackGLSample::onNewDepthFrame(DepthFrame::Ptr frame)
{

	uint8_t* texturePtr = _textureBuffer;
	const uint16_t* depthPtr = frame->getData();
	
	float wStep = (float)_width / frame->getCols();
	float hStep = (float)_height / frame->getRows();
	
	float nextVerticalBorder = hStep;

  //test to save depth data
  char d_name[50];
  FILE *fp_d;

  sprintf(d_name,"rgbd/d-%d%02d%02d-%02d-%02d-%02d-%06d.csv",
    t_st->tm_year+1900,
    t_st->tm_mon+1,
    t_st->tm_mday,
    t_st->tm_hour,
    t_st->tm_min,
    t_st->tm_sec,
    myTime.tv_usec);
  //test_d.open(d_name);
  if((fp_d = fopen(d_name, "a")) != NULL){

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
      
      for (size_t j = 0; j < _width; ++j, texturePtr += 3)
      {
        if (j == (int)nextHorizontalBorder)
        {
          ++col;
          nextHorizontalBorder += wStep;
          depthValue = *(depthPtr + col) >> 5;
        }
        
        texturePtr[0] = depthValue;
        texturePtr[1] = depthValue;
        texturePtr[2] = depthValue;
        //test_d<<depthValue;
        fprintf(fp_d,"%d",depthValue);
        if(j!=_width-1)  fprintf(fp_d,",");//test_d<<",";
      }
      //test_d<<std::endl;
      fprintf(fp_d,"\n");
    }
  }
  fclose(fp_d);
}

// Copy color frame data, received from Nuitrack, to texture to visualize
void NuitrackGLSample::onNewRGBFrame(RGBFrame::Ptr frame)
{

	const tdv::nuitrack::Color3* colorPtr = frame->getData();

	float wStep = (float)_width / frame->getCols();
	float hStep = (float)_height / frame->getRows();

  time(&timer);
  t_st=localtime(&timer);

  char r_name[50];
  char g_name[50];
  char b_name[50];
  FILE *fp_r;
  FILE *fp_g;
  FILE *fp_b;

  sprintf(r_name,"rgbd/r-%d%02d%02d-%02d-%02d-%02d-%06d.csv",
    t_st->tm_year+1900,
    t_st->tm_mon+1,
    t_st->tm_mday,
    t_st->tm_hour,
    t_st->tm_min,
    t_st->tm_sec,
    myTime.tv_usec);

  sprintf(g_name,"rgbd/g-%d%02d%02d-%02d-%02d-%02d-%06d.csv",
    t_st->tm_year+1900,
    t_st->tm_mon+1,
    t_st->tm_mday,
    t_st->tm_hour,
    t_st->tm_min,
    t_st->tm_sec,
    myTime.tv_usec);

  sprintf(b_name,"rgbd/b-%d%02d%02d-%02d-%02d-%02d-%06d.csv",
    t_st->tm_year+1900,
    t_st->tm_mon+1,
    t_st->tm_mday,
    t_st->tm_hour,
    t_st->tm_min,
    t_st->tm_sec,
    myTime.tv_usec);

  //test to save rgb data

	float nextVerticalBorder = hStep;

  if(
    (fp_r = fopen(r_name, "a")) != NULL&&
    (fp_g = fopen(g_name, "a")) != NULL&&
    (fp_b = fopen(b_name, "a")) != NULL){
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
          //depthValue = *(depthPtr+col)>>5;
        }

        int r = (int)(colorPtr + col)->red;
        int g = (int)(colorPtr + col)->green;
        int b = (int)(colorPtr + col)->blue;

        fprintf(fp_r,"%d",r);
        fprintf(fp_g,"%d",g);
        fprintf(fp_b,"%d",b);

        if(j!=_width-1){
          fprintf(fp_r,",");
          fprintf(fp_g,",");
          fprintf(fp_b,",");
        }
      }
      fprintf(fp_r,"\n");
      fprintf(fp_g,"\n");
      fprintf(fp_b,"\n");
    }
  }
  fclose(fp_r);
  fclose(fp_g);
  fclose(fp_b);
}
// Colorize user segments using Nuitrack User Tracker data
void NuitrackGLSample::onUserUpdate(UserFrame::Ptr frame)
{

}

// Prepare visualization of skeletons, received from Nuitrack
void NuitrackGLSample::onSkeletonUpdate(SkeletonData::Ptr userSkeletons)
{

}

// Prepare visualization of tracked hands
void NuitrackGLSample::onHandUpdate(HandTrackerData::Ptr handData)
{
}

// Display information about gestures in the console
void NuitrackGLSample::onNewGesture(GestureData::Ptr gestureData)
{
}

// Helper function to draw a skeleton bone
void NuitrackGLSample::drawBone(const Joint& j1, const Joint& j2)
{

}

// Helper function to draw skeleton from Nuitrack data
void NuitrackGLSample::drawSkeleton(const std::vector<Joint>& joints)
{

}

// Render prepared background texture
void NuitrackGLSample::renderTexture()
{
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
	
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, _textureBuffer);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer(2, GL_FLOAT, 0, _vertexes);
	glTexCoordPointer(2, GL_FLOAT, 0, _textureCoords);
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glDisable(GL_TEXTURE_2D);
}

int NuitrackGLSample::power2(int n)
{
	unsigned int m = 2;
	while (m < n)
		m <<= 1;
	
	return m;
}

// Visualize bones, joints and hand positions
void NuitrackGLSample::renderLines()
{

}

void NuitrackGLSample::initTexture(int width, int height)
{
	glGenTextures(1, &_textureID);
	
	width = power2(width);
	height = power2(height);
	
	if (_textureBuffer != 0)
		delete[] _textureBuffer;
	
	_textureBuffer = new uint8_t[width * height * 3];
//	memset(_textureBuffer, 0, sizeof(uint8_t) * width * height * 3);
	
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	// Set texture coordinates [0, 1] and vertexes position
	{
		_textureCoords[0] = (float) _width / width;
		_textureCoords[1] = (float) _height / height;
		_textureCoords[2] = (float) _width / width;
		_textureCoords[3] = 0.0;
		_textureCoords[4] = 0.0;
		_textureCoords[5] = 0.0;
		_textureCoords[6] = 0.0;
		_textureCoords[7] = (float) _height / height;
		
		_vertexes[0] = _width;
		_vertexes[1] = _height;
		_vertexes[2] = _width;
		_vertexes[3] = 0.0;
		_vertexes[4] = 0.0;
		_vertexes[5] = 0.0;
		_vertexes[6] = 0.0;
		_vertexes[7] = _height;
	}
}
