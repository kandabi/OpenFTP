#pragma once
#include "stdafx.h"
#include <chrono>

class Timer : public QObject
{
	Q_OBJECT
public:
	Timer()
	{
		
	}

	void Start()
	{
		startPoint = std::chrono::high_resolution_clock::now();
	}

	long long Stop()
	{
		auto endTimePoint = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(startPoint).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimePoint).time_since_epoch().count();

		return end - start;
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> startPoint;
};