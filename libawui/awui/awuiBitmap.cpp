// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiBitmap.h"
#include <stdlib.h>

awuiBitmap::awuiBitmap(int width, int height)
{
	this->width = width;
	this->height = height;
	this->btpp = 4;
	this->image = (unsigned char *) calloc (this->btpp * this->width * this->height, 1);
}

awuiBitmap::~awuiBitmap()
{
}