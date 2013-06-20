#include "lwPrefix.h"
#include "lwColor.h"

namespace lw{

	const Color COLOR_BLACK = Color(0.f, 0.f, 0.f, 1.f);
	const Color COLOR_WHITE = Color(1.f, 1.f, 1.f, 1.f);
	const Color COLOR_NONE = Color(0.f, 0.f, 0.f, 0.f);

    
	bool operator == (const Color& a, const Color& b){
		return ( a.a == b.a && a.r == b.r && a.g == b.g && a.b == b.b );
	}

	bool operator != (const Color& a, const Color& b){
		return !( a.a == b.a && a.r == b.r && a.g == b.g && a.b == b.b );
	}

    Color::Color():r(1.f), g(1.f), b(1.f), a(1.f) {
        
    }
    
    Color::Color(float rr, float gg, float bb, float aa) {
        r = rr; g = gg; b = bb; a = aa;
    }
    
    Color::Color(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa) {
        r = (float)rr/255.f;
        g = (float)gg/255.f;
        b = (float)bb/255.f;
        a = (float)aa/255.f;
    }
    
    Color::Color(int rr, int gg, int bb, int aa) {
        r = (float)rr/255.f;
        g = (float)gg/255.f;
        b = (float)bb/255.f;
        a = (float)aa/255.f;
    }
    
    void Color::set(float rr, float gg, float bb, float aa) {
        r = rr; g = gg; b = bb; a = aa;
    }
    
    void Color::set(int rr, int gg, int bb, int aa) {
        r = (float)rr/255.f;
        g = (float)gg/255.f;
        b = (float)bb/255.f;
        a = (float)aa/255.f;
    }
    
    void Color::set(unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa) {
        r = (float)rr/255.f;
        g = (float)gg/255.f;
        b = (float)bb/255.f;
        a = (float)aa/255.f;
    }

	void rgb2Hsv(float R, float G, float B, float& H, float& S, float&V)
	{
		// r,g,b values are from 0 to 1
		// h = [0,360], s = [0,1], v = [0,1]
		// if s == 0, then h = -1 (undefined)

		float m1, m2, delta,tmp;
		tmp = std::min(R, G);
		m1 = std::min( tmp, B );
		tmp = std::max( R, G);
		m2 = std::max(tmp, B );
		V = m2; // v

		delta = m2 - m1;

		if( m2 != 0 )
			S = delta / m2; // s
		else
		{
			// r = g = b = 0 // s = 0, v is undefined
			S = 0;
			H = 0;
			return;
		}
		if( R == m2 )
			H = ( G - B ) / delta; // between yellow & magenta
		else if( G == m2 )
			H = 2 + ( B - R ) / delta; // between cyan & yellow
		else
			H = 4 + ( R - G ) / delta; // between magenta & cyan

		H *= 60; // degrees
		if( H < 0 )
			H += 360;
	}



	void hsv2Rgb(float H, float S, float V, float &R, float &G, float &B)
	{
		int i;
		float f, p, q, t;

		if( S == 0 ) 
		{
			// achromatic (grey)
			R = G = B = V;
			return;
		}

		H /= 60; // sector 0 to 5
		i = (int)H;
		f = H - i; // factorial part of h
		p = V * ( 1 - S );
		q = V * ( 1 - S * f );
		t = V * ( 1 - S * ( 1 - f ) );

		switch( i ) 
		{
		case 0: 
			R = V;
			G = t;
			B = p;
			break;
		case 1:
			R = q;
			G = V;
			B = p;
			break;
		case 2:
			R = p;
			G = V;
			B = t;
			break;
		case 3:
			R = p;
			G = q;
			B = V;
			break;
		case 4:
			R = t;
			G = p;
			B = V;
			break;
		default: // case 5:
			R = V;
			G = p;
			B = q;
			break;
		}
	}

} //namespace lw