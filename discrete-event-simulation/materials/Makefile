doit: $(wildcard *.tex)
	mkdir -p out
	pdflatex --output-directory=out system-description.tex
	cp out/system-description.pdf system-description.pdf
	open system-description.pdf

.PHONY: clean
clean:
	rm -f system-description.pdf
	rm -rf out/

