def (nil) []
def (true) 1
def (false) 0

; Slightly more standard Lisp syntax.
; Cost is to define some 'special cases' in the eval function
; (as most other implementations do, vs. base case in 'Build Your Own Lisp).

defun (length xs) (if (== xs nil) (0) (+ 1 (length (tail xs))))
