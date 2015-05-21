#!/bin/sh

latex2html main.tex && makeglossaries main && latex2html main.tex
pdflatex main.tex && makeglossaries main && pdflatex main.tex
