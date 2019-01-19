#ifndef CAPTURE_H
#define CAPTURE_H

#include <nuitrack/Nuitrack.h>
using namespace tdv::nuitrack;

void showHelpInfo();
void *getColor(RGBFrame::Ptr frame);
void *getDepth(DepthFrame::Ptr frame);

#endif
