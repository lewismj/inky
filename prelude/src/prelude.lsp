; prelude
;   basic types and infrastructure in the lisp-ish.

; defun - provide a way of defining functions as named lambda.
def [defun] (\ [args body] [def (head args) (\ (tail args) body)])

; e.g. defun [example-fn x y] [+ x y]

def [nil] []
def [true] 1
def [false] 0
