

.PHONY : build

install : 
	bash scripts/install.sh

build : 
	bash scripts/build.sh

display : 
	bash scripts/virtual_display.sh