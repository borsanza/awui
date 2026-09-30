#pragma once

namespace awui::Drawing {
	class GlyphMetrics {
	  private:
		int m_width;
		int m_height;
		int m_advanceX;
		int m_advanceY;
		int m_bearingX;
		int m_bearingY;
		int m_ascent;
		int m_descent;

	  public:
		GlyphMetrics();

		int GetWidth() const;
		int GetHeight() const;
		void SetWidth(int width);
		void SetHeight(int height);

		int GetAdvanceX() const;
		int GetAdvanceY() const;
		void SetAdvanceX(int width);
		void SetAdvanceY(int height);

		int GetBearingX() const;
		int GetBearingY() const;
		void SetBearingX(int width);
		void SetBearingY(int height);

		int GetAscent() const;
		int GetDescent() const;
		void SetAscent(int ascent);
		void SetDescent(int descent);
	};
} // namespace awui::Drawing
