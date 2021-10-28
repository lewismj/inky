## Inky
> Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp.

Greenspun’s tenth rule

### Summary
This project is an implementation of Greenspun’s tenth rule. It is not meant to be a full featured Lisp. 

There are a number of small Lisp interpreters available. Two with differing approaches are: [Build our Own Lisp][1] & [Wisp][2].

In the ‘Build Your Own Lisp’ version, there is a special type of S-Expression called the Q-Expression (*quoted* expression). This replaces and is a simplification of the macro system.  

If an expression has a type Q-Expression it won’t be eagerly evaluated. *lambda* itself is just a symbol in the global environment, whose value (S-Expression) happens to be a built-in function that is used to construct instances of a `Lambda`.  

There are pros-cons to the Q-Expression approach. It does lead to a very small subset of ‘built-in’ functions required. 

Other implementations tend not to do this, and have the *eval* function treat operations such as *lambda*, *defun*, etc.  as a ’special’ forms. The set of special forms is generally small; but strictly speaking unnecessary (if you adopt syntax to represent ‘quoted expressions’).

The simplest example is from Peter Novig’s [How to write a Lisp interpreter in Python][3]

Each of these is an Interpreter that produces results by traversal of the parser output (abstract syntax tree or equivalent).  The alternative is for
the parser to traverse the abstract syntax tree, producing instructions for an abstract stack (or ‘virtual’) machine. 

Note:
* The first version v1.0 implemented the ‘special syntax’ for a minimal *Eval* function. v1.1 extend the evaluation routines to deal with ‘special forms’. So flipping between the two versions you can see the trade-off in code complexity (albeit I would euphemistically call the code ‘experimental’).

* Basic functionality is working. Though purely as demo/throwaway code; essentially a proof-of-concept or loose specification for a ‘real’ interpreter. It does however support partial application of functions, higher order functions etc.

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

I would definitely **not** use `std::variant` again. It isn’t a good alternative to sum types. It is very slow. There is lots of code bloat. These issues aren’t solved with ‘visitor’ patterns.

2. The parsing routines are basic. I could have used a combinator library. I have used [FastParse][4] in Scala. I decided to investigate Boost’s Spirit parser.  *I quickly gave up on the idea of using Boost’s Spirit parser.*

3.  The interface to the parser is simple ` Either<Error,ValuePtr> parse(std::string_view in)`. The parser will return a smart pointer to a `Value` that represents the S-Expression or an Error.

	One issue is that this carried over to the evaluation function `Either<Error,ValuePtr> eval(...)` this does mean *runtime errors* are essentially treated as exceptions. 

	* The error type  in the parser should be `ParseError`.
	* The error type in the `eval` function could be `Uncaught 	Exception` or `FatalException`.
	* A runtime error, could be some `ErrorValue` and should be part of the discriminated union. Lisp functions themselves should be able to pattern match say on `Error` representing some error in computation.

4. In the first version, I followed the ‘Build Your Own Lisp’ approach of adopting a ‘special syntax’ for Lambda expressions.In v1.1 I made the changes necessary to support more standard syntax.  

	There is a trade off: ‘special forms’ complicate the evaluation. However they simplify the syntax for the user. 
In addition the evaluation also has 'spliced in' some of the work that should be done by a macro expander. That should be abstracted out to support proper `defmacro` syntax.

5. This is largely ‘throwaway’ code, however very useful as a prototyping exercise and evaluating what would be necessary for a better, full implementation.

6. In this codebase I’ve made no attempt at any tail call optimisation.

### Example Output
#### v1.1
```lisp
λ> defun (length xs) (if (== xs nil) (0) (+ 1 (length (tail xs))))
lambda:
	formals:(xs)
	body:(if (== xs nil) (0) (+ 1 (length (tail xs))))

λ> def (nil) []
()
λ> length [a b c d]
4
λ> defun (foo x) (+ 1 x)
lambda:
	formals:(x)
	body:(+ 1 x)

λ> defun (bar f x) (f x)
lambda:
	formals:(f x)
	body:(f x)

λ> bar foo 1
2
λ> ; Using prelude ...
λ> defun (inv x) (* -1 x)
lambda:
	formals:(x)
	body:(* -1 x)

λ> map inv [2 4 6 8]
[-2 -4 -6 -8]
λ> :q
```

[1]:	https://github.com/orangeduck/BuildYourOwnLisp
[2]:	https://github.com/adam-mcdaniel/wisp
[3]:	https://norvig.com/lispy.html
[4]:	https://github.com/com-lihaoyi/fastparse
