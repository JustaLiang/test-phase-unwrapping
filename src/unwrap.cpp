//#############################################################################
//
//	unwrap.cpp
//
//#############################################################################
#include <iostream>
#include <cstdlib>
#include "unwrap.h"

namespace mypu
{

//#############################################################################
//
//  computePhaseMap(): compute phase map given three phase images (0,120,240)
//
//#############################################################################
int computePhaseMap(const cv::Mat	&image_0,
					const cv::Mat 	&image_120,
					const cv::Mat	&image_240,
					cv::Mat 		&wrapped_phase_map)
{
	int 			rows 	= image_0.rows;
	int 			cols 	= image_0.cols;
	const uchar*	img0_ptr;
	const uchar* 	img120_ptr;
	const uchar* 	img240_ptr;
	double* 		phase_ptr;	

	wrapped_phase_map.release();

	if (image_120.rows != rows || image_120.cols != cols ||
		image_240.rows != rows || image_120.cols != cols)
	{
		std::cout << "ERROR: image resolution not match" << std::endl;
		return EXIT_FAILURE;
	}

	wrapped_phase_map.create(rows, cols, CV_64F);

	for (int row = 0; row < rows; ++row)
	{
		img0_ptr 	= image_0.ptr<uchar>(row);
		img120_ptr 	= image_120.ptr<uchar>(row);
		img240_ptr 	= image_240.ptr<uchar>(row);
		phase_ptr 	= wrapped_phase_map.ptr<double>(row);

		for (int col = 0; col < cols; ++col)
		{
			phase_ptr[col] = (std::atan2( 
						std::sqrt(3.0)*(img240_ptr[col] - img120_ptr[col]),
						2.0*img0_ptr[col] - img120_ptr[col] - img240_ptr[col])
						/3.14159265359)+1.0;
		}
	}

	return EXIT_SUCCESS;
}

//#############################################################################
//
//  unwrapPhaseDMWL(): unwrap phase of DMWL patterns
//
//#############################################################################
int unwrapPhaseDMWL(cv::Mat 		&unwrapped_map,
					const cv::Mat	&wrapped_map)
{
	int 			rows = unwrapped_map.rows;
	int 			cols = unwrapped_map.cols;
	double 			unwrapping_value;
	const double* 	wrapped_ptr;
	double* 		unwrapped_ptr;

	if (unwrapped_map.empty())
	{
		std::cout << "ERROR: input unwrapped map is empty" << std::endl;
		return EXIT_FAILURE;
	}

	if (wrapped_map.rows != rows || wrapped_map.cols != cols)
	{
		std::cout << "ERROR: phase map resolution not match" << std::endl;
		return EXIT_FAILURE;
	}

	for (int row = 0; row < rows; ++row)
	{
		wrapped_ptr 	= wrapped_map.ptr<double>(row);
		unwrapped_ptr 	= unwrapped_map.ptr<double>(row);

		for (int col = 0; col < cols; ++col)
		{
			unwrapping_value = std::round(unwrapped_ptr[col] - wrapped_ptr[col]/2.0);
			unwrapped_ptr[col] = (unwrapping_value*2.0 + wrapped_ptr[col]);
		}
	}

	return EXIT_SUCCESS;
}

//#############################################################################
//
//  applyDMWL(): apply digital multi-wavelength phase unwrapping
//
//#############################################################################
int applyDMWL(	const std::vector<cv::Mat> 	&PatternVec,
				const int 					waveSteps,
				cv::Mat 					&disparity_map)
{
	size_t 		pattern_count = PatternVec.size();
	cv::Mat 	image_0;
	cv::Mat 	image_120;
	cv::Mat 	image_240;
	cv::Mat 	wrapped_map;

	if (waveSteps < 2)
	{
		std::cout << "ERROR: waveSteps should be at least 2" << std::endl;
		return EXIT_FAILURE;
	}

	if (waveSteps*3 != pattern_count)
	{
		std::cout << "ERROR: Expect " << waveSteps*3 << "images, but " 
					<< pattern_count << " were input" << std::endl;
		return EXIT_FAILURE;
	}

	for (int i = 0; i < waveSteps; ++i)
	{
		image_0 	= PatternVec.at(3*i);
		image_120 	= PatternVec.at(3*i+1);
		image_240 	= PatternVec.at(3*i+2);

		if (computePhaseMap(image_0,
							image_120,
							image_240,
							wrapped_map) == EXIT_FAILURE)
		{
			std::cout << "ERROR: computePhaseMap()" << std::endl;
			return EXIT_FAILURE;
		}

		if (i == 0)
		{
			wrapped_map.copyTo(disparity_map);
			continue;
		}

		if (unwrapPhaseDMWL(disparity_map,
							wrapped_map) == EXIT_FAILURE)
		{
			std::cout << "ERROR: unwrapPhaseDMWL()" << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

} //--- namespace mypu