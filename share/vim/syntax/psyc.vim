" Vim syntax file
" Language: PSYC - syntax highlighting for protocol packets
" Maintainer: carlo von lynX
" Latest Revision: 2016-08-25

if exists("b:current_syntax")
  finish
endif

" PSYC2 syntax
syn match psycSeparator /^|$/

" PSYC1 syntax
syn match psycSeparator /^\.$/

" PSYC operators
syn match psycSet /^:[a-z_].*$/
syn match psycAssign /^=[a-z_].*$/
syn match psycAugment /^+[a-z_].*$/
" why doesn't this one work?
syn match psycDiminsh /^\-[a-z_].*$/

" should define a region instead, so that header rules
" are not applied to body
syn match psycMethod /^[a-z_]\+$/

hi def link psycSeparator PreProc
hi def link psycMethod Statement
hi def link psycAssign Constant
hi def link psycSet Identifier
hi def link psycAugment Type
hi def link psycDiminish Type

let b:current_syntax = "psyc"
