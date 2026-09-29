#pragma once

#include <cstdint>

namespace awui {
	class String;

	// Intervalo de tiempo en ticks de 100 ns, como System.TimeSpan de .NET. Tipo valor: se copia con el operador
	// por defecto
	class TimeSpan {
	  public:
		static constexpr int64_t TicksPerMillisecond = 10000;
		static constexpr int64_t TicksPerSecond = TicksPerMillisecond * 1000;
		static constexpr int64_t TicksPerMinute = TicksPerSecond * 60;
		static constexpr int64_t TicksPerHour = TicksPerMinute * 60;
		static constexpr int64_t TicksPerDay = TicksPerHour * 24;

	  private:
		int64_t ticks;

		String ConvertDecimals(int value, int decimals) const;

	  public:
		TimeSpan();
		TimeSpan(int64_t ticks);
		TimeSpan(int hours, int minutes, int seconds);
		TimeSpan(int days, int hours, int minutes, int seconds);
		TimeSpan(int days, int hours, int minutes, int seconds, int milliseconds);

		int64_t GetTicks() const;
		double GetTotalMilliseconds() const;
		double GetTotalSeconds() const;
		double GetTotalMinutes() const;
		double GetTotalHours() const;
		double GetTotalDays() const;

		int GetMilliseconds() const;
		int GetSeconds() const;
		int GetMinutes() const;
		int GetHours() const;
		int GetDays() const;

		String ToString() const;
	};
} // namespace awui
