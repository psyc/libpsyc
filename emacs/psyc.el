(require 'font-lock)

(setq
 psyc-op "\\([:!?=$@+-]\\)"
 psyc-routing-op "^\\([:=]\\)"
 psyc-state-op "^\\([?=]\\)$"

 psyc-num "\\([0-9]+\\)"
 psyc-kw "\\([a-z_][a-z0-9_]*\\)"

 psyc-length "^[0-9]+$"

 psyc-routing-var-names (regexp-opt '("_amount_fragments" "_context" "_counter"
                                      "_fragment" "_source" "_source_relay"
                                      "_target" "_target_relay"
                                      "_tag" "_tag_relay") 'word)
 psyc-routing-var (concat psyc-routing-op psyc-routing-var-names)

 psyc-types (concat "\\([a-z][a-z0-9]+\\|"
                    (regexp-opt '("_dict" "_list" "_struct"
                                  "_int" "_uint" "_num" "_unum" "_flag" "_amount"
                                  "_date" "_time" "_uniform") nil)
                    "\\)")

 psyc-mod-op (concat "^" psyc-op)
 psyc-mod-name (concat psyc-mod-op psyc-kw) ;2
 psyc-mod-type (concat psyc-mod-op psyc-types "\\b") ;2
 psyc-mod-len (concat psyc-mod-op psyc-kw " " psyc-num) ;3
 psyc-mod-delim (concat "\\(?: " psyc-num "\\)?[\t ]+")
 psyc-mod (concat psyc-mod-op psyc-kw psyc-mod-delim) ;3

 psyc-default-name (concat psyc-mod psyc-kw "[|{]") ;4
 psyc-default-type (concat psyc-mod psyc-types "[_|{]") ;4
; psyc-default-name (concat "[\t ]" psyc-kw "[|{]")
; psyc-default-type (concat "[\t ]" psyc-types "[_|{]")


 psyc-elem-delim "[|{}]"
 psyc-elem-start "[|}]"
 psyc-elem-op (concat psyc-elem-start "\\(=\\)")
 psyc-elem-name (concat psyc-elem-start "=" psyc-kw)
 psyc-elem-type (concat psyc-elem-start "=" psyc-types "\\b")
 psyc-elem-name-delim (concat psyc-elem-name "\\(:\\)") ;2
 psyc-elem-len (concat "\\(?:" psyc-elem-name-delim "\\)?" psyc-num) ;3

 psyc-update-op (concat "^@" psyc-kw psyc-mod-delim ".+ " psyc-op) ;3
 psyc-update-name (concat "^@" psyc-kw psyc-mod-delim ".+ "
                          psyc-op psyc-kw) ;4
 psyc-update-type (concat "^@" psyc-kw psyc-mod-delim ".+ "
                          psyc-op psyc-types "\\b") ;4
 psyc-update-len-delim (concat "^@" psyc-kw psyc-mod-delim ".+ "
                               psyc-op psyc-kw "?\\(:\\)") ;5
 psyc-update-len (concat "^@" psyc-kw psyc-mod-delim ".+ "
                         psyc-op psyc-kw "?:" psyc-num) ;5

 psyc-index-delim "\\(#\\)-?[0-9]+"

 psyc-struct-delim "[\t0-9}]\\(\\.\\)"
 psyc-struct-name (concat psyc-struct-delim psyc-kw) ;2
 psyc-struct-type (concat psyc-struct-delim psyc-types "\\b") ;2


 psyc-dict-key-len (concat "{" psyc-num)
 psyc-dict-key (concat "{\\(?:" psyc-num " \\)?\\([^}]+\\)") ;2

 psyc-method (concat "^_" psyc-kw)
 ;psyc-body "^[^_:!?=$@+-].*$"

 psyc-tmpl-start (concat "\\(\\[\\)" psyc-kw ".+?\\]")
 psyc-tmpl-end (concat "\\[" psyc-kw ".+?\\(\\]\\)")

 psyc-packet-delim "^|$"

 psyc-font-lock-keywords
 `(
   (,psyc-routing-var	. (2 font-lock-keyword-face t))
   (,psyc-length	. (0 font-lock-constant-face))
   (,psyc-state-op	. (0 font-lock-preprocessor-face))
   (,psyc-mod-op	. (1 font-lock-preprocessor-face))
   (,psyc-mod-name	. (2 font-lock-variable-name-face))
   (,psyc-mod-type	. (2 font-lock-type-face t))
   (,psyc-mod-len	. (3 font-lock-constant-face))

   (,psyc-default-name	. (4 font-lock-variable-name-face))
   (,psyc-default-type	. (4 font-lock-type-face t))

   (,psyc-dict-key-len	. (1 font-lock-constant-face))
   (,psyc-dict-key	. (2 font-lock-string-face))

   (,psyc-elem-op	. (1 font-lock-preprocessor-face))
   (,psyc-elem-name	. (1 font-lock-variable-name-face))
   (,psyc-elem-type	. (1 font-lock-type-face t))
   (,psyc-elem-name-delim . (2 font-lock-comment-face))
   (,psyc-elem-len	. (3 font-lock-constant-face))
   (,psyc-elem-delim	. (0 font-lock-keyword-face))

   (,psyc-update-op	. (3 font-lock-preprocessor-face))
   (,psyc-update-name	. (4 font-lock-variable-name-face))
   (,psyc-update-type	. (4 font-lock-type-face t))
   (,psyc-update-len-delim . (5 font-lock-comment-face))
   (,psyc-update-len	. (5 font-lock-constant-face))

   (,psyc-method	. (0 font-lock-function-name-face))
   ;(,psyc-body		. (0 font-lock-comment-face))

   (,psyc-index-delim	. (1 font-lock-comment-face))
   (,psyc-struct-name	. (1 font-lock-comment-face))
   (,psyc-struct-name	. (2 font-lock-variable-name-face))
   (,psyc-struct-type	. (2 font-lock-type-face t))

   (,psyc-tmpl-start	. (1 font-lock-keyword-face))
   (,psyc-tmpl-end	. (2 font-lock-keyword-face))

   (,psyc-packet-delim	. (0 font-lock-preprocessor-face))
   ))

(define-derived-mode psyc-mode fundamental-mode
  "PSYC"
  "Major mode for editing PSYC packets"

  (setq font-lock-defaults '((psyc-font-lock-keywords)))
  (setq show-trailing-whitespace t))

(provide 'psyc)
