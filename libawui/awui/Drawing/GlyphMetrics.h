// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIGLYPHMETRICS_H__
#define __AWUIGLYPHMETRICS_H__

namespace awui {
	namespace Drawing {
		class GlyphMetrics {
			private:
				int width;
				int height;
				int advancex;
				int advancey;
				int bearingx;
				int bearingy;

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
		};
	}
}

#endif
