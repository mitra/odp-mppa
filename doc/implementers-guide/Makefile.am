TARGET = $(top_builddir)/doc/output/implementers-guide.html

EXTRA_DIST = implementers-guide.adoc

all-local: $(TARGET)

$(TARGET): implementers-guide.adoc
	@mkdir -p $(top_builddir)/doc/output
	asciidoc -a data-uri -b html5  -a icons -a toc2  -a max-width=55em --out-file=$@ $< || asciidoc -a data-uri -b html4  -a icons -a toc2  -a max-width=55em --out-file=$@ $<

clean-local:
	rm -f $(TARGET)
