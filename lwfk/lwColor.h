#ifndef __LW_COLOR_H__
#define __LW_COLOR_H__

namespace lw{
	
	struct Color{
		Color();
        Color(float rr, float gg, float bb, float aa);
		Color(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa);
		Color(int rr, int gg, int bb, int aa);
		
		void set(float rr, float gg, float bb, float aa);
		void set(int rr, int gg, int bb, int aa);
        void set(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa);
        
		float r, g, b, a;
	};

	bool operator == (const Color& a, const Color& b);
	bool operator != (const Color& a, const Color& b);

	extern const Color COLOR_BLACK;
	extern const Color COLOR_WHITE;
	extern const Color COLOR_NONE;

	void rgb2Hsv(float R, float G, float B, float& H, float& S, float&V);
	void hsv2Rgb(float H, float S, float V, float &R, float &G, float &B);

} //namespace lw

#endif //__LW_COLOR_H__