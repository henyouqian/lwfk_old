#ifndef __LW_TEXTURE_H__
#define __LW_TEXTURE_H__

#include "lwRes.h"

namespace lw{

	class TextureRes : public Res{
	public:
		static TextureRes* create(const char* fileName);	//reserveData only for PNG
		GLuint getGlId();
		const char* getFileName();
		int getWidth();
		int getHeight();
		int getChannelNum();

	private:
		~TextureRes();
		TextureRes(const char* filename, bool& ok);
		int loadAndCreateOgl(const unsigned char* buf, int buflen);

	private:
		GLuint _glId;
		std::string _fileName;
		int _numChannels;
		int _w, _h;
	};

} //namespace lw


#endif //__LW_TEXTURE_H__