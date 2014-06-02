// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

class Registers {
	private:
		unsigned char * _v;
		unsigned char _length;

	public:
		Registers(unsigned char n);
		~Registers();

		void setV(unsigned char pos, unsigned char value);
		unsigned char getV(unsigned char pos);
};
