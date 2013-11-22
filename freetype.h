#ifndef FREETYPE_H
#define FREETYPE_H

#pragma warning(disable: 4786)

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include "stdafx.h"

#include <vector>
#include <string>
#include <stdexcept>

 
namespace freetype {

	typedef struct  {
		float height;	     
		GLuint* textures;	
		GLuint list_base; 

		void init(const char* fname, unsigned int h);
		void clean();
	} font_data;

	void print(const font_data &ft_font, float x, float y, const char *fmt, ...) ;

}

#endif