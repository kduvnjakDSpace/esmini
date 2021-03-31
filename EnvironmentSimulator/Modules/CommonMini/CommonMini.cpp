﻿/* 
 * esmini - Environment Simulator Minimalistic 
 * https://github.com/esmini/esmini
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) partners of Simulation Scenarios
 * https://sites.google.com/view/simulationscenarios
 */

#include <stdarg.h> 
#include <stdio.h>
#include <iostream>

#include "CommonMini.hpp"


#define DEBUG_TRACE

 // These variables are autogenerated and compiled
 // into the library by the version.cmake script
extern const char* ESMINI_GIT_TAG;
extern const char* ESMINI_GIT_REV;
extern const char* ESMINI_GIT_BRANCH;
extern const char* ESMINI_BUILD_VERSION;

static SE_SystemTime systemTime_;

const char* esmini_git_tag(void)
{
	return ESMINI_GIT_TAG;
}

const char* esmini_git_rev(void)
{
	return ESMINI_GIT_REV;
}

const char* esmini_git_branch(void)
{
	return ESMINI_GIT_BRANCH;
}

const char* esmini_build_version(void)
{
	return ESMINI_BUILD_VERSION;
}

bool FileExists(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

std::string CombineDirectoryPathAndFilepath(std::string dir_path, std::string file_path)
{
	std::string path = file_path;

	if (file_path[0] != '/' || file_path[0] != '\\' || file_path[1] != ':')
	{
		// Relative path. Make sure it starts with ".." or "./"
		if (path[0] != '.')
		{
			path.insert(0, "./");
		}
		if (dir_path != "")
		{
			// Combine with directory path
			path.insert(0, dir_path + '/');
		}
	}

	return path;
}

double GetAngleOfVector(double x, double y)
{
	double angle;
	if (abs(x) < SMALL_NUMBER)
	{
		if (abs(y) < SMALL_NUMBER)
		{
			return 0.0; // undefined
		}
		x = SIGN(x) * SMALL_NUMBER;
	}
	angle = atan2(y, x);
	if (angle < 0.0)
	{
		angle += 2*M_PI;
	}
	return angle;
}

double GetAbsAngleDifference(double angle1, double angle2)
{
	double diff = fmod(angle1 - angle2, 2 * M_PI);

	if (diff < 0)
	{
		diff += 2 * M_PI;
	}

	if (diff > M_PI)
	{
		diff = 2 * M_PI - diff;
	}

	return GetAngleInInterval2PI(diff);
}

double GetAngleDifference(double angle1, double angle2)
{
	double diff = fmod(angle1 - angle2, 2 * M_PI);

	if (diff < -M_PI)
	{
		diff += 2 * M_PI;
	}
	else if (diff > M_PI)
	{
		diff -= 2 * M_PI;
	}

	return diff;
}

bool IsAngleStraight(double teta)
{
	teta = fmod(teta + M_PI, 2 * M_PI);

	if (teta < 0)
		teta += 2 * M_PI;

	teta = teta - M_PI;

	if (teta >= -(M_PI / 2) && teta <= (M_PI / 2))
	{
		return true;
	}
	else
	{
		return false;
	}
}

double GetAngleSum(double angle1, double angle2)
{
	double sum = fmod(angle1 + angle2, 2 * M_PI);

	if (sum < 0)
	{
		sum += 2 * M_PI;
	}

	return sum;
}

double GetAngleInInterval2PI(double angle)
{
	double angle2 = fmod(angle, 2 * M_PI);

	if (angle2 < 0)
	{
		angle2 += 2 * M_PI;
	} 
	else if (angle2 == -0)
	{
		angle2 = 0;
	}

	return angle2;
}

int GetIntersectionOfTwoLineSegments(double ax1, double ay1, double ax2, double ay2, double bx1, double by1, double bx2, double by2, double &x3, double &y3)
{
	// Inspiration: https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

	double t_demonitator = (ax1 - ax2) * (by1 - by2) - (ay1 - ay2) * (bx1 - bx2);
	
	if(fabs(t_demonitator) < SMALL_NUMBER)
	{
		return -1;
	}

	double t = ((ax1 - bx1) * (by1 - by2) - (ay1 - by1) * (bx1 - bx2)) / t_demonitator;

	x3 = ax1 + t * (ax2 - ax1);
	y3 = ay1 + t * (ay2 - ay1);

	return 0;
}

bool PointInBetweenVectorEndpoints(double x3, double y3, double x1, double y1, double x2, double y2, double &sNorm)
{
	bool inside;

	if (fabs(y2 - y1) < SMALL_NUMBER && fabs(x2 - x1) < SMALL_NUMBER)
	{
		// Point - not really a line 
		// Not sure if true of false should be returned
		sNorm = 0;
		inside = true;
	}
	else if (fabs(x2 - x1) < fabs(y2 - y1))  // Line is steep (more vertical than horizontal
	{
		sNorm = (y3 - y1) / (y2 - y1);
		if (y2 > y1)  // ascending
		{
			inside = !(y3 < y1 || y3 > y2);
		}
		else
		{
			inside = !(y3 > y1 || y3 < y2);
		}
	}
	else
	{
		sNorm = (x3 - x1) / (x2 - x1);
		if (x2 > x1)  // forward
		{
			inside = !(x3 < x1 || x3 > x2);
		}
		else
		{
			inside = !(x3 > x1 || x3 < x2);
		}
	}
	if (!inside)
	{
		if (sNorm < 0)
		{
			sNorm = -PointDistance2D(x3, y3, x1, y1);
		}
		else
		{
			sNorm = PointDistance2D(x3, y3, x2, y2);
		}
	}
	return inside;
}

double DistanceFromPointToEdge2D(double x3, double y3, double x1, double y1, double x2, double y2, double* x, double* y)
{
	double px = 0;
	double py = 0;
	double distance = 0;
	double sNorm = 0;
	
	// First project point on edge
	ProjectPointOnVector2D(x3, y3, x1, y1, x2, y2, px, py);
	distance = PointDistance2D(x3, y3, px, py);

	if (PointInBetweenVectorEndpoints(px, py, x1, y1, x2, y2, sNorm))
	{
		// Point within edge interior
		*x = px;
		*y = py;
	}
	else if (sNorm < 0)
	{
		// measure to first endpoint
		distance = PointDistance2D(x3, y3, x1, y1);
		*x = x1;
		*y = y1;
	}
	else
	{
		// measure to other (2:nd) endpoint
		distance = PointDistance2D(x3, y3, x2, y2);
		*x = x2;
		*y = y2;
	}

	return distance;
}

int PointSideOfVec(double px, double py, double vx1, double vy1, double vx2, double vy2)
{
	// Use cross product
	return SIGN(GetCrossProduct2D((vx2 - vx1), (px - vx1), (vy2 - vy1), (py - vy1)));
}

double PointDistance2D(double x0, double y0, double x1, double y1)
{
	return sqrt((x1 - x0)*(x1 - x0) + (y1 - y0) * (y1 - y0));
}

double PointToLineDistance2DSigned(double px, double py, double lx0, double ly0, double lx1, double ly1)
{
	double l0x = lx1 - lx0;
	double l0y = ly1 - ly0;
	double cp = GetCrossProduct2D(lx1 - lx0, ly1 - ly0, px - lx0, py - ly0);
	double l0Length = sqrt(l0x * l0x + l0y * l0y);
	return cp / l0Length;
}

double PointSquareDistance2D(double x0, double y0, double x1, double y1)
{
	return (x1 - x0)*(x1 - x0) + (y1 - y0) * (y1 - y0);
}

void ProjectPointOnVector2D(double x, double y, double vx1, double vy1, double vx2, double vy2, double &px, double &py)
{
	// Project the given point on the straight line between geometry end points
	// https://stackoverflow.com/questions/1811549/perpendicular-on-a-line-from-a-given-point

	double dx = vx2 - vx1;
	double dy = vy2 - vy1;

	if (fabs(dx) < SMALL_NUMBER && fabs(dy) < SMALL_NUMBER)
	{
		// Line too small - projection not possible, copy first point position
		px = vx1;
		py = vy1;
	}
	else
	{
		double k = (dy * (x - vx1) - dx * (y - vy1)) / (dy*dy + dx*dx);
		px = x - k * dy;
		py = y + k * dx;
	}
}

double GetLengthOfLine2D(double x1, double y1, double x2, double y2)
{
	return sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

double GetLengthOfVector3D(double x, double y, double z)
{
	return sqrt(x*x + y*y + z*z);
}

void RotateVec2D(double x, double y, double angle, double &xr, double &yr)
{
	xr = x * cos(angle) - y * sin(angle);
	yr = x * sin(angle) + y * cos(angle);
}

void Global2LocalCoordinates(double xTargetGlobal, double yTargetGlobal, 
							 double xHostGlobal, double yHostGlobal, double angleHost, 
							 double &targetXforHost, double &targetYforHost)
{
	double relativeX = xTargetGlobal - xHostGlobal;
	double relativeY = yTargetGlobal - yHostGlobal;
	targetXforHost = relativeX * cos(angleHost) - relativeY * sin(angleHost);
	targetYforHost = relativeX * sin(angleHost) + relativeY * cos(angleHost);
}

void SwapByteOrder(unsigned char *buf, int data_type_size, int buf_size)
{
	unsigned char *ptr = buf;
	unsigned char tmp;

	if (data_type_size < 2)
	{
		// No need to swap for one byte data types
		return;
	}

	for (int i = 0; i < buf_size / data_type_size; i++)
	{
		for (int j = 0; j < data_type_size / 2; j++)
		{
			tmp = ptr[j];
			ptr[j] = ptr[data_type_size - j - 1];
			ptr[data_type_size - j - 1] = tmp;
		}
		ptr += data_type_size;
	}
}

int strtoi(std::string s) 
{
	return atoi(s.c_str());
}

double strtod(std::string s) 
{
	return atof(s.c_str());
}

#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)

	#include <windows.h>
	#include <process.h>

	__int64 SE_getSystemTime()
	{
		return timeGetTime();
	}

	void SE_sleep(unsigned int msec)
	{
		Sleep(msec);
	}

#else

	#include <chrono>

	using namespace std::chrono;

	__int64 SE_getSystemTime()
	{
		return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	}

	void SE_sleep(unsigned int msec)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds((int)(msec)));
	}

#endif

double SE_getSimTimeStep(__int64 &time_stamp, double min_time_step, double max_time_step)
{
	double dt;

	__int64 now = SE_getSystemTime();

	if (time_stamp == 0)
	{
		// First call. Return minimal dt
		dt = min_time_step;
	}
	else
	{
		dt = (now - time_stamp) * 0.001;  // step size in seconds

		if (dt > max_time_step) // limit step size
		{
			dt = max_time_step;
		}
		else if (dt < min_time_step)  // avoid CPU rush, sleep for a while
		{
			SE_sleep((int)((min_time_step - dt) * 1000));
			now = SE_getSystemTime();
			dt = min_time_step;
		}
	}
	time_stamp = now;

	return dt;
}

std::vector<std::string> SplitString(const std::string& s, char separator)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;

	while ((pos = s.find(separator, pos)) != std::string::npos)
	{
		std::string substring(s.substr(prev_pos, pos - prev_pos));
		output.push_back(substring);
		prev_pos = ++pos;
	}
	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

	return output;
}

std::string DirNameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");

	return (std::string::npos == pos) ? "./" : fname.substr(0, pos);
}

std::string FileNameOf(const std::string& fname)
{
	size_t pos = fname.find_last_of("\\/");
	if (pos != std::string::npos)
	{
		return (fname.substr(pos+1));
	}
	else
	{
		return fname;  // Assume filename with no separator
	}
}

std::string FileNameWithoutExtOf(const std::string& fname)
{
	size_t start_pos = fname.find_last_of("\\/");
	if (start_pos != std::string::npos)
	{
		start_pos++;
	}
	else
	{
		start_pos = 0;
	}

	size_t end_pos = fname.find_last_of(".");
	if (end_pos != std::string::npos)
	{
		return (fname.substr(start_pos, end_pos - start_pos));
	}
	else
	{
		return (fname.substr(start_pos));
	}
}

double GetCrossProduct2D(double x1, double y1, double x2, double y2)
{
	return x1 * y2 - x2 * y1;
}

double GetDotProduct2D(double x1, double y1, double x2, double y2)
{
	return x1 * x2 + y1 * y2;
}

void NormalizeVec2D(double x, double y, double &xn, double &yn)
{
	double len = sqrt(x*x + y*y);
	if (len < SMALL_NUMBER)
	{
		len = SMALL_NUMBER;
	}
	xn = x / len;
	yn = y / len;
}

void OffsetVec2D(double x0, double y0, double x1, double y1, double offset, double& xo0, double& yo0, double& xo1, double& yo1)
{
	double angle_line = atan2(y1 - y0, x1 - x0);
	double angle_offset = angle_line + (offset < 0 ? M_PI_2 : -M_PI_2);  // perpendicular to line
	double line_offset[2] = { fabs(offset) * cos(angle_offset), fabs(offset) * sin(angle_offset) };
	
	xo0 = x0 + line_offset[0];
	yo0 = y0 + line_offset[1];
	xo1 = x1 + line_offset[0];
	yo1 = y1 + line_offset[1];
}

void ZYZ2EulerAngles(double z0, double y, double z1, double& h, double& p, double& r)
{
	double cx = cos(z0);
	double cy = cos(y);
	double cz = cos(z1);
	double sx = sin(z0);
	double sy = sin(y);
	double sz = sin(z1);

	// Create a rotation matrix Z0 * Y * Z1
	double m[3][3] =
	{
		{cx * cy * cz - sx * sz, -cx * cy * sz - sx * cz, cx * sy} ,
		{sx * cy * cz + cx * sz, cx * cz - sx * cy * sz, sx * sy},
		{-sy * cz, sy * sz, cy}
	};

	// Avoid gimbal lock
	if (fabs(m[0][0]) < SMALL_NUMBER) m[0][0] = SIGN(m[0][0]) * SMALL_NUMBER;
	if (fabs(m[2][2]) < SMALL_NUMBER) m[2][2] = SIGN(m[2][2]) * SMALL_NUMBER;

	h = atan2(m[1][0], m[0][0]);
	p = atan2(-m[2][0], sqrt(m[2][1] * m[2][1] + m[2][2] * m[2][2]));
	r = atan2(m[2][1], m[2][2]);
}

void R0R12EulerAngles(double h0, double p0, double r0, double h1, double p1, double r1, double& h, double& p, double& r)
{
	// 1. Create two rotation matrices 
	// 2. Multiply them
	// 3. Extract yaw. pitch , roll

	double cx = cos(h0);
	double cy = cos(p0);
	double cz = cos(r0);
	double sx = sin(h0);
	double sy = sin(p0);
	double sz = sin(r0);

	double R0[3][3] =
	{
		{cx * cy, cx * sy * sz - sx * cz, sx * sz + cx * sy * cz} ,
		{sx * cy, cx * cz + sx * sy * sz, sx * sy * cz - cx * sz},
		{-sy, cy * sz, cy * cz}
	};

	cx = cos(h1);
	cy = cos(p1);
	cz = cos(r1);
	sx = sin(h1);
	sy = sin(p1);
	sz = sin(r1);

	double R1[3][3] =
	{
		{cx * cy, cx * sy * sz - sx * cz, sx * sz + cx * sy * cz},
		{sx * cy, cx * cz + sx * sy * sz, sx * sy * cz - cx * sz},
		{-sy, cy * sz, cy * cz}
	};

	// Multiply
	double R2[3][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				R2[i][j] += R0[i][k] * R1[k][j];

	// Avoid gimbal lock
	if (fabs(R2[0][0]) < SMALL_NUMBER) R2[0][0] = SIGN(R2[0][0]) * SMALL_NUMBER;
	if (fabs(R2[2][2]) < SMALL_NUMBER) R2[2][2] = SIGN(R2[2][2]) * SMALL_NUMBER;

	h = GetAngleInInterval2PI(atan2(R2[1][0], R2[0][0]));
	p = GetAngleInInterval2PI(atan2(-R2[2][0], sqrt(R2[2][1] * R2[2][1] + R2[2][2] * R2[2][2])));
	r = GetAngleInInterval2PI(atan2(R2[2][1], R2[2][2]));
}

int SE_Env::AddPath(std::string path)
{
	// Check if path already in list
	for (size_t i = 0; i < paths_.size(); i++)
	{
		if (paths_[i] == path)
		{
			return -1;
		}
	}
	paths_.push_back(path);

	return 0;
}

Logger::Logger() : callback_(0), time_(0)
{
	callback_ = 0;
	time_ = 0;
}

Logger::~Logger()
{
	if (file_.is_open())
	{
		file_.close();
	}

	callback_ = 0;
}

bool Logger::IsCallbackSet()
{
	return callback_ != 0;
}

void Logger::Log(bool quit, char const* file, char const* func, int line, char const* format, ...)
{
	static char complete_entry[2048];
	static char message[1024];

	va_list args;
	va_start(args, format);
	vsnprintf(message, 1024, format, args);

#ifdef DEBUG_TRACE
	if (time_)
	{
		snprintf(complete_entry, 2048, "%.3f %s / %d / %s(): %s", *time_, file, line, func, message);
	}
	else
	{
		snprintf(complete_entry, 2048, "%s / %d / %s(): %s", file, line, func, message);
	}
#else
	strncpy(complete_entry, message, 1024);
#endif

	if (file_.is_open())
	{
		file_ << complete_entry << std::endl;
		file_.flush();
	}

	if (callback_)
	{
		callback_(complete_entry);
	}

	va_end(args);

	if (quit)
	{
		throw std::runtime_error(complete_entry);
	}
}

void Logger::SetCallback(FuncPtr callback)
{
	callback_ = callback;

	static char message[1024];

	snprintf(message, 1024, "esmini GIT REV: %s", esmini_git_rev());
	callback_(message);
	snprintf(message, 1024, "esmini GIT TAG: %s", esmini_git_tag());
	callback_(message);
	snprintf(message, 1024, "esmini GIT BRANCH: %s", esmini_git_branch());
	callback_(message);
	snprintf(message, 1024, "esmini BUILD VERSION: %s", esmini_build_version());
	callback_(message);
}

Logger& Logger::Inst()
{
	static Logger instance_;
	return instance_;
}

void Logger::OpenLogfile()
{
#ifndef SUPPRESS_LOG
	if (!SE_Env::Inst().GetLogFilePath().empty())
	{
		if (file_.is_open())
		{
			// Close any open logfile, perhaps user want a new with unique filename
			file_.close();
		}

		file_.open(SE_Env::Inst().GetLogFilePath().c_str());
		if (file_.fail())
		{
			const char* filename = std::tmpnam(NULL);
			printf("Cannot open log file: %s in working directory. Trying system tmp-file: %s\n",
				SE_Env::Inst().GetLogFilePath().c_str(), filename);
			file_.open(filename);
			if (file_.fail())
			{
				printf("Also failed to open log file: %s. Continue without logfile, still logging to console.\n", filename);
			}
		}
		if (file_.is_open())
		{
			static char message[1024];
			snprintf(message, 1024, "esmini GIT REV: %s", esmini_git_rev());
			file_ << message << std::endl;
			snprintf(message, 1024, "esmini GIT TAG: %s", esmini_git_tag());
			file_ << message << std::endl;
			snprintf(message, 1024, "esmini GIT BRANCH: %s", esmini_git_branch());
			file_ << message << std::endl;
			snprintf(message, 1024, "esmini BUILD VERSION: %s", esmini_build_version());
			file_ << message << std::endl;
			file_.flush();
		}
	}
#endif
}

SE_Env& SE_Env::Inst()
{
	static SE_Env instance_;
	return instance_;
}

void SE_Env::SetLogFilePath(std::string logFilePath)
{
	logFilePath_ = logFilePath;
	if (Logger::Inst().IsFileOpen())
	{
		// Probably user wants another logfile with a new name
		Logger::Inst().OpenLogfile();
	}
}

/*
 * Logger for all vehicles contained in the Entities vector.
 * 
 * Builds a header based on the number of vehicles then prints data
 * in columnar format, with time running from top to bottom and
 * vehicles running from left to right, starting with the Ego vehicle
 */
CSV_Logger::CSV_Logger(std::string scenario_filename, int numvehicles, std::string csv_filename)
{

	file_.open(csv_filename);
	if (file_.fail())
	{
		throw std::iostream::failure(std::string("Cannot open file: ") + csv_filename);
	}

	data_index_ = 0;

	//Standard ESMINI log header, appended with Scenario file name and vehicle count
	static char message[1024];
	snprintf(message, 1024, "esmini GIT REV: %s", esmini_git_rev());
	file_ << message << std::endl;
	snprintf(message, 1024, "esmini GIT TAG: %s", esmini_git_tag());
	file_ << message << std::endl;
	snprintf(message, 1024, "esmini GIT BRANCH: %s", esmini_git_branch());
	file_ << message << std::endl;
	snprintf(message, 1024, "esmini BUILD VERSION: %s", esmini_build_version());
	file_ << message << std::endl;
	snprintf(message, 1024, "Scenario File Name: %s", scenario_filename.c_str());
	file_ << message << std::endl;
	snprintf(message, 1024, "Number of Vehicles: %d", numvehicles);
	file_ << message << std::endl;

	//Ego vehicle is always present, at least one set of vehicle data values should be stored 
	//Index and TimeStamp are included in this first set of columns 
	const char* egoHeader = "Index [-] , TimeStamp [sec] , #1 Entitity_Name [-] , "
		"#1 Entitity_ID [-] , #1 Current_Speed [m/sec] , #1 Wheel_Angle [deg] , "
		"#1 Wheel_Rotation [-] , #1 World_Position_X [-] , #1 World_Position_Y [-] , "
		"#1 World_Position_Z [-] , #1 Distance_Travelled_Along_Road_Segment [m] , "
		"#1 Lateral_Distance_Lanem [m] , #1 World_Heading_Angle [rad] , "
		"#1 Relative_Heading_Angle [rad] , #1 Relative_Heading_Angle_Drive_Direction [rad] , "
		"#1 World_Pitch_Angle [rad] , #1 Road_Curvature [1/m] , ";
	snprintf(message, 1024, egoHeader);
	file_ << message;

	//Based on number of vehicels in the Entities vector, extend the header accordingly
	const char* npcHeader = "#%d Entitity_Name [-] , #%d Entitity_ID [-] , "
		"#%d Current_Speed [m/sec] , #%d Wheel_Angle [deg] , #%d Wheel_Rotation [-] , "
		"#%d World_Position_X [-] , #%d World_Position_Y [-] , #%d World_Position_Z [-] , "
		"#%d Distance_Travelled_Along_Road_Segment [m] , #%d Lateral_Distance_Lanem [m] , "
		"#%d World_Heading_Angle [rad] , #%d Relative_Heading_Angle [rad] , "
		"#%d Relative_Heading_Angle_Drive_Direction [rad] , #%d World_Pitch_Angle [rad] , "
		"#%d Road_Curvature [1/m] , ";
	for (int i = 2; i <= numvehicles; i++)
	{
		snprintf(message, 1024, npcHeader, i, i, i, i, i, i, i, i, i, i, i, i, i, i, i);
		file_ << message;
	}
	file_ << std::endl;

	file_.flush();

	callback_ = 0;
}

CSV_Logger::~CSV_Logger()
{
	if (file_.is_open())
	{
		file_.close();
	}

	callback_ = 0;
}

void CSV_Logger::LogVehicleData(bool isendline, double timestamp,
	char const* name_, int id_, double speed_, double wheel_angle_, double wheel_rot_,
	double posX_, double posY_, double posZ_, double distance_road_, double distance_lanem_, 
	double heading_, double heading_angle_, double heading_angle_driving_direction_, 
	double pitch_, double curvature_, ...)
{
	static char data_entry[2048];

	//If this data is for Ego (position 0 in the Entities vector) print using the first format
	//Otherwise use the second format
	if (id_ == 0)
		snprintf(data_entry, 2048,
			"%d , %f , %s , %d , %f , %f , %f , %f , %f , %f , %f , %f, %f, %f, %f, %f , %f ,",
			data_index_, timestamp, name_, id_, speed_, wheel_angle_, wheel_rot_, posX_, posY_,
			posZ_, distance_road_, distance_lanem_, heading_, heading_angle_, 
			heading_angle_driving_direction_, pitch_, curvature_);
	else
		snprintf(data_entry, 2048,
			"%s , %d , %f , %f , %f , %f , %f , %f , %f , %f, %f, %f, %f, %f , %f,",
			name_, id_, speed_, wheel_angle_, wheel_rot_, posX_, posY_, posZ_, distance_road_,
			distance_lanem_, heading_, heading_angle_, heading_angle_driving_direction_, pitch_,
			curvature_);

	//Add lines horizontally until the endline is reached
	if (isendline == false)
	{
		file_ << data_entry;
	}
	else if (file_.is_open())
	{

		file_ << data_entry << std::endl;
		file_.flush();
		
		data_index_++;
	}

	

	if (callback_)
	{
		callback_(data_entry);
	}
}

void CSV_Logger::SetCallback(FuncPtr callback)
{
	callback_ = callback;

	static char message[1024];

	snprintf(message, 1024, "esmini GIT REV: %s", esmini_git_rev());
	callback_(message);
	snprintf(message, 1024, "esmini GIT TAG: %s", esmini_git_tag());
	callback_(message);
	snprintf(message, 1024, "esmini GIT BRANCH: %s", esmini_git_branch());
	callback_(message);
	snprintf(message, 1024, "esmini BUILD VERSION: %s", esmini_build_version());
	callback_(message);
}

//instantiator
//Filename and vehicle number are used for dynamic header creation
CSV_Logger& CSV_Logger::InstVehicleLog(std::string scenario_filename, int numvehicles, std::string csv_filename)
{
	static CSV_Logger instance(scenario_filename, numvehicles, csv_filename);
	return instance;
}

SE_Thread::~SE_Thread()
{
	Wait();
}

void SE_Thread::Start(void(*func_ptr)(void*), void *arg)
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	thread_ = (void*)_beginthread(func_ptr, 0, arg);
#else
	thread_ = std::thread(func_ptr, arg);
#endif
}


void SE_Thread::Wait()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	WaitForSingleObject((HANDLE)thread_, 3000);  // Should never need to wait for more than 3 sec
#else
	if (thread_.joinable())
	{
		thread_.join();
	}
#endif
}

SE_Mutex::SE_Mutex()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	mutex_ = (void*)CreateMutex(
		NULL,              // default security attributes
		0,             // initially not owned
		NULL);             // unnamed mutex

	if (mutex_ == NULL)
	{
		LOG("CreateMutex error: %d\n", GetLastError());
		mutex_ = 0;
	}
#else

#endif
}


void SE_Mutex::Lock()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	WaitForSingleObject(mutex_, 1000);  // Should never need to wait for more than 1 sec
#else
	mutex_.lock();
#endif
}

void SE_Mutex::Unlock()
{
#if (defined WINVER && WINVER == _WIN32_WINNT_WIN7)
	ReleaseMutex(mutex_);
#else
	mutex_.unlock();
#endif
}


void SE_Option::Usage()
{
	printf("  %s%s %s", OPT_PREFIX, opt_str_.c_str(), (opt_arg_ != "") ? std::string('<'+ opt_arg_ +'>').c_str() : "");
	printf("\n      %s\n", opt_desc_.c_str());
}


void SE_Options::AddOption(std::string opt_str, std::string opt_desc, std::string opt_arg)
{
	SE_Option opt(opt_str, opt_desc, opt_arg);
	option_.push_back(opt);
};

void SE_Options::PrintUsage()
{
	printf("\nUsage %s: [options]\n", app_name_.c_str());
	printf("Options: \n");
	for (size_t i = 0; i < option_.size(); i++)
	{
		option_[i].Usage();
	}
	printf("\n");
}

void SE_Options::PrintArgs(int argc, char *argv[], std::string message)
{
	printf("\n%s\n", message.c_str());
	for (size_t i = 1; i < argc; i++)
	{
		printf("  %s\n", argv[i]);
	}
}

bool SE_Options::GetOptionSet(std::string opt)
{
	SE_Option *option = GetOption(opt);

	if (option)
	{
		return option->set_;
	}
	else
	{
		return false;
	}
}

bool SE_Options::IsOptionArgumentSet(std::string opt)
{
	return GetOption(opt)->set_;
}

std::string SE_Options::GetOptionArg(std::string opt)
{
	SE_Option *option = GetOption(opt);

	if (option && option->opt_arg_ != "")
	{
		return option->arg_value_;
	}
	else
	{
		return "";
	}
}

static void ShiftArgs(int *argc, char** argv, int start_i)
{
	if (start_i >= 0 && start_i < *argc)
	{
		for (int i = start_i; i < *argc - 1; i++)
		{
			argv[i] = argv[i + 1];
		}
		(*argc)--;
	}
}

void SE_Options::ParseArgs(int *argc, char* argv[])
{
	std::string app_name = argv[0];

	for (size_t i = 0; i < *argc; i++)
	{
		originalArgs_.push_back(argv[i]);
	}

	for (size_t i = 1; i < *argc;)
	{
		std::string arg = argv[i];

		if (!(arg.substr(0, strlen(OPT_PREFIX)) == OPT_PREFIX))
		{
			i++;
			continue;
		}

		SE_Option *option = GetOption(&argv[i][strlen(OPT_PREFIX)]); // skip prefix

		if (option)
		{
			option->set_ = true;
			if (option->opt_arg_ != "")
			{
				if (i < *argc - 1)
				{
					option->arg_value_ = argv[i+1];
					ShiftArgs(argc, argv, (int)i);
				}
				else
				{
					LOG("Argument parser error: Missing option %s argument", option->opt_str_.c_str());
					i++;
				}
			}
			ShiftArgs(argc, argv, (int)i);
		}
		else
		{
			i++;
		}
	}
}

SE_Option* SE_Options::GetOption(std::string opt)
{
	for (size_t i = 0; i < option_.size(); i++)
	{
		if (opt == option_[i].opt_str_)
		{
			return &option_[i];
		}
	}
	return 0;
}