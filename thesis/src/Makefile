#################################################################
# Makefile (use GNU-Make)
# ---------------------------------------------------------------
# ---------------------------------------------------------------
# $* file name part of the current dependent with suffix deleted
#    (only for suffix rules)
# $@ full target name
# $< out-of-date module (only for suffix rules)
# $? list of components to be rebuilt
#
#################################################################

.DEFAULT:
	@echo "This target needs remaking: default = [$<]"
	@echo "Do you really use GNU-Make?"

MAINFILE	:= thesis
TITLEPAGE	:= titelseite
TEXSOURCES	:= $(wildcard *.tex)
BIBSRCFILES	:= $(wildcard *.bib)
BBLFILES	:= $(subst bib,bbl,$(BIBSRCFILES))
AUXFILES	:= $(subst tex,aux,$(TEXSOURCES))
SCRIPTDIR	:= scripts

SOURCES = thesis.tex macros.tex Makefile

# FIGS = pics/fig*.fig

# PICS = pics/*.eps

# FOLIEN = folien/dafolien.tex

$(MAINFILE).dvi: $(MAINFILE).tex  \
		wissdoc.cls $(TEXSOURCES) $(BBLFILES)
	- makeindex -s thesis.ist thesis.idx
	- latex $(MAINFILE)

it:
	@rm -f $(MAINFILE).dvi
	- latex $(MAINFILE)

#$(AUXFILES): %.aux: %.tex
#             latex $(MAINFILE)

$(BBLFILES): %.bbl: %.bib
	- pdflatex -shell-escape $(MAINFILE)
	biber $(MAINFILE)

final: it bbls double index ps

double: $(MAINFILE).tex
	- latex $(MAINFILE)
	- latex $(MAINFILE)

complete: $(MAINFILE).dvi
	- latex $(MAINFILE)
	- latex $(MAINFILE)

index: $(MAINFILE).ind

#makeindex kann keine 8-bit Umlaute richtig sortieren! :-(
#deswegen setzen wir sie für makeindex einfach nochmal um
$(MAINFILE).ind: $(MAINFILE).idx $(MAINFILE).ist
	- mv $(MAINFILE).idx $(MAINFILE).idx.orig
	- $(SCRIPTDIR)/recodetexumlaut <$(MAINFILE).idx.orig >$(MAINFILE).idx
	- makeindex -s $(MAINFILE).ist $(MAINFILE)
	- mv $(MAINFILE).idx.orig $(MAINFILE).idx

ps: $(MAINFILE).ps

$(MAINFILE).ps:	$(MAINFILE).dvi
	dvips -K -o $(MAINFILE).ps $(MAINFILE).dvi


#Für ein vernünftiges PDF wird benötigt:
# pdftex >=0.14e
# pdftex.def >=v0.02r
# hyperref.sty >= 6.69
# thumbpdf >=1.11
# ghostscript 6.50
finalpdf: pdf
	- thumbpdf $(MAINFILE)
	- pdflatex -shell-escape $(MAINFILE)

pdf:    $(MAINFILE).pdf  $(MAINFILE).ind

onepdf: 
	- pdflatex -shell-escape $(MAINFILE)

$(MAINFILE).pdf: $(MAINFILE).tex $(TEXSOURCES)
	- rm $(MAINFILE).aux $(MAINFILE).ind
	- pdflatex -shell-escape $(MAINFILE)
	- make bbls
	- makeglossaries $(MAINFILE)
	- pdflatex -shell-escape $(MAINFILE)
	- $(MAKE) index
	- pdflatex -shell-escape $(MAINFILE)


bbls: $(BBLFILES)


rmbbls:
	- rm $(BBLFILES)


view: $(MAINFILE).dvi
	xdvi $@ &

zipps: $(MAINFILE).ps
	gzip -p $(MAINFILE).ps

folien: folien/dafolien.dvi
	(cd folien; dvips -K -t a4 -t landscape dafolien)

folien/dafolien.dvi: $(FOLIEN)
	(cd folien; latex dafolien)

viewf: 
	(cd folien; xdvi -paper a4r dafolien) &

final: 
	dvips -K -t a4 diplarb

clean:
	- find . | egrep ".*((\.(aux|idx|ind|ilg|log|blg|bbl|toc|lof|lot|dvi|tlf|tlt))|~)$$" | xargs rm
	- rm $(MAINFILE).out thumb???.png thumbdta.tex thumbpdf.*

archive:
	- @ echo "Making archive..."
	- gtar -czvf $(MAINFILE).tar.gz $(SOURCES)  pics/*.ps pics/*.eps Makefile 
	- @ echo "Archive ready."

save:
	- @ echo -n "Copying to DOS disk..."
	@ mcopy -no $(MAINFILE).tar.gz A:$(MAINFILE).tgz
	- @ echo "ready."


pics/%.eps: pics/fig/%.fig
	fig2dev -L ps $< $@
#end of Makefile


