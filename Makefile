EXT_CFLAGS=`pkg-config cairo freetype2 --cflags`
EXT_LDFLAGS=`pkg-config cairo freetype2 --libs`

cairo_test: main.cpp SimpleCommentFactory.cpp
	g++ \
	-o cairo_test main.cpp SimpleCommentFactory.cpp \
	$(EXT_CFLAGS) $(EXT_LDFLAGS) $(EXT_LIBS)
