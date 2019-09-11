//#############################################################################
//
//  test_2WL_unwrapping.cpp
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
				std::cout << std::fixed << std::setprecision(1) << value << ", ";			
			}	
		}
		std::cout << std::endl;
	}
}

int main()
{
	std::string 			file_path 		= "../../Dataset/";
	std::string 			pattern_prefix 	= file_path + "2WL/MFPS";
	std::string 			output_filename = file_path + "output/unwrapped_phase_2FQ_vertical.bmp";
	double					highFrequency 	= 100;
	double 					lowFrequency 	= 99;
	cv::Mat 				pattern;
	std::vector<cv::Mat>	highPatterns;
	std::vector<cv::Mat> 	lowPatterns;
	cv::Mat 				unwrapped_phase;
	cv::Mat 				show_phase;

	highPatterns.reserve(4);
	lowPatterns.reserve(4);

	for (int j = 1; j <= 4; ++j )
	{
		pattern = cv::imread(pattern_prefix+std::to_string(j)+".bmp", cv::IMREAD_GRAYSCALE);
		lowPatterns.push_back(pattern);
	}

	for (int i = 5; i <= 8; ++i)
	{
		pattern = cv::imread(pattern_prefix+std::to_string(i)+".bmp", cv::IMREAD_GRAYSCALE);
		highPatterns.push_back(pattern);
	}

	if (mypu::apply2FQ(	highPatterns,
						highFrequency,
						lowPatterns,
						lowFrequency,
						unwrapped_phase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: mypu::apply2FQ()" << std::endl;
		return EXIT_FAILURE;
	}
	//showMapSample<double>(unwrapped_phase);
	cv::normalize(unwrapped_phase, show_phase, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::imwrite(output_filename, show_phase);

	output_filename = file_path + "output/unwrapped_phase_2FQ_horizontal.bmp";
	highPatterns.clear();
	lowPatterns.clear();
	highFrequency 	= 49;
	lowFrequency 	= 48;
	
	for (int j = 9; j <= 12; ++j )
	{
		pattern = cv::imread(pattern_prefix+std::to_string(j)+".bmp", cv::IMREAD_GRAYSCALE);
		lowPatterns.push_back(pattern);
	}

	for (int i = 13; i <= 16; ++i)
	{
		pattern = cv::imread(pattern_prefix+std::to_string(i)+".bmp", cv::IMREAD_GRAYSCALE);
		highPatterns.push_back(pattern);
	}

	if (mypu::apply2FQ(	highPatterns,
						highFrequency,
						lowPatterns,
						lowFrequency,
						unwrapped_phase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: mypu::apply2FQ()" << std::endl;
		return EXIT_FAILURE;
	}	
	//showMapSample<double>(unwrapped_phase);
	cv::normalize(unwrapped_phase, show_phase, 0, 255, cv::NORM_MINMAX, CV_8UC1);
	cv::imwrite(output_filename, show_phase);

	return EXIT_SUCCESS;
}
