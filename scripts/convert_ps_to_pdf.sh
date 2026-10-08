epstopdf ./output/temp/out.eps ./output/out.pdf
# gs -sOutputFile=./output/out.pdf -dNOPAUSE -dBATCH -dSAFER -sDEVICE=pdfwrite -dPDFFitPage ./output/out.eps