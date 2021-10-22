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

* Slightly uglier (but somewhat follows other implementations) In **v1.1** I deal with ‘special cases’ in the evaluation function. This enables a conventional syntax. For example, `lambda (x) (+ 1 x)` rather than `lambda [x] [+ 1 x]`.

### Design

The core design choices made were:

1. Use of a discriminated union to hold S-Expression, i.e.
```cpp
std::variant<long,double,std::string,BuiltinFunction,LambdaPtr,ExpressionPtr> var;
```

In a language like Haskell we would use a *sum* type. The choice in C++ is either a union type (as above) or an inheritance hierarchy.

I’m not sure if I would follow this approach if I were building a full implementation. The cost (lots of `std::get` function calls to take a value out of the variant v.s. the benefit - no inheritance hierarchy, smaller codebase, in
theory less memory.

2. The parsing routines are basic. I could have used a combinator library. I have used [FastParse][4] in Scala. I decided to investigate Boost’s Spirit parser.  *I quickly gave up on the idea of using Boost’s Spirit parser.*

3. I followed the ‘Build Your Own Lisp’ approach of adopting a ‘special syntax’ for Lambda expressions. In the first version v1.0. In v1.1 I made minimal changes necessary to support more standard syntax. As other implementations, it just means adding ‘special cases’ into the evaluation function.

4. This is largely ‘throwaway’ code, not for a serious project. I’m not sure if the C++ boilerplate is repaid by speed in a proper implementation.  There is not too much code, but would be less if done using say Scala.

5. In this codebase I’ve made no attempt at any tail call optimisation.

### Prelude

#### v1.0
The ‘Build Your Own Lisp’ approach is interesting as it allows you to bootstrap your environment from a very basic Prelude, for example:

```lisp
def [defun] (\ [args body] [def (head args) (\ (tail args) body)])
```

#### v1.1
We define function *defun* in the evaluator itself, as so the expansion:
`defun (foo x y) (+ x y)` to `def (foo) (lambda (x y) (+ x y)`,
is implemented directly.

### Example Output

#### v1.0
```lisp
λ> def [defun] (\ [args body] [def (head args) (\ (tail args) body)])
()
λ> def [nil] []
()
λ> def [true] 1
()
λ> def [false] 0
()
λ> defun [length xs] [if (== xs nil) [0] [+ 1 (length (tail xs))]]
()
λ> length [a b c d]
4
λ> (+ (* 3 (+ (* 2 4) (+ 3 5))) (+ (- 10 7) 6))
57
λ> (def [foo] (lambda [x] [+ x 1]))
()
λ> (def [bar] (lambda [f x] [f x]))
()
λ> bar foo 1
2
λ> defun [fst xs] [ eval (head xs) ]
()
λ> defun [foldl f z l] [if (== l nil) [z] [foldl f (f z (fst l)) (tail l)]]
()
λ> defun [sum l] [foldl + 0 l]
()
λ> sum [1 2 3 4 5 6 7 8 9 10]
55
λ> :q
```

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