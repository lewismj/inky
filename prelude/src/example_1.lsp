def [defun] (lambda [args body] [def (head args) (lambda (tail args) body)])
def [nil] []
def [true] 1
def [false] 0
defun [length xs] [if (== xs nil) [0] [+ 1 (length (tail xs))]]
length [a b c d]

