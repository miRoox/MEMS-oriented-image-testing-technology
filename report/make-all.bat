@echo off

set name=document
set texfile=%name%.tex

call make-clean
:: compile the tex file
xelatex.exe --synctex=-1 %texfile%
:: compile bibliography
biber %name%
:: compile again
xelatex.exe --synctex=-1 %texfile%
:: to do it again for backref
xelatex.exe --synctex=-1 %texfile%
