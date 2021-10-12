## Inky 

## Summary

"Any sufficiently complicated C or Fortran program contains an ad hoc, informally-specified, bug-ridden, slow implementation of half of Common Lisp"


WIP - need to add builtin for if etc.. and define a proper prelude...

![icon](https://github.com/lewismj/inky/blob/main/doc/resources/img/inky.png) 

This is a throwaway project proof of concept. Interpreter for Common Lisp (like) language.

Lots of other project have done similar:

https://github.com/orangeduck/BuildYourOwnLisp 
https://github.com/kanaka/mal
https://github.com/adam-mcdaniel/wisp


Mainly done as a prototype. Even the better Lisp interpreters basically just improve on:

- parsing (using combinators, I looked at Spirit, will look for something else ...)
- repl (using editline etc., Will evaluate for a 'proper' implementation)

For eval, I've seen lots of interpreters for LISP. But none that follow:

https://www.microsoft.com/en-us/research/wp-content/uploads/1987/01/slpj-book-1987-small.pdf

i.e. Build an a Lambda Calculus engine (with the reductions/optimisations) and compiler/interpreter for that.
Then convert the AST to the LC for evaluation.

So a 'real' implementation to follow that would take longer.


## Dependencies

- {fmt}
- Catch2
