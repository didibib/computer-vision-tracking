/*
 * Util.cpp
 *
 *  Created on: Nov 13, 2013
 *      Author: coert
 */

#include "cvpch.h"
#include "util.h"


using namespace std;

namespace nl_uu_science_gmt
{

const string Util::CBConfigFile         = "checkerboard.xml";
const string Util::CalibrationVideo     = "calibration.avi";
const string Util::CheckerboadVideo     = "checkerboard.avi";
const string Util::BackgroundImageFile  = "background.png";
const string Util::VideoFile            = "video.avi";
const string Util::IntrinsicsFile       = "intrinsics.xml";
const string Util::CheckerboadCorners   = "boardcorners.xml";
const string Util::ConfigFile           = "config.xml";

/**
 * Linux/Windows friendly way to check if a file exists
 */
bool Util::fexists(const std::string &filename)
{
	ifstream ifile(filename.c_str());
	return ifile.is_open();
}

} /* namespace nl_uu_science_gmt */
