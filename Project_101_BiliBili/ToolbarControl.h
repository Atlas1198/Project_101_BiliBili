#pragma once
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <vector>
#include <string>

struct ToolParameter
{
	private:
		int current = 2;
	public:
		float divisionBy = 10;
		int min = 0;
		int max = 10;
		std::string name;

		void SetValue(int val) 
		{ 
			if (val < min) current = min;
			else if (val > max) current = max;
			else current = val; 
		}
		float GetValue() const { return static_cast<float>(current) / divisionBy; }
		int GetIntValue() const { return current; }

		ToolParameter(std::string paramName, int minVal, int maxVal, int currentVal, float divBy)
			: name(paramName), divisionBy(divBy)
		{
			min = minVal;
			max = maxVal;
			current = currentVal;
		}
};

struct ToolbarControl
{
	HWND hToolbar = NULL;
	bool mousedown = false;
	POINT lastLocation;

	std::vector<ToolParameter> parameters =
	{
		{ "move-speed", 0, 10, 2, 10.0f },
	};
};