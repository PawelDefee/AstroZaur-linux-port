all: about.html changes.html help.html lua.html menu.html edit.html

clean:
	rm *.html

.SUFFIXES: .shtml .html .xml .sxml

.shtml.html:
	iconv -f koi8-r -t utf-8 $^ > $@

