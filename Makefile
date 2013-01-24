testa:
	clang example/*.c src/*.c src/qtkit/*.m -o test -Isrc/ -DQTKIT_INPUT -framework QTKit -framework Foundation -framework QuartzCore