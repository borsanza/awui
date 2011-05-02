// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

typedef struct _aw aw;
typedef struct _ac ac;

class awuiApplication;

class awuiForm {
	friend class awuiApplication;

	private:
		int x, y;
		int width, height;
		int red, green, blue;
		aw * w;
		void OnPaintPre();

		void ProcessEvents(ac * c);

	public:
		awuiForm();
		~awuiForm();

		void SetLeft(int x);
		void SetTop(int y);
		void SetLocation(int x, int y);

		void SetWidth(int width);
		void SetHeight(int height);
		void SetSize(int width, int height);

		void SetBounds(int x, int y, int width, int height);
		
		int GetTop();
		int GetLeft();
		void GetLocation(int &x, int &y);

		int GetWidth();
		int GetHeight();
		void GetSize(int &width, int &height);

		void GetBounds(int &x, int &y, int &width, int &height);

		void Show();
		virtual void OnPaint() {}
		virtual void OnResize() {}

		void SetBackColor(int red, int green, int blue);
		void GetBackColor(int &red, int &green, int &blue);
};
