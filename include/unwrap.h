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
//  subtractPhase(): subtract two phase map (mod 2pi)
//
//#############################################################################
int subtractPhase(	const cv::Mat	&phase1,
					const cv::Mat 	&phase2,
					cv::Mat			&outPhase);

//#############################################################################
//
//  computePhaseMap(): compute phase map given 3-step phase-shifting patterns  
//
//#############################################################################
int computePhaseMap(const cv::Mat	&pattern0,
					const cv::Mat 	&pattern120,
					const cv::Mat	&pattern240,
					cv::Mat 		&wrapped_phase);

//#############################################################################
//
//  computePhaseMap(): compute phase map given N-step phase-shifting patterns
//
//#############################################################################
int computePhaseMap(const std::vector<cv::Mat>	&patternVec,
					cv::Mat 					&wrapped_phase);

//#############################################################################
//
//  unwrapPhase(): unwrap target phase with reference 
//
//#############################################################################
int unwrapPhase(const cv::Mat	&reference_phase,
				const cv::Mat 	&target_phase,
				const double 	phaseFactor,
				cv::Mat 		&unwrapped_phase);

//#############################################################################
//
//  applyDMWL(): apply digital multi-wavelength phase unwrapping
//
//#############################################################################
int applyDMWL(	const std::vector<cv::Mat> 	&PatternVec,
				const int 					digitalSteps,
				cv::Mat 					&unwrapped_phase);

//#############################################################################
//
//  apply2WL(): apply two-wavelength phase unwrapping
//
//#############################################################################
int apply2WL(	const std::vector<cv::Mat> 	&longPatterns,
				const double 				longWavelength,
				const std::vector<cv::Mat> 	&shortPatterns,
				const double				shortWavelength,
				cv::Mat 					&unwrapped_phase);

//#############################################################################
//
//  apply2FQ(): apply two-frequency phase unwrapping
//
//#############################################################################
int apply2FQ(	const std::vector<cv::Mat> 	&highPatterns,
				const double 				highFrequency,
				const std::vector<cv::Mat>	&lowPatterns,
				const double				lowFrequency,
				cv::Mat 					&unwrapped_phase);

} //--- namespace mypu

#endif //--- __UNWRAP_H__