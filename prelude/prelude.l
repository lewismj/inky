; prelude
;   basic types and infrastructure in the lisp.

; defun - provide a way of defining functions as named lambda.
def [defun] (\ [args body] [def (head args) (\ (tail args) body)])

; test case failure
defun [add-me x y] [+ x y]
; the above alters env for 'defun' not defined properly.


; error when trying to define subsequent fn.
defun [plus-me x y] [+ x y] 
