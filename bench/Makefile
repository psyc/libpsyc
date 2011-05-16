ORG_PATH = /usr/share/emacs/site-lisp/org-mode
INIT = (setq load-path (cons \"/usr/share/emacs/site-lisp/org-mode\" load-path) \
             make-backup-files nil \
             org-export-author-info nil \
             org-export-creator-info nil \
             org-export-html-validation-link nil \
             org-babel-tangle-pad-newline nil \
             org-src-preserve-indentation t) \
       (require 'org-install)

ORG = benchmark.org results.org

wiki2org:
	perl -pe '\
		s/^= (.*) =\s*$$/#+TITLE: $$1\n/; \
		s/^== (.*) ==\s*$$/* $$1/; \
		s/^=== (.*) ===\s*$$/** $$1/; \
		s/^{{{/#+BEGIN_SRC/; \
		s/^}}}/#+END_SRC/ \
	' benchmark.wiki >benchmark.org

packets:
	emacs -Q --batch --eval \
		"(progn ${INIT} (find-file \"benchmark.org\") \
			(org-babel-tangle) (kill-buffer))"
	perl -pi -e 'print "\n" unless $$p; $$p=1' packets/user_profile.psyc

html:
	for f in ${ORG}; do \
		emacs -Q --batch --eval \
			"(progn ${INIT} (find-file \"$$f\") \
			  (org-export-as-html-batch) (kill-buffer))"; \
	done

pdf:
	for f in ${ORG}; do \
		emacs -Q --batch --eval \
			"(progn ${INIT} (find-file \"$@.org\") \
			  (org-export-as-pdf org-export-headline-levels) (kill-buffer))"
