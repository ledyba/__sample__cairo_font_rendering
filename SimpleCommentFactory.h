/*
 * SimpleCommentFactory.h
 *
 *  Created on: 2012/04/15
 *      Author: psi
 */

#ifndef SDL_SIMPLECOMMENTFACTORY_H_
#define SDL_SIMPLECOMMENTFACTORY_H_

#include <string>
#include "./ClassAccessor.h"
#ifdef WIN32
#include <cairo/cairo-win32.h>
#else
#include <cairo/cairo-ft.h>
#endif
#include <fontconfig/fontconfig.h>

namespace saccubus {
namespace draw {
namespace sdl {

class Renderer;
class SimpleCommentFactory{//: public saccubus::draw::CommentFactory {
private:
	static const double ShadowWidth;
private:
#ifndef WIN32
	DEF_ATTR_ACCESSOR(private, private, FcPattern*, pattern);
#endif
	DEF_ATTR_ACCESSOR(private, private, cairo_font_face_t*, face);
	DEF_ATTR_ACCESSOR(private, private, cairo_t*, emptyCairo);
	DEF_ATTR_ACCESSOR(private, private, cairo_surface_t*, emptySurface);
public:
	SimpleCommentFactory();//(sdl::Renderer* renderer);
	virtual ~SimpleCommentFactory();
private:
	void setColor(cairo_t* cairo, unsigned int color);
	//void setupCairo(cairo_t* cairo, const saccubus::context::Comment* comment, float factor);
	void setupCairo(cairo_t* cairo, float fontWidth, float factor);
public:
//	virtual saccubus::draw::Sprite::Handler<saccubus::draw::Sprite> renderComment(const saccubus::context::Comment* comment, float factor);
	void renderComment(const std::string& msg, float fontWidth, unsigned int color, unsigned int shadowColor, float factor);
};

}}}
#endif /* SDL_SIMPLECOMMENTFACTORY_H_ */
