#pragma once

namespace awui {
	class Math {
		public:
			static constexpr double PI = 3.14159265358979323846;
			static constexpr double E = 2.7182818284590452354;

			static double Abs(double value);
			static float Abs(float value);
			static int Abs(int value);

			static double Acos(double d);
			static float Acos(float d);

			static double Asin(double d);
			static float Asin(float d);

			static double Atan(double d);
			static float Atan(float d);

			static double Atan2(double y, double x);
			static float Atan2(float y, float x);

			static double Ceiling(double a);
			static float Ceiling(float a);

			static double Cos(double d);
			static float Cos(float d);

			static double Cosh(double value);
			static float Cosh(float value);

			static double Exp(double d);
			static float Exp(float d);

			static double Floor(double d);
			static float Floor(float d);

			static double FMod(double x, double y);
			static float FMod(float x, float y);

			static double Log(double d);
			static float Log(float d);

			static double Log10(double d);
			static float Log10(float d);

			static double Max(double val1, double val2);
			static float Max(float val1, float val2);
			static int Max(int val1, int val2);

			static double Min(double val1, double val2);
			static float Min(float val1, float val2);
			static int Min(int val1, int val2);

			static double Clamp(double value, double min, double max);
			static float Clamp(float value, float min, float max);
			static int Clamp(int value, int min, int max);

			static double Pow(double x, float y);
			static float Pow(float x, float y);

			static double Round(double x);
			static float Round(float x);

			static int Sign(double value);
			static int Sign(float value);
			static int Sign(short value);
			static int Sign(long value);
			static int Sign(int value);

			static double Sin(double a);
			static float Sin(float a);

			static double Sinh(double value);
			static float Sinh(float value);

			static double Sqrt(double d);
			static float Sqrt(float d);

			static double Tan(double a);
			static float Tan(float a);

			static double Tanh(double value);
			static float Tanh(float value);

			static float Interpolate(float from, float to, float percent, bool rounding = true);
	};
}
