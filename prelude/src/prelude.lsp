def (nil) []
def (true) 1
def (false) 0

; Slightly more standard Lisp syntax.
; Cost is to define some 'special cases' in the eval function
; (as most other implementations do, vs. base case in 'Build Your Own Lisp).

; list functions, map, foldl, etc.
defun (len xs) (if (== xs nil) (0) (+ 1 (len (tail xs))))
defun (fst xs) ( eval (head xs) )
defun (drop n xs) ( if (== n 0) (xs) (drop (- n 1) (tail xs)))
defun (foldl f z xs) (if (== xs nil) [z] (foldl f (f z (fst xs)) (tail xs)))
defun (map f xs) ( (if (== xs nil) (nil) (join (list (f (fst xs))) (map f (tail xs)))))
defun (filter f xs) (if (== xs nil) (nil) (join (if (f (fst xs)) (head xs) (nil)) (filter f (tail xs))))
