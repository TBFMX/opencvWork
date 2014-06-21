/*****************************************************************************
*   SimpleVisualizationController.h
*   Example_MarkerBasedAR
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch2 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/
#include "GeometryTypes.h"
#include "Marker.h"
#include "MarkerDetector.h"
#include "CameraCalibration.h"
#include <vector>
#include <iostream>
//~ #include "BGRAVideoFrame.h"

void setTransformationList(const std::vector<Transformation>& transformations);
void drawFrame();
