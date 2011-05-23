#!/bin/sh
dot uml.txt -Tpdf -ouml-fdp.pdf -Kfdp
dot uml.txt -Tpdf -ouml-dot.pdf -Kdot
