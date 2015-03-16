# Introduction #

To be able to test the two GCs, I need a problem that generates a lot of objects and that can be easily scaled to test heavy loads. Ideally, the execution of the test program varies depending on the input data.

This can be achieved by implementing a unix style filter.

# Cesar Shift #

A cesar shift "encrypts" a plaintext by shifting each character in the input alphabet by a constant value, modulo the size of the alphabet. Thus the name "HAL" in space odyssey 2001 is a cesar shift of -1 on the plaintext "IBM" (or so rumor has it.)

This problem is ideal for testing the GCs, since the input data (the plaintext) can be as long as needed. Benchmarking the two GC algorithms can be reduced to timing the execution of the cesar shift algorithm on a very long text.

Also, the language does not need a lot of facilities:

  * integers (strings can be represented as lists of integers)
  * characters (actually special literal syntax for integers)
  * read a line from stdin (read-line)
  * write a line to stdout (write-line)
  * simple arithmetic (add, sub, mod)
  * working with characters (char-is-alpha, char-to-upper)
  * basic flow control (break, if, unless, loop, call)
  * binding symbols (bind)
  * basic stack manipulation (swap, dup, rot)
  * working with lists (list-prepend, list-new, list-head, list-is-empty)

# Example #

Here is an example of how the cesar shift algorithm could be implemented in sina. It converts all characters in the range [a-zA-Z] into uppercase to make logic easier (sina, being a toy language makes lots of conditions rather hard). Each line has a comment with the state of the data stack (DS), at least the part of it being used by the current block.

```

{
	/* "if not" DS: [cond statement] */
	swap
	0 equals swap /* DS: [!cond statement] */
	if
} :unless bind

{
   /* cesar shift the character  DS: [ char ] */
   dup                     /* DS: [ char char ] */
   char-is-alpha           /* DS: [ char int ] */
   :break unless           /* DS: [ char ] */
   char-to-upper           /* DS: [ char ] */
   'A' sub                 /* DS: [ char ] */
   1 add                   /* DS: [ char ] */
   'Z' 'A' sub mod         /* DS: [ char ] */
   'A' add                 /* DS: [ char ] */
} :cesar-shift-char bind

{
   /* replace each element of the list with a shifted version, DS: [ list ] */
   list-new                /* DS: [ in-list out-list(empty) ] */
   {
      /* DS: [ in-list out-list ] */
      swap                 /* DS: [ out-list in-list ] */
      list-head            /* DS: [ out-list in-list(minus head) head ] */
      cesar-shift-char     /* DS: [ out-list in-list head ] */
	  roll                 /* DS: [ head out-list in-list ] */
      roll                 /* DS: [ in-list head out-list ] */
	  swap                 /* DS: [ in-list out-list head ] */
      list-append          /* DS: [ in-list out-list ] */
      swap                 /* DS: [ out-list in-list ] */
      dup                  /* DS: [ out-list in-list in-list ] */
      list-is-empty        /* DS: [ out-list in-list int ] */
      :break if            /* exit current block if != 0, DS: [ out-list in-list ] */
      swap                 /* DS: [ in-list out-list ] */
      loop
   }                       /* DS: [ in-list out-list(empty) block ] */
   call                    /* DS: [ out-list in-list(empty) ] */
   drop                    /* DS: [ out-list ] */
} :cesar-shift bind

{
   /* execute the cesar shift algorithm on stdin, DS: [ ] */

   read-line               /* DS: [ list ] */
   dup                     /* DS: [ list list ] */
   list-is-empty           /* DS: [ list int ] */
   :break if               /* exit current block if true, DS: [ list ] */
   cesar-shift             /* DS: [ list ] */
   print-line              /* DS: [ ] */
   loop                    /* redo the current block (main) */

} :main bind

main                       /* DS: [ empty-list ] */
drop                       /* [ ] */
```