; prelude
;   basic types and infrastructure in the lisp.

; defun - provide a way of defining functions as named lambda.
def [defun] (\ [args body] [def (head args) (\ (tail args) body)])

defun [add-me x y] [+ x y]
