## Inky
> Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp.
Greenspun’s tenth rule

### Summary
This project is an implementation of Greenspun’s tenth rule. It is not meant to be a full featured Lisp. 

There are a number of small Lisp interpreters available. These two are interesting, they take different approaches.

* [Build our Own Lisp][1] 
* [Wisp][2]

The interesting difference between these approaches is how they deal with lambda functions and macros.

In the ‘Build Your Own Lisp’ version, there is a special type of S-Expression called the Q-Expression (*quoted* expression). This replaces and is a simplification of the macro system.  

If an expression has a type Q-Expression it won’t be eagerly evaluated. *lambda* itself is just a symbol in the global environment, whose value (S-Expression) happens to be a built-in function that is used to construct ‘Lambda’ structures.  

There are pros-cons to the Q-Expression approach. It does lead to a very small subset of ‘built-in’ functions required( ‘*homoiconicity*’).  

Other implementations tend not to do this, and have the *eval* function treat *lambda* as a ’special’ operation.

The simplest example is from Peter Novig’s [How to write a Lisp interpreter in Python][3]

Note:

* Looking at the designs, the simplicity of ‘Build Your Own Lisp’ and having a very small set of builtin functions that are essential (*def* and *lambda*) is conceptually very clean. This was implemented for **v1.0**.

* Fundamentally the design of ‘Build Your Own Lisp’ v.s. other designs is one that easily allows for partial function application. I decided to follow a similar design and extend it with special forms necessary to provide a more usual Lisp syntax, **v1.1**

* Basic functionality is working. Though purely as demo/throwaway code; essentially a proof-of-concept or loose specification for a ‘real’ interpreter.	
```lisp
λ> filter (lambda (x) (> x 2)) [ -1 0 1 2 3 4]
[3 4]

λ> defun (product xs) (foldl * 1 xs)

λ> product [2 2 2]
8
λ> filter (lambda (x) (> x 2)) [ -1 0 1 2 3 4]
[3 4]

λ> defun (inv x) (* -1 x)
lambda:
	formals:(x)
	body:(* -1 x)

λ> map inv [2 4 6 8]
[-2 -4 -6 -8]

λ>  defun (fib n)  (if (== n 0) (0) (if (== n 1) (1) ((+ (fib (- n 2)) (fib (- n 1))))))
lambda:
	formals:(n)
	body:(if (== n 0) (0) (if (== n 1) (1) ((+ (fib (- n 2)) (fib (- n 1))))))

λ> fib 10
55
λ> fib 15
610
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

I would definitely *not* use `std::variant` again. A poor alternative to sum types. The code bloat with `std::get` is horrific. Manual pattern matching on type (that I’m not convinced can be got rid of by sprinkling in a Visitor pattern.
Very, very slow.

2. The parsing routines are basic. I could have used a combinator library. I have used [FastParse][4] in Scala. I decided to investigate Boost’s Spirit parser.  *I quickly gave up on the idea of using Boost’s Spirit parser.*

3.  The interface to the parser is simple ` Either<Error,ValuePtr> parse(std::string_view in)`. The parser will return a smart pointer to a `Value` that represents the S-Expression or an Error.
	One issue is that this carried over to the evaluation function `Either<Error,ValuePtr> eval(EnvironmentPtr env, ValuePtr val)` this does mean *runtime errors* are essentially treated as exceptions. The variant above should really contain an `Error` type (separate to Parse Error) so that there is a distinction between exception and error.

4. I followed the ‘Build Your Own Lisp’ approach of adopting a ‘special syntax’ for Lambda expressions. In the first version v1.0. In v1.1 I made minimal changes necessary to support more standard syntax.  There is a trade off, the ‘special forms’ complicate the evaluation. 

5. This is largely ‘throwaway’ code, not for a serious project. I’m not sure if the C++ boilerplate is repaid by speed in a proper implementation. **Actually it isn’t.**  There is not too much code, but would be less if done using say Scala. 

6. In this codebase I’ve made no attempt at any tail call optimisation.

### Example Output
#### v1.1
```lisp
`
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
λ> filter (lambda (x) (> x 2)) [ -1 0 1 2 3 4]
[3 4]
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