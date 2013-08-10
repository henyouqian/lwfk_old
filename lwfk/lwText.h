#ifndef __LW_TEXT_H__
#define __LW_TEXT_H__

#include "lwfk/lwColor.h"
#include "PVRTArray.h"

namespace lw{
    
    enum TextAlign{
		ALIGN_TOP_LEFT,
		ALIGN_TOP_MID,
		ALIGN_TOP_RIGHT,
		ALIGN_MID_LEFT,
		ALIGN_MID_MID,
		ALIGN_MID_RIGHT,
		ALIGN_BOTTOM_LEFT,
		ALIGN_BOTTOM_MID,
		ALIGN_BOTTOM_RIGHT,
	};
    
    class FontRes;
	
    class Text{
	public:
		static Text* create(const char *fntFile);
		~Text();
        
        void draw();
		void setPos(float x, float y);
		void setAlign(TextAlign align);
		void setText(const char *text);
        void setRotate(float rotate);
		void setScale(float scaleX, float scaleY);
		void setColor(const lw::Color& color);
		const char* getText();
		void getPos(float &x, float &y);
        void getSize(float &w, float &h);
        
	private:
		Text(const char *fntFile, bool &ok);
		void update();
        
		FontRes* _pRes;
        CPVRTArray<PVRTuint32> _text;
        std::string _utf8str;
		TextAlign _align;
		float _posX, _posY;
        float _scaleX, _scaleY;
		float _rotate;
        lw::Color _color;
        float _width, _height;
        std::vector<float> _linesOffset;
        float _y0;
		bool _needUpdate;
        bool _isHD;
	};
    
} //namespace lw

#endif //__LW_TEXT_H__