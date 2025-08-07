#pragma once

// »ö»ó
enum class Color
{
	Blue = 1,
	Green = 2,
	Cyan = Blue | Green,
	Red = 4,
	Magenta = Blue | Red,
	Yellow = Green | Red,
	White = Red | Green | Blue,
	Intensity = 8,

	BlueIntensity = Blue | Intensity,
	GreenIntensity = Green | Intensity,
	CyanIntensity = Blue | Green | Intensity,
	RedIntensity = Red | Intensity,
	MagentaIntensity = Blue | Red | Intensity,
	YellowIntensity = Green | Red | Intensity,
	WhiteIntensity = Red | Green | Blue | Intensity,
};