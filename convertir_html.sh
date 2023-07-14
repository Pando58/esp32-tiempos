echo "const char HTML_INDEX[] PROGMEM = R\"=====(" > HtmlIndex.h
cat ui/dist/index.html >> HtmlIndex.h
echo ")=====\";" >> HtmlIndex.h
