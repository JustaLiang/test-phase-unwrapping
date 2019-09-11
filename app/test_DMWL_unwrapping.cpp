//#############################################################################
//
//  test_DMWL_unwrapping.cpp
//
//#############################################################################
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "unwrap.h"

//#############################################################################
//
//	showMapSample(): show some value sampled from map
//
//#############################################################################
template <class T> static void showMapSample(const cv::Mat 	&map)
{
	int 	x0;
	int 	y0;

	std::cout << std::endl << "map size: " << map.cols << "x" << map.rows << std::endl;

	std::cout << "x0: ";
	std::cin  >> x0;
	std::cout << "y0: ";
	std::cin  >> y0;
	cv::Rect rect(x0,y0,20,20);

	int yUp		= std::max<int>(rect.y, 0);		
	int yDown 	= std::min<int>(rect.y + rect.height, map.rows);
	int xLeft	= std::max<int>(rect.x, 0);
	int xRight 	= std::min<int>(rect.x + rect.width, map.cols);

	std::cout << "x: " << xLeft << " ~ " << xRight - 1 <<
		 "  " << "y: " << yUp << " ~ " << yDown - 1 << std::endl << std::endl;

	T value;
	for (int y = yUp; y < yDown; ++y)
	{
		for (int x = xLeft; x < xRight; ++x)
		{	
			value = (T)map.at<T>(y,x);
			if (value == 0xFFFF)
			{
				std::cout << "  xxxx,";
			}
			else
			{
				std::cout << std::fixed << std::setprecision(2) << value << ", ";			
			}	
		}
		std::cout << std::endl;
	}
}

int main()
{
	const std::string 		file_path 		= "../../Dataset/";
	const std::string 		pattern_prefix 	= file_path + "DMWL/three_phase_1024x512_wavestep_";
	const std::string 		output_filename = file_path + "output/unwrapped_phase_DMWL.bmp";
	const int 				digitalSteps 	= 4;
	std::string 			img_name;
	cv::Mat 				pattern;
	std::vector<cv::Mat>	patternVec;
	cv::Mat 				unwrapped_phase;
	cv::Mat 				show_phase;

	patternVec.reserve(12);

	for (int ws = 0; ws < digitalSteps; ++ws)
	{
		img_name = pattern_prefix+std::to_string(ws)+"_phase_0.bmp";
		pattern = cv::imread(img_name, cv::IMREAD_GRAYSCALE);
		patternVec.push_back(pattern);

		img_name = pattern_prefix+std::to_string(ws)+"_phase_120.bmp";
		pattern = cv::imread(img_name,cv::IMREAD_GRAYSCALE);
		patternVec.push_back(pattern);

		img_name = pattern_prefix+std::to_string(ws)+"_phase_240.bmp";
		pattern = cv::imread(img_name,cv::IMREAD_GRAYSCALE);
		patternVec.push_back(pattern);
	}

	if (mypu::applyDMWL(patternVec,
						digitalSteps,
						unwrapped_phase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: mypu::applyDMWL()" << std::endl;
		return EXIT_FAILURE;
	}

	//showMapSample<double>(unwrapped_phase);

	cv::normalize(unwrapped_phase, show_phase, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::imwrite(output_filename, show_phase);

	return EXIT_SUCCESS;
}
