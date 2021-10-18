## Inky
> Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp.
Greenspun’s tenth rule

### Summary
This project is an implementation of Greenspun’s tenth rule. It is not meant to be a full featured Lisp.

There are a number of small Lisp interpreters available, two that I came across are interesting as they take different approaches.

1. [Build our Own Lisp][1]
2. [Wisp][2]

The interesting difference between these approaches is how they deal with lambda functions.

Typically, we have
```
(lambda (x) (+ x 1))
```

The question is how do evaluate it? In the ‘Build Your Own Lisp’ version, *lambda* is just a symbol in the environment who’s lookup is formals and body (which have a ‘quoted’ type to ensure they aren’t eagerly evaluated).

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

### Prelude

The ‘Build Your Own Lisp’ approach is interesting as it allows you to bootstrap your environment from a very basic Prelude, for example:

```lisp
def [defun] (\ [args body] [def (head args) (\ (tail args) body)])
```

[1]:	https://github.com/orangeduck/BuildYourOwnLisp
[2]:	https://github.com/adam-mcdaniel/wisp
[3]:	https://norvig.com/lispy.html