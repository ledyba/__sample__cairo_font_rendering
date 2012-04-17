
#include "./SimpleCommentFactory.h"

int main(int argc, char** argv){
	saccubus::draw::sdl::SimpleCommentFactory factory;
	factory.renderComment("おいしいうどんが食べたいな 한글 فارسى‎; Fārsī", 39, 0xffffff, 0x000000, 1);
	return 0;
}

