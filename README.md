## Inky
> Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp.

Greenspun’s tenth rule

### Summary
This project is an implementation of Greenspun’s tenth rule. It is not meant to be a full featured Lisp. 

There are a number of small Lisp interpreters available. Two with differing approaches are: [Build our Own Lisp][1] & [Wisp][2].

In the ‘Build Your Own Lisp’ version, there is a special type of S-Expression called the Q-Expression (*quoted* expression). This replaces and is a simplification of the macro system. This does lead to a conceptually neat λ-calculus engine that doesn't require the 'special forms' of other implementations. This is achieved at the expense of some non-standard syntax.

The simplest example of Lisp interpreter that I could find is from Peter Novig’s article [How to write a Lisp interpreter in Python][3]

Each of these is an Interpreter that produces results by traversal of an abstract syntax tree. The obvious following step would be to implement the evaluator using a stack machine.

Note:
* The solution started *v1.0* implementing a special syntax `[expr]` to represent quoted expressions. These are expressions that aren’t eagerly evaluated. In *v1.2* onward that is dropped and more conventional Lisp syntax adopted (at the expense of some complications in the `eval` function). A proper Lisp *should* have a macro expansion table. This hasn’t been implemented.

* Basic functionality is working. Just proof of concept code, it does support partial function application, higher order functions,  etc.

* The core functionality is defined within the Lisp prelude itself (i.e. we do not implement everything via builtin functions). Effectively this implements an extended untyped λ-calculus engine (*sort of*) and the Lisp prelude builds on that.

### Usage

#### Prelude
The builtin functions provide the basic `head`, `tail`, `join`, `eval` etc. functions. However the language constructs themselves should generally be built in the language from these builtin functions.

```lisp
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

#### Simple list and other expressions
```lisp
λ> filter (lambda (x) (> x 2)) [-1 0 1 2 3 4]
[3 4]
 
λ> defun (product xs) (foldl * 1 xs)
	lambda:
       formals:(xs)
       body:(foldl * 1 xs)
   
λ> product [2 2 2]
8

λ> defun (inv x) (* -1 x)
  lambda:
      formals:(x)
      body:(* -1 x)
  
  
λ> map inv [2 4 6 8]
[-2 -4 -6 -8]

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
```

#### Variable scope
```lisp
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
```

#### Partial & Higher order functions
Since a Lambda has an environment scope of its own, we can easily support partial function application. Example:

```lisp
λ> defun (add x y) (+ x y)
lambda:
	formals:(x y)
	body:(+ x y)

λ> (add 2 3)
5

; N.B. We are NOT using the 'defun' syntax here.
; 'add' is a lambda that has 'add' as its associated symbol.
; Here we're defining 'plusOne' symbol the result of (add 1)
; which is a partially applied function. 
λ> def (plusOne) (add 1)
()
λ> plusOne
lambda:
	formals:(y)
	body:(+ x y)

λ> plusOne 1
2
λ> plusOne 2
3

λ> defun (adder x) (+ 1 x)
lambda:
	formals:(x)
	body:(+ 1 x)

λ> defun (identity f x) (f x)
lambda:
	formals:(f x)
	body:(f x)
λ> identity adder 1
2
```

### Design

I would summarise this section as justification for always producing a throwaway prototype. I like to rapidly prototype *but* throwaway that prototype. I think it is an invaluable exercise.

In prototyping there are a few design decisions that I would re-visit; to establish if I would make the same choices for a *real* implementation, these are:

1. Use of a discriminated union to hold S-Expression, i.e.

	```cpp
	std::variant<long,double,std::string,BuiltinFunction,LambdaPtr,ExpressionPtr> var;
	```

	In a language like Haskell we would use a *sum* type. The choice in C++ is either a union type (as above) or an inheritance hierarchy.

	I would probably not use `std::variant` again. It isn’t a good alternative to sum types. Intuitively it seems slow.  It introduces some boilerplate that complicates some implementation details. 

	I’m not convinced the alternative (to variant/union): an object oriented hierarchy would ultimately produce a cleaner design.

2. The parsing routines are basic. I could have used a combinator library. I have used [FastParse][4] in Scala. I decided to investigate Boost’s Spirit parser.  *I  gave up on using Boost’s Spirit parser.*
	Probably worth investigating combinator alternatives to Spirit. I would **not** use Boost Spirit.

3. The `eval` function has 'spliced in' some of the work that should be done by a macro expander. That should be abstracted out to support proper `defmacro` syntax.

4. In this codebase I’ve made no attempt at any tail call optimisation in the `eval`. I think in a better implementation either you would address that (trampolining) or introduce a stack machine rather than AST walking interpreter.


### How does evaluation work?

Suppose we have an expression of the type:
```lisp
(lambda(x) (+ x (+ 2 3)) ) (+ 1 2)
```

We would expect the result **8**
`(+ 1 2)` evaluates to 3
`(+ 2 3)` evaluates to 5

Finally, in the function call 3 is supplied for ‘x’, so we are left with `(+ 3 5)`

The algorithm for evaluating expressions is simple,

1. If we have a primitive type it evaluates as itself, i.e. 4 evaluates to 4.
	Quoted expressions also just evaluate to themselves, the `eval` built-in is used to evaluate the quoted expression.  It simply flips its type kind to S-Expression.

2. Given an S-Expression:
	1. Loop over all the sub-expressions and reduced them if we can.

		So in the above S-Expression:
		1. We don’t evaluate the argument *x* until the function is invoked.
		2. Evaluate `(+ 2 3)` and `(+ 1 2)` 

			The result of looping over the sub-expressions would be:

			```lisp
			(lambda(x) (+ x 5)) 3		
			```

	2. Once sub-expressions have been reduced (evaluated) then evaluate function calls; or return the list of results if no functions calls are made.

	When evaluating an S-Expression, the key thing is to reduce the expressions first. However, don’t try to reduce things like function arguments as these won’t be bound until invoked.

	The implementation `eval.cpp` shows that we skip ahead in the loop if we encounter things like a function definition. 


### Background

*The structure and Interpretation of Computer Programs* by Harold Abelson and Gerald Jay Sussman with Julie Sussman.

I highly recommend this book. A great insight into Lisp/Scheme and how you would start building your own interpreters
and compilers.

 I've not (yet) found a more modern reference on compilers that matches *The Dragon Book*.


![][image-1]

[1]:	https://github.com/orangeduck/BuildYourOwnLisp
[2]:	https://github.com/adam-mcdaniel/wisp
[3]:	https://norvig.com/lispy.html
[4]:	https://github.com/com-lihaoyi/fastparse

[image-1]:	https://github.com/lewismj/inky/blob/main/doc/sicp.jpg