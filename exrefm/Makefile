NAME=	exrefm
PDF=	$(NAME).pdf
MORE=	$(NAME).more
ROFF=	tmac.e exrefm0.n exrefm1.n exrefm2.n exrefm3.n exrefm4.n exrefm5.n \
	exrefm6.n exrefm7.n exrefm8.n exrefm9.n exrefma.n exrefmb.n exrefmc.n

$(PDF): $(ROFF)
	# Original troff
	#tbl $(ROFF) | troff -ms | dpost | ps2pdf - $@
	# groff
	tbl $(ROFF) | troff -ms | grops | ps2pdf - $@

$(MORE): $(ROFF)
	# Original troff
	#tbl $(ROFF) | nroff -ms | col -x > $@
	# groff
	tbl $(ROFF) | nroff -ms > $@

clean:
	rm -f $(PDF)
