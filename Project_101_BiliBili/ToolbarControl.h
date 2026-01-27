#pragma once
#include <Windows.h>
#include <vector>
#include <string>

struct ToolParameter
{
	private:
		float current = 0.2f;
	public:
		float min = 0;
		float max = 1.0f;
		std::string name;

		void SetValue(float val) 
		{ 
			if (val < min) current = min;
			else if (val > max) current = max;
			else current = val; 
		}
		float GetValue() const { return current; }

		ToolParameter(std::string paramName, float minVal, float maxVal, float currentVal)
			: name(paramName)
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
		{ "move-speed", 0, 1.0f, 0.2f},
		{ "bullet-speed", 0.05f, 1.0f, 0.2f},
		{ "bullet-recovery", 1.0f, 5.0f, 2.0f},
		{ "bullet-damage", 0, 0.3f, 0.1f},
		{ "item-spawn", 0, 20.0f, 15.0f},
		{ "bilibili-duration", 1.0f, 20.0f, 5.0f},
		{ "bilibili-damage", 0, 0.01f, 0.001f},
	};
};