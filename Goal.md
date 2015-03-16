# Introduction #

The main goal of this work is to implement a functioning concurrent garbage collector (GC) modeled after the [algorithm proposed by Dijkstra](http://www.cs.utexas.edu/users/EWD/transcriptions/EWD04xx/EWD492.html). A secondary goal is to compare the GC with the two-space algorithm for efficiency.

A byproduct of this effort will be a [(toy) programming language](SinaLanguage.md) and a runtime to go with it (the SinaVM).

# Context #

This project is a school project I am writing as part of my studies at the [Hochschule für Technik Zürich](http://www.hsz-t.ch). I will be finished before the end of November 2007. Please don't contribute code before that as that could be seen as cheating :)

# Details #

To achieve these goals, I will need to:

  * set up a project home (source control, status, documentation etc.)
  * define a language and it's runtime to work in conjunction with the GC.
  * [define a problem to be solved by this language](CesarShift.md), with which I can test the two algorithms.
  * implement the (toy) language
    * implement a parser for the language
    * implement a runtime for the language (interpreter)
    * this should be designed so that the GC can be swapped easily.
  * implement the two algorithms
    * locking will be a big problem with the concurrent GC
    * I expect the two-space algorithm to be rather trivial to implement but the design will have to be done carefully since the two-space algorithm won't need the locking and unlocking used by the concurrent GC.
  * implement the problem in the toy language.
  * benchmark the problem with both GC algorithms and analyze the results.