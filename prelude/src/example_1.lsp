; examples
defun (inv x) (* -1 x)
defun (product xs) (foldl * 1 xs)

;
map (lambda (x) (+ 1 x)) [ 2 3 4 6]
map inv [2 4 6 8]
filter (lambda (x) (> x 2)) [ -1 0 1 2 3 4]
product [2 2 2]
