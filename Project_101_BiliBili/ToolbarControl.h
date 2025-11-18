#pragma once
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

struct ToolParameter
{
	private:
		int current = 2;
		float divisionBy = 10;
	public:
		int min = 0;
		int max = 10;
		void SetValue(int val) 
		{ 
			if (val < min) current = min;
			else if (val > max) current = max;
			else current = val; 
		}
		float GetValue() const { return static_cast<float>(current) / divisionBy; }
		int GetIntValue() const { return current; }
};

struct ToolbarControl
{
	HWND hToolbar = NULL;
	bool mousedown = false;
	POINT lastLocation;

	ToolParameter speed;
};