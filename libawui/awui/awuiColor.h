// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

class awuiColor {
private:
	unsigned char a, r, g, b;
	awuiColor();

public:
	unsigned char GetA();
	unsigned char GetR();
	unsigned char GetG();
	unsigned char GetB();
	int ToArgb();
	float GetBrightness();
	float GetHue();
	float GetSaturation();

	static awuiColor * FromArgb(int argb);
	static awuiColor * FromArgb(int alpha, awuiColor * baseColor);
	static awuiColor * FromArgb(int red, int green, int blue);
	static awuiColor * FromArgb(int alpha, int red, int green, int blue);
};