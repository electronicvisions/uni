all: vision-bibtex-checkout main.pdf vision-bibtex-check
main.pdf:
main.ps:

# bibliography stuff
vision-bibtex-check: vision-bibtex-checkout
	-@mr -c .mrconfig -d vision-bibtex -q status -s | grep -q -v '^$$' && echo "########### VISIONBIB ############\nPlease commit & push your changes!\n########### VISIONBIB ############"; true
vision-bibtex-checkout:
	-@mr -c .mrconfig checkout
vision-bibtex-update:
	-@mr -c .mrconfig update


include LaTeX.mk

.PHONY: vision-bibtex-check vision-bibtex-update
