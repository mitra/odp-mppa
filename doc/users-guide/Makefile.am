TARGET = $(top_builddir)/doc/output/users-guide.html

EXTRA_DIST = users-guide.adoc

all-local: $(TARGET)

$(TARGET): users-guide.adoc
	@mkdir -p $(top_builddir)/doc/output
	asciidoc -a data-uri -b html5  -a icons -a toc2  -a max-width=55em --out-file=$@ $< || asciidoc -a data-uri -b html4  -a icons -a toc2  -a max-width=55em --out-file=$@ $<

clean-local:
	rm -f $(TARGET)
