/*
 * SimpleCommentFactory.cpp
 *
 *  Created on: 2012/04/15
 *      Author: psi
 */

#include <cmath>
#include <cstring>
#include <iostream>
#include "SimpleCommentFactory.h"

namespace saccubus {
namespace draw {
namespace sdl {

const double SimpleCommentFactory::ShadowWidth = 5.0;

/*
 * コンストラクタです。オブジェクト作成時に呼ばれます。
 */
SimpleCommentFactory::SimpleCommentFactory()//(sdl::Renderer* renderer)　←　さきゅばす本実装では、レンダラを引数にとります
//:CommentFactory(renderer)
{
#ifdef WIN32 /* Windowsの時だけコンパイルされます */
	{
		/* MSDN */
		LOGFONTW logf;
		memset(&logf, 0, sizeof(LOGFONTW));
		logf.lfCharSet=DEFAULT_CHARSET;
		// こうやるとファミリーを指定可能 Lはワイド文字列のリテラルです
		//wcscpy(logf.lfFaceName, L"Arial");
		wcscpy(logf.lfFaceName, L"MS Gothic");
		//より詳しくは
		// http://msdn.microsoft.com/en-us/goglobal/bb688134
		cairo_font_face_t* ft = cairo_win32_font_face_create_for_logfontw(&logf);
		// ここでは、ヘッダでマクロを使って作成したsetterメソッドを使っています。
		// this-><プロパティ名>() がgetter、
		// this-><プロパティ名>( <パラメータ> )がsetterです
		this->face(ft);
	}
#else /* Windows以外のシステムではFreeType+fontconfigを使います */
	{ /* パターンの作成 */
		FcPattern* pattern = FcPatternCreate();
		this->pattern(pattern);
	}
	{ /* パターンを設定 */
		FcPatternAddString( this->pattern(), FC_LANG, (const FcChar8*)"ja" );
		FcPatternAddString( this->pattern(), FC_FAMILY, (FcChar8*)"gothic");
		cairo_font_options_t* opt = cairo_font_options_create();
		cairo_font_options_set_subpixel_order(opt, CAIRO_SUBPIXEL_ORDER_RGB);
		cairo_font_options_set_antialias(opt, CAIRO_ANTIALIAS_DEFAULT);
		cairo_ft_font_options_substitute(opt, this->pattern());
		cairo_font_options_destroy(opt);
	}
	{ /* フェースの作成 */
		cairo_font_face_t* ft = cairo_ft_font_face_create_for_pattern(this->pattern());
		this->face(ft);
	}
#endif
	{
		// 事前にフォントのサイズを計算するために、空のサーフェイスを作ってる。
		// FIXME: 代替策があるはず
		
		// 10x10のサーフェイスを作ります。
		this->emptySurface(cairo_image_surface_create(CAIRO_FORMAT_RGB24, 10, 10));
		// そのサーフェイスをターゲットにしたcairo_t*を作ります。
		this->emptyCairo(cairo_create(this->emptySurface()));
	}
}

 /*
  * デストラクタです。Javaでいうところのファイナライザで、
  * オブジェクトが開放される時に必ず呼ばれます。
  */
SimpleCommentFactory::~SimpleCommentFactory() {
	cairo_destroy(this->emptyCairo());
	this->emptyCairo(0);
	cairo_surface_destroy(this->emptySurface());
	this->emptySurface(0);
	cairo_font_face_destroy(this->face());
	this->face(0);
#ifdef WIN32
	// do nothing!
#else
	FcPatternDestroy(this->pattern());
	this->pattern(0);
#endif
}


//void SimpleCommentFactory::setupCairo(cairo_t* cairo, const saccubus::context::Comment* comment, float factor)
void SimpleCommentFactory::setupCairo(cairo_t* cairo, float fontWidth, float factor)
{
	// 変換行列を単位行列に戻します。
	cairo_identity_matrix(cairo);
	// ファクタに合わせてスケールしてもらいます。
	cairo_scale(cairo, factor, factor);
	// フォントの設定です。
	cairo_set_font_face(cairo, this->face());
	// 太さはこちらで設定します。
	cairo_set_font_size(cairo, fontWidth);
	// 描画先を左上に設定します。
	cairo_move_to(cairo, 0, 0);
}

// カラーコードからcairoが用いる色を設定します。
void SimpleCommentFactory::setColor(cairo_t* cairo, unsigned int color)
{
	unsigned int r = (color & 0xff0000) >> 16;
	unsigned int g = (color & 0x00ff00) >> 8;
	unsigned int b = (color & 0x0000ff) >> 0;
	float red = r/255.0f;
	float green = g/255.0f;
	float blue = b/255.0f;
	cairo_set_source_rgba(cairo, red, green, blue, 1);
}

//saccubus::draw::Sprite::Handler<saccubus::draw::Sprite> SimpleCommentFactory::renderComment(const saccubus::context::Comment* comment, float factor)
void SimpleCommentFactory::renderComment(const std::string& msg, float fontWidth, unsigned int color, unsigned int shadowColor, float factor)
{
	double x;
	double y;
	int width;
	int height;
	cairo_text_extents_t ext;
	std::cout << "Rendering...: " << msg << std::endl;
	{ /* 大きさを測定 */
		this->setupCairo(this->emptyCairo(), fontWidth, factor);
		cairo_text_extents(this->emptyCairo(), msg.c_str(), &ext);
		x = -ext.x_bearing+ShadowWidth/2;
		y = -ext.y_bearing+ShadowWidth/2;
		double w = ext.width+ShadowWidth;
		double h = ext.height+ShadowWidth;
		//このwとhは論理座標なので、実際の大きさを取得するために変換してもらう。
		cairo_user_to_device_distance(this->emptyCairo(), &w, &h);
		width = std::ceil(w);
		height = std::ceil(h);
	}
	// さきゅばす本実装では、このように描画先のスプライトを取得できます。
	// ここでは、ヘッダで指定してあるアクセサ関数を用いてレンダラを取得しています。
	//Sprite::Handler<RawSprite> spr = this->renderer()->queryRawSprite(width, height);
	{ /* 実際にレンダリング */
		/*
		RawSprite::Sessionを使うと、スプライト内のデータに直接アクセスできます。
		RawSprite::Session session(spr);
		cairo_surface_t* surface = cairo_image_surface_create_for_data(
						reinterpret_cast<unsigned char*>(session.data()),
						CAIRO_FORMAT_ARGB32,
						session.width(),
						session.height(),
						session.stride()
				);
		*/
		std::cout << width <<"x"<<height << std::endl;
		// ここではサンプルなので、スプライトのデータは使いません。
		cairo_surface_t* surface = cairo_image_surface_create(
						CAIRO_FORMAT_ARGB32,
						width,
						height
				);
		cairo_t* cairo = cairo_create(surface);
		// コンストラクタで作ったフォントを設定して、
		// 拡大率も設定します。
		this->setupCairo(cairo, fontWidth, factor);

		// フォントをレンダリングするために、移動します。
		cairo_move_to(cairo, x, y);
		// フォントに合わせて、パスを引きます。
		cairo_text_path(cairo, msg.c_str());

		this->setColor(cairo, shadowColor);
		// 引いたパスにそって、ラインを引きます。これが影になります。
		cairo_set_line_width(cairo, ShadowWidth);
		// ここでpreserveを使うことで、次の関数でもフォントに合わせて引いたパスが使えます。
		// ためしにcairo_strokeにしたらどうなるか試してみてください。
		cairo_stroke_preserve(cairo);

		this->setColor(cairo, color);
		// 引いたパスの中身を、setColorした色に合わせて塗りつぶします。
		// これが文字本体になります。
		cairo_fill(cairo);

		cairo_destroy(cairo);
		
		// for debug
		// 描画結果をPNGに保存します。
		cairo_surface_write_to_png(surface, "rendererd.png");

		cairo_surface_destroy(surface);

	}
	//return saccubus::draw::Sprite::Handler<saccubus::draw::Sprite>(spr);
}

}}}
