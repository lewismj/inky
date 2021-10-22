; empty list, true & false.

def (nil) []
def (true) 1
def (false) 0

; list functions, map, foldl, etc.
defun (len xs) (if (== xs nil) (0) (+ 1 (len (tail xs))))
defun (fst xs) ( eval (head xs) )
defun (drop n xs) ( if (== n 0) (xs) (drop (- n 1) (tail xs)))
defun (foldl f z xs) (if (== xs nil) [z] (foldl f (f z (fst xs)) (tail xs)))
defun (map f xs) ( (if (== xs nil) (nil) (join (list (f (fst xs))) (map f (tail xs)))))
defun (filter f xs) (if (== xs nil) (nil) (join (if (f (fst xs)) (head xs) (nil)) (filter f (tail xs))))

; definitions for conditional functions.
defun (pack f & xs) (f xs)
def (uncurry) pack
defun (unpack f xs) (eval (join (list f) xs))
def (curry) unpack

