; prelude
;   basic types and infrastructure in the lisp-ish.

; defun - provide a way of defining functions as named lambda.
def [defun] (\ [args body] [def (head args) (\ (tail args) body)])

; example function
defun [example-fn x y] [+ x y]
