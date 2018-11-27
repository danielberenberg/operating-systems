doit: $(wildcard *.tex)
	mkdir -p out
	pdflatex --output-directory=out planned-experiments.tex
	cp out/planned-experiments.pdf planned-experiments.pdf
	open planned-experiments.pdf

.PHONY: clean
clean:
	rm -f planned-experiments.pdf
	rm -rf out/

