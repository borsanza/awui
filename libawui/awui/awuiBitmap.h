// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiImage.h"

class awuiBitmap : public awuiImage {
public:
	awuiBitmap(int width, int height);
	~awuiBitmap();

	virtual int IsClass(awuiClasses objectClass) const;
};