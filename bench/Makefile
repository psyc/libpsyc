ORG_PATH = /usr/share/emacs/site-lisp/org-mode
INIT = (setq load-path (cons \"/usr/share/emacs/site-lisp/org-mode\" load-path)) (require 'org-install)

wiki2org:
	perl -pe '\
		s/^= (.*) =\s*$$/#+TITLE: $$1\n/; \
		s/^== (.*) ==\s*$$/* $$1/; \
		s/^=== (.*) ===\s*$$/** $$1/; \
		s/^{{{/#+BEGIN_SRC/; \
		s/^}}}/#+END_SRC/ \
	' benchmark.wiki >benchmark.org

tangle:
	emacs -Q --batch --eval \
		"(progn ${INIT} (find-file \"benchmark.org\") \
			(setq org-babel-tangle-pad-newline nil org-src-preserve-indentation t) \
			(org-babel-tangle) (kill-buffer))"
	perl -pi -e 'print "\n" unless $$p; $$p=1' packets/user_profile.psyc

html:
	emacs -Q --batch --eval \
		"(progn ${INIT} (find-file \"benchmark.org\") \
			(org-export-as-html-batch) (kill-buffer))"

pdf:
	emacs -Q --batch --eval \
		"(progn ${INIT} (find-file \"benchmark.org\") \
			(org-export-as-pdf org-export-headline-levels) (kill-buffer))"
