/*

	Log.cpp

*/
#include "stdafx.h"
#include "Log.h"

#include <iostream>
#include <iomanip>
#include <CTIME>


void Log::AppendToLogFile(std::string text)
{
	std::ofstream logfile;
	logfile.open("logfile.txt", std::ios_base::app);
	logfile << text << std::endl;
	logfile.close();
}

void Log::AppendToLogFileWithDate(std::string text)
{
	struct tm newtime;
	__time64_t long_time;
	errno_t err;
	// Get time as 64-bit integer.
	_time64(&long_time);
	// Convert to local time.
	err = _localtime64_s(&newtime, &long_time);
	if (err)
	{
		printf("Invalid argument to _localtime64_s.");
		exit(1);
	}
	std::ofstream logfile;
	logfile.open("logfile.txt", std::ios_base::app);
	logfile << "[" << std::put_time(&newtime, "%d-%m-%Y %H-%M-%S") << "]" << std::endl;
	logfile << text << std::endl;
	logfile.close();
}