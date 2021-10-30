## Inky
> Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp.

Greenspun’s tenth rule

### Summary
This project is an implementation of Greenspun’s tenth rule. It is not meant to be a full featured Lisp. 

There are a number of small Lisp interpreters available. Two with differing approaches are: [Build our Own Lisp][1] & [Wisp][2].

In the ‘Build Your Own Lisp’ version, there is a special type of S-Expression called the Q-Expression (*quoted* expression). This replaces and is a simplification of the macro system. This does lead to a conceptually neat λ-calculus engine that doesn't require the 'special forms' of other implementations. 
This is achieved at the expense of some non-standard syntax.

The simplest example of Lisp interpreter that I could find is from Peter Novig’s article [How to write a Lisp interpreter in Python][3]

Each of these is an Interpreter that produces results by traversal of an abstract syntax tree. The obvious following step would be to implement the evaluator using a stack machine.

Note:
* The first version v1.0 implemented the ‘special syntax’ for a minimal *Eval* function. v1.1 extend the evaluation routines to deal with ‘special forms’. So flipping between the two versions you can see the trade-off in code complexity.

* Basic functionality is working. Though just proof of concept code, it does support partial function application, higher order functions,  etc. 

* The core functionality being defined within the Lisp prelude itself (i.e. we do not implement everything via builtin functions, but effectively *very loosely* we implement an extended untyped λ-calculus engine and the Lisp prelude builds on that.

### Prelude & Example Output

```lisp
`
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
```


```lisp
λ> filter (lambda (x) (> x 2)) [ -1 0 1 2 3 4]
[3 4]

λ> defun (product xs) (foldl * 1 xs)
lambda:
	formals:(xs)
	body:(foldl * 1 xs)

λ> product [2 2 2]
8

λ> defun (foo x) ( (def (y) 1) (+ x y))
lambda:
	formals:(x)
	body:((def (y) 1) (+ x y))

λ> foo 20
21
; Note 'def' is a shortcut for 'define as global here', 
; the assignment operator '=' is used for local variables (see below).


λ> ; Function with a local variable assigned.
λ> defun (bar x) ( (= y 1) (+ x y))
lambda:
	formals:(x)
	body:((= y 1) (+ x y))

λ> ; check that we don't leak y definition into global scope.
λ> bar 10
11
λ> bar 11
12
λ> y
unbound symbol: y
λ> ; Correct, y should only exist in the scope of the function as a local variable.

λ> ; some list based operations....
λ> def xs [ (+ 1 1) (+ 2 2) (+ 3 3) ]
()
λ> tail xs
[(+ 2 2) (+ 3 3)]

λ> ; Check eval on quoted expression is correct.
λ> eval (tail xs)
[4 6]

λ> ; Correct, xs should still contain the 'code' the quoted (we use [] syntax) expressions.
λ> xs
[(+ 1 1) (+ 2 2) (+ 3 3)]
λ> 

λ> (+ 2 2) (+ 4 4)
[4 8]

λ> defun (inv x) (* -1 x)
lambda:
	formals:(x)
	body:(* -1 x)


λ> map inv [2 4 6 8]
[-2 -4 -6 -8]


λ> ; Slow Finbonacci
λ>  defun (fib n) (if (< n 2) (n) (+  (fib (- n 2)) (fib (- n 1)) ))

λ> fib 10
55
λ> fib 15
610
λ> fib 25
75025
```

### Design

I would summarise this section as justification for always producing a throwaway prototype. Not (as so often happens) starting with an R&D effort and ‘iterating’ in a phoney agile way. 

 If building new systems, I always like to rapidly prototype *but* throwaway the prototype. I think its invaluable part of building new software.

The core design choices made were:

1. Use of a discriminated union to hold S-Expression, i.e.

	```cpp
	std::variant<long,double,std::string,BuiltinFunction,LambdaPtr,ExpressionPtr> var;
	```

	In a language like Haskell we would use a *sum* type. The choice in C++ is either a union type (as above) or an inheritance hierarchy.

	I would probably **not** use `std::variant` again. It isn’t a good alternative to sum types. Intuitively it seems slow. There is reasonable code bloat.  	That said, I’m not convinced the alternative (if we ignore C-style union as just a bit messy) an OO hierarchy is any better. I prefer a somewhat more 		functional approach.

2. The parsing routines are basic. I could have used a combinator library. I have used [FastParse][4] in Scala. I decided to investigate Boost’s Spirit parser.  *I quickly gave up on the idea of using Boost’s Spirit parser.*

3.  The interface to the parser is simple ` Either<Error,ValuePtr> parse(std::string_view in)`. The parser will return a smart pointer to a `Value` that represents the S-Expression or an Error.

	One issue is that this carried over to the evaluation function `Either<Error,ValuePtr> eval(...)` this does mean *runtime errors* are essentially treated as exceptions. 

	* The error type  in the parser should be `ParseError`.
	* The error type in the `eval` function could be `Uncaught 	Exception` or `FatalException`.
	* A runtime error, could be some `ErrorValue` and should be part of the discriminated union. Lisp functions themselves should be able to pattern match say on `Error` representing some error in computation.

4. The `eval` function has 'spliced in' some of the work that should be done by a macro expander. That should be abstracted out to support proper `defmacro` syntax.

5. This is largely ‘throwaway’ code, however very useful as a prototyping exercise and evaluating what would be necessary for a better, full implementation.

6. In this codebase I’ve made no attempt at any tail call optimisation in the `eval`. I think in a better implementation either you would address that (trampolining) or introduce a stack machine rather than AST walking interpreter.

[1]:	https://github.com/orangeduck/BuildYourOwnLisp
[2]:	https://github.com/adam-mcdaniel/wisp
[3]:	https://norvig.com/lispy.html
[4]:	https://github.com/com-lihaoyi/fastparse
