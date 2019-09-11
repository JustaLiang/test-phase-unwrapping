//#############################################################################
//
//	unwrap.cpp
//
//#############################################################################
#include <iostream>
#include <cstdlib>
#include <opencv2/highgui.hpp>
#include "unwrap.h"

#define PI 3.14159265359

namespace mypu
{

//#############################################################################
//
//  subtractPhase(): phase3 = phase1-phase2 (mod 2pi)
//
//#############################################################################
int subtractPhase(	const cv::Mat	&phase1,
					const cv::Mat 	&phase2,
					cv::Mat			&phase3)
{
	int 			rows = phase1.rows;
	int 			cols = phase1.cols;
	const double* 	ph1_ptr;
	const double* 	ph2_ptr;
	double*			ph3_ptr;
	double 			value;

	if (phase2.rows != rows || phase2.cols != cols)
	{
		std::cout << "ERROR: resolution not match" << std::endl;
		return EXIT_FAILURE;
	}

	phase3.release();
	phase3.create(rows, cols, CV_64F);

	for (int row = 0; row < rows; ++row)
	{
		ph1_ptr = phase1.ptr<double>(row);
		ph2_ptr = phase2.ptr<double>(row);
		ph3_ptr = phase3.ptr<double>(row);

		for (int col = 0; col < cols; ++col)
		{
			value = ph1_ptr[col] - ph2_ptr[col];
			if (value < 0)
			{
				value += 2.0;
			}
			if (value > 2.0)
			{
				value -= 2.0;
			}
			ph3_ptr[col] = value;
		}
	}

	return EXIT_SUCCESS;
}

//#############################################################################
//
//  computePhaseMap(): compute phase map given 3-step phase-shifting patterns  
//
//#############################################################################
int computePhaseMap(const cv::Mat	&pattern0,
					const cv::Mat 	&pattern120,
					const cv::Mat	&pattern240,
					cv::Mat 		&wrapped_phase)
{
	int 			rows 	= pattern0.rows;
	int 			cols 	= pattern0.cols;
	const uchar*	pattern0_ptr;
	const uchar* 	pattern120_ptr;
	const uchar* 	pattern240_ptr;
	double* 		phase_ptr;	

	wrapped_phase.release();

	if (pattern120.rows != rows || pattern120.cols != cols ||
		pattern240.rows != rows || pattern120.cols != cols)
	{
		std::cout << "ERROR: resolution not match" << std::endl;
		return EXIT_FAILURE;
	}

	wrapped_phase.create(rows, cols, CV_64F);

	for (int row = 0; row < rows; ++row)
	{
		pattern0_ptr 	= pattern0.ptr<uchar>(row);
		pattern120_ptr 	= pattern120.ptr<uchar>(row);
		pattern240_ptr 	= pattern240.ptr<uchar>(row);
		phase_ptr 		= wrapped_phase.ptr<double>(row);

		for (int col = 0; col < cols; ++col)
		{
			phase_ptr[col] = std::atan2( 
						std::sqrt(3.0)*(pattern240_ptr[col] - pattern120_ptr[col]),
						2.0*pattern0_ptr[col] - pattern120_ptr[col] - pattern240_ptr[col])/PI +1.0;
		}
	}

	return EXIT_SUCCESS;
}

//#############################################################################
//
//  computePhaseMap(): compute phase map given N-step phase-shifting patterns
//
//#############################################################################
int computePhaseMap(const std::vector<cv::Mat>	&patternVec,
					cv::Mat 					&wrapped_phase)
{
	size_t 		steps 	= patternVec.size();
	int 		rows 	= patternVec.at(0).rows;
	int 		cols 	= patternVec.at(0).cols;
	size_t 		i;
	uchar		pattern_value;
	double*		phase_ptr;
	double 		sinSum;
	double		cosSum;

	//--- Check at least 3 patterns
	if (steps < 3)
	{
		std::cout << "ERROR: shifting steps should be at least 3" << std::endl;
		return EXIT_FAILURE;
	}

	//--- Check pattern resolution
	for (i = 0; i < steps; ++i)
	{
		if (patternVec.at(i).rows != rows || patternVec.at(i).cols != cols)
		{
			std::cout << "ERROR: resolution not match (pattern no."<< i <<")" << std::endl;
			return EXIT_FAILURE;
		}
	}

	std::vector<const uchar*> pattern_ptrs;
	pattern_ptrs.resize(steps);
	wrapped_phase.release();
	wrapped_phase.create(rows,cols,CV_64F);

	for (int row = 0; row < rows; ++row)
	{
		for (i = 0; i < steps; ++i)
		{
			pattern_ptrs[i] = patternVec[i].ptr<uchar>(row);
		}
		phase_ptr = wrapped_phase.ptr<double>(row);

		for (int col = 0; col < cols; ++col)
		{
			sinSum = 0.0;
			cosSum = 0.0;
			for (i = 0; i < steps; ++i)
			{
				pattern_value = (pattern_ptrs[i])[col];
				sinSum += std::sin(2*PI*i/steps)*pattern_value;
				cosSum += std::cos(2*PI*i/steps)*pattern_value;
			}
			phase_ptr[col] = std::atan2(sinSum, cosSum)/PI;
		}
	}

	return EXIT_SUCCESS;
}

//#############################################################################
//
//  unwrapPhase(): unwrap target phase with reference 
//
//#############################################################################
int unwrapPhase(const cv::Mat	&reference_phase,
				const cv::Mat 	&target_phase,
				const double 	phaseFactor,
				cv::Mat 		&unwrapped_phase)
{
	int rows = target_phase.rows;
	int cols = target_phase.cols;

	if (reference_phase.empty())
	{
		std::cout << "ERROR: reference phase map is empty" << std::endl;
		return EXIT_FAILURE;		
	}

	if (target_phase.empty())
	{
		std::cout << "ERROR: target phase map is empty" << std::endl;
		return EXIT_FAILURE;
	}

	if (reference_phase.rows != rows || reference_phase.cols != cols)
	{
		std::cout << "ERROR: phase map resolution not match" << std::endl;
		return EXIT_FAILURE;
	}

	unwrapped_phase.release();
	unwrapped_phase.create(rows, cols, CV_64F);

	double 			phaseOrder;
	const double*	ref_ptr;
	const double* 	tgt_ptr;
	double* 		out_ptr;

	for (int row = 0; row < rows; ++row)
	{
		ref_ptr = reference_phase.ptr<double>(row);
		tgt_ptr = target_phase.ptr<double>(row);
		out_ptr = unwrapped_phase.ptr<double>(row);

		for (int col = 0; col < cols; ++col)
		{
			phaseOrder = std::round((ref_ptr[col]*phaseFactor - tgt_ptr[col])/2.0);
			out_ptr[col] = (phaseOrder*2.0 + tgt_ptr[col]);
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
				const int 					digitalSteps,
				cv::Mat 					&unwrapped_phase)
{
	size_t 		pattern_count 	= PatternVec.size();
	cv::Mat 	pattern0;
	cv::Mat 	pattern120;
	cv::Mat 	pattern240;
	cv::Mat 	wrapped_phase;
	cv::Mat 	temp_phase;

	if (digitalSteps < 2)
	{
		std::cout << "ERROR: digitalSteps should be at least 2" << std::endl;
		return EXIT_FAILURE;
	}

	if (digitalSteps*3 != pattern_count)
	{
		std::cout << "ERROR: Expect " << digitalSteps*3 << "images, but " 
					<< pattern_count << " were input" << std::endl;
		return EXIT_FAILURE;
	}

	for (int i = 0; i < digitalSteps; ++i)
	{
		pattern0 	= PatternVec.at(3*i);
		pattern120 	= PatternVec.at(3*i+1);
		pattern240 	= PatternVec.at(3*i+2);

		if (computePhaseMap(pattern0,
							pattern120,
							pattern240,
							wrapped_phase) == EXIT_FAILURE)
		{
			std::cout << "ERROR: computePhaseMap()" << std::endl;
			return EXIT_FAILURE;
		}

		if (i == 0)
		{
			wrapped_phase.copyTo(unwrapped_phase);
			continue;
		}

		if (unwrapPhase(unwrapped_phase,
						wrapped_phase,
						2.0,
						temp_phase) == EXIT_FAILURE)
		{
			std::cout << "ERROR: unwrapPhaseDMWL()" << std::endl;
			return EXIT_FAILURE;
		}
		temp_phase.copyTo(unwrapped_phase);
	}

	return EXIT_SUCCESS;
}

//#############################################################################
//
//  apply2WL(): apply two-wavelength phase unwrapping
//
//#############################################################################
int apply2WL(	const std::vector<cv::Mat> 	&longPatterns,
				const double 				longWavelength,
				const std::vector<cv::Mat> 	&shortPatterns,
				const double				shortWavelength,
				cv::Mat 					&unwrapped_phase)
{
	if (longWavelength <= shortWavelength)
	{
		std::cout << "ERROR: long wavelength is shorter or equal to short wavelength" << std::endl;
		return EXIT_FAILURE;
	}

	cv::Mat 	longPhase;
	cv::Mat 	shortPhase;

	if (computePhaseMap(longPatterns, longPhase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: computePhaseMap() on longPatterns" << std::endl;
		return EXIT_FAILURE;
	}
	if (computePhaseMap(shortPatterns, shortPhase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: computePhaseMap() on shortPatterns" << std::endl;
		return EXIT_FAILURE;		
	}

	double 		phaseFactor;
	cv::Mat 	eqPhase;

	phaseFactor = longWavelength/(longWavelength - shortWavelength);
	subtractPhase(shortPhase, longPhase, eqPhase);

	if (unwrapPhase(eqPhase,
					shortPhase,
					phaseFactor,
					unwrapped_phase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: unwrapPhase() in apply2WL()" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

//#############################################################################
//
//  apply2FQ(): apply two-frequency phase unwrapping
//
//#############################################################################
int apply2FQ(	const std::vector<cv::Mat> 	&highPatterns,
				const double 				highFrequency,
				const std::vector<cv::Mat>	&lowPatterns,
				const double				lowFrequency,
				cv::Mat 					&unwrapped_phase)
{
	if (highFrequency <= lowFrequency)
	{
		std::cout << "ERROR: high frequency is lower or equal to low frequency" << std::endl;
		return EXIT_FAILURE;
	}

	cv::Mat 	highPhase;
	cv::Mat 	lowPhase;

	if (computePhaseMap(highPatterns, highPhase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: computePhaseMap() on highPatterns" << std::endl;
		return EXIT_FAILURE;
	}
	if (computePhaseMap(lowPatterns, lowPhase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: computePhaseMap() on lowPatterns" << std::endl;
		return EXIT_FAILURE;		
	}

	double 		phaseFactor;
	cv::Mat 	eqPhase;

	phaseFactor = highFrequency/(highFrequency - lowFrequency);
	subtractPhase(highPhase, lowPhase, eqPhase);

	if (unwrapPhase(eqPhase,
					highPhase,
					phaseFactor,
					unwrapped_phase) == EXIT_FAILURE)
	{
		std::cout << "ERROR: unwrapPhase() in apply2FQ()" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

} //--- namespace mypu