## Inky
> Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp.
Greenspun’s tenth rule

### Summary
This project is an implementation of Greenspun’s tenth rule. It is not meant to be a full featured Lisp.

There are a number of small Lisp interpreters available. These two are interesting, they take different approaches.

1. [Build our Own Lisp][1] 2. [Wisp][2]

The interesting difference between these approaches is how they deal with lambda functions and macros.

If you take a simple lambda expression:
```
(lambda (x) (+ x 1))
```

In the ‘Build Your Own Lisp’ version, when evaluating S-Expressions, lambda and other functions are assigned a special ‘type’ so that they aren’t eagerly evaluated’. In that implementation **lambda** is itself a symbol (in global scope) that corresponds to a built-in function that creates a structure for holding an evaluated lambda.

Other implementations tend not to do this, and have the *eval* function treat *lambda* as a ’special’ operation.

The simplest example is from Peter Novig’s [How to write a Lisp interpreter in Python][3]

```python


class Procedure(object):
    "A user-defined Scheme procedure."
    def __init__(self, parms, body, env):
        self.parms, self.body, self.env = parms, body, env
    def __call__(self, *args): 
        return eval(self.body, Env(self.parms, args, self.env))



def eval(x, env=global_env):
    "Evaluate an expression in an environment."
    if isinstance(x, Symbol):      # variable reference
        return env.find(x)[x]
    elif not isinstance(x, List):  # constant literal
        return x                
....
....
....

    else:                          # (proc arg...)
        proc = eval(x[0], env)
        args = [eval(exp, env) for exp in x[1:]]
        return proc(*args)
```

### Design

Central to the design are the following choices

1. Use a discriminated union to hold S-Expression, i.e.
```cpp
std::variant<long,double,std::string,BuiltinFunction,LambdaPtr,ExpressionPtr> var;
```

In a language like Haskell we would use a *sum* type. The choice in C++ is either a union type (as above) or an inheritance hierarchy.

I’m not sure if I would follow this approach if I were building a full implementation. The cost (lots of `std::get` function calls to take a value out of the variant v.s. the benefit - no inheritance hierarchy, smaller codebase, in
theory less memory.

2. Parsing. The parse is *very* basic, it is just dealing with S-Expressions. Having used [FastParse][4] in Scala; I decided to investigate Boost’s Spirit parser.  *I quickly gave up on the idea of using Boost’s Spirit parser.*

	Currently investigating smaller C++ Parser combinators.

3. I’ve followed the ‘Build Your Own Lisp’ approach of ‘special syntax’ for Lambda expressions. If writing this again, I’d maybe use the regular syntax (with modified *eval*).

4. This is largely ‘throwaway’ code, not for a serious project. I’m not sure if the C++ boilerplate is repaid by speed in a proper implementation.  The codebase would be significantly smaller if the implementation were done in a language like Scala. C++ involves *lots* of boilerplate, with no type pattern matching etc.


### Prelude

The ‘Build Your Own Lisp’ approach is interesting as it allows you to bootstrap your environment from a very basic Prelude, for example:

```lisp
def [defun] (\ [args body] [def (head args) (\ (tail args) body)])
```

### Example Output
Here is some output from my C++ solution:
```lisp
λ> def [defun] (\ [args body] [def (head args) (\ (tail args) body)])
()
λ> defun [length xs] [ if (== xs nil) [0] [+ 1 (length (tail xs))]]
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
λ> 
```

[1]:	https://github.com/orangeduck/BuildYourOwnLisp
[2]:	https://github.com/adam-mcdaniel/wisp
[3]:	https://norvig.com/lispy.html
[4]:	https://github.com/com-lihaoyi/fastparse
