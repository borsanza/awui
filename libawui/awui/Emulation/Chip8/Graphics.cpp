// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Graphics.h"

#include <stdlib.h>

Graphics::Graphics(int width, int height) {
	this->_width = width;
	this->_height = height;
	this->_data = (unsigned char *) malloc (sizeof(unsigned char *) * width * height);
	this->Clear();
}

Graphics::~Graphics() {
	free(this->_data);
}

void Graphics::Clear() {
	for (int i = 0; i < this->_width * this->_height; i++)
		this->_data[i] = 0;
}
