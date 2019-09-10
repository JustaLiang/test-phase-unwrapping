//#############################################################################
//
//	unwrap.h
//
//#############################################################################
#ifndef __UNWRAP_H__
#define __UNWRAP_H__

#include <opencv2/core.hpp>

namespace mypu
{

//#############################################################################
//
//  computePhaseMap(): compute phase map given three phase shifting (0,120,240)
//
//#############################################################################
int computePhaseMap(const cv::Mat	&phase_map_0,
					const cv::Mat 	&phase_map_120,
					const cv::Mat	&phase_map_240,
					cv::Mat 		&wrapped_phase_map);

//#############################################################################
//
//  unwrapPhaseDMWL(): unwrap phase of DMWL patterns
//
//#############################################################################
int unwrapPhaseDMWL(cv::Mat 		&unwrapped_map,
					const cv::Mat	&wrapped_map);

//#############################################################################
//
//  applyDMWL(): apply digital multi-wavelength phase unwrapping
//
//#############################################################################
int applyDMWL(	const std::vector<cv::Mat> 	&PatternVec,
				const int 					digitalSteps,
				cv::Mat 					&unwrapped_map);

} //--- namespace mypu

#endif //--- __UNWRAP_H__