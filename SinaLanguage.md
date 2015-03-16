# Introduction #

The Sina programming language uses reverse polish notation (RPN) to keep syntax as simple as possible. At the same time it also adds notation for creating lists and blocks. I won't be implementing namespaces (this is the feature, or lack thereof, that makes it a toy programming language in my eyes).

Sina is a typed language. Each piece of memory (object) has an associated type.

# Types #

  * INTEGER -- an integer (Platforms notion of an int in C)
  * LIST\_NODE -- a node in a list
  * LIST\_HEAD -- maintains information about start and end of a list
  * BLOCK -- the head of a block (references a list, knows current position in list)
  * NATIVE -- a pointer to native code (for built in functions)
  * SYMBOL -- a symbol in the program. Identifiers are translated into symbols. Symbols can be dynamically bound to other objects.
  * ESCAPED\_SYMBOL -- like a symbol, but with slightly modified semantics.


"Strings" are represented as lists of INTEGERSs. There are built in functions to output a "string" and to read a string from input into a list.


# Syntax #

A Sina program consists of a sequence of literals, words, lists and blocks.

## Literals ##

  * An integer (e.g. 10, 1, 0, -245)
  * string literals are converted to lists of integers. A string literal is enclosed in double quotes. There is no way to escape double quotes inside a string literal. This is, after all, a toy language.
  * lists are represented by parentheses, list elements are listed inside, separated by whitespace:
    * () -- empty list
    * (1 2 3) -- list with 3 INT elements
    * ( ("hello" 1) ("world" 2) ) -- nested list
  * blocks are represented by curly braces, elements inside are separated by whitespace

# Semantics #

Parsing Sina source code results in a list of typed "objects". The [SinaVM](SinaVM.md) then executes the program by evaluating each object in the list according to its type. The default evaluation of an object is to push it onto the data stack (DS).

Evaluation of a SYMBOL type is special: A symbol is evaluated by looking up the object it refers to and evaluating that instead. Evaluating a symbol that is not bound to an object is an error. This mode of evaluating a symbol can be suppressed by "escaping" the symbol (prefixing the symbol with a colon). This pushes the symbol itself onto the DS.

| **type** | **execution behavior**  |
|:---------|:------------------------|
| INTEGER  | push DS |
| LIST\_HEAD | push DS |
| ESCAPED\_SYMBOL  | push DS |
| BLOCK | push DS |
| NATIVE | execute function |
| SYMBOL | look up symbol, if it is a BLOCK, then push it onto CS, next object to be executed is first element in BLOCK. Else interpret according to type. |


# Example program #

```
"hello, what is your name?" print-line
read-line :name bind
"hello, " name list-concat print-line
```