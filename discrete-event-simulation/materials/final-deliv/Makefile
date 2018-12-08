doit: $(wildcard *.tex)
	mkdir -p out
	pdflatex --output-directory=out final-deliv.tex
	cp out/final-deliv.pdf final-deliv.pdf
	open final-deliv.pdf

.PHONY: clean
clean:
	rm -f final-deliv.pdf
	rm -rf out/

