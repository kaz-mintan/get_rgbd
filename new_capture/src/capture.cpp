#include <nuitrack/Nuitrack.h>

using namespace tdv::nuitrack;

void showHelpInfo()
{
  std::cout << "Usage: nuitrack_console_sample [path/to/nuitrack.config]" << std::endl;
}

void *getColor(RGBFrame::Ptr frame, void *gc_p)
{
  uint8_t* bufPtr = gc_p;
  const tdv::nuitrack::Color3* colorPtr = frame->getData();

  float wStep = (float)_width / frame->getCols();
  float hStep = (float)_height / frame->getRows();

  float nextVerticalBorder = hStep;

  for (size_t i = 0; i < _height; ++i)
  {
    if (i == (int)nextVerticalBorder)
    {
      nextVerticalBorder += hStep;
      colorPtr += frame->getCols();
    }

    int col = 0;
    float nextHorizontalBorder = wStep;

    for (size_t j = 0; j < _width; ++j, bufPtr += 3)
    {
      if (j == (int)nextHorizontalBorder)
      {
        ++col;
        nextHorizontalBorder += wStep;
      }

      int r = (int)(colorPtr + col)->red;
      int g = (int)(colorPtr + col)->green;
      int b = (int)(colorPtr + col)->blue;

      bufPtr[0] = r;
      bufPtr[1] = g;
      bufPtr[2] = b;

    }
  }
}

void *getDepth(DepthFrame::Ptr frame, void *gd_p)
{
  if(!frame)
  {
    // No hand data
    std::cout << "No hand data" << std::endl;
    return;
  }

  float wStep = (float)_width / frame->getCols();
  float hStep = (float)_height / frame->getRows();

  uint8_t* bufPtr = gd_p;
  const uint16_t* depthPtr = frame->getData();
  float nextVerticalBorder = hStep;

  int mode = 3;

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
      
      for (size_t j = 0; j < _width; ++j, bufPtr +=1)//add bufptr +3
      {
        if (j == (int)nextHorizontalBorder)
        {
          ++col;
          nextHorizontalBorder += wStep;
          depthValue = *(depthPtr + col) >> 5;
        }
        
        bufPtr[0] = depthValue;
      }
  }
}
