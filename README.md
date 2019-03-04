

Variant Expansion
=================

Generate variants of an input string based on markup:
the vertical bar `|` separates variants, brackets `[...]` limit
the scope of variants, and brackets with only one variant (i.e.,
no vertical bar) denote an optional part.

Empty variants are not emitted.  
Brackets can be nested, but should be balanced.  
Invalid markup may result in unexpected output, but never raises an error.

Examples:

 * `"foo|bar"` expands into the two strings `"foo"` and `"bar"`
 * `"ba[r|z]"` expands into the two strings `"bar"` and `"baz"`
 * `"qu[u]x"` expands into `"quux"` and `"qux"`
 * `"foo|ba[r|z[aar]]|quux"` expands
   into `"foo"`, `"bar"`, `"bazaar"`, `"baz"`, `"quux"`
 * `"[foo|]"` expands to `"foo"` (the empty string is not emitted)

Limitations:

 * No escaping is possible, that is, brackets and the vertical bar
   are always interpreted with their special meaning.


Syntax
------

The syntax of variant notation is as follows:

```
Variants = Sequence { "|" Sequence }.  
Sequence = Part { Part }.  
Part = Text | "[" Variants "]".
```

Here Text is any characters other than the vertical bar or brackets;
{...} denotes repetition (zero or more times), and | separates alternatives.
(See [WSN][wsn] and [EBNF][ebnf] in Wikipedia.)

[wsn]: https://en.wikipedia.org/wiki/Wirth_syntax_notation
[ebnf]: https://en.wikipedia.org/wiki/Extended_Backus-Naur_form


Usage
-----

From a shell prompt, type `make` to compile the C source,
which should result in the `variants` executable program.

Usage: **variants** _text_ ...

The `variants` program reads variant notation from all its arguments,
one after the other, and writes the expansions to standard output, each
followed by a newline. Be careful to quote arguments because the special
characters of variant syntax have special meaning to your shell, too.
An example run in your shell could look like this:

```
$ variants "ba[r|z[aar]]" qu[u]x
bar
bazaar
baz
quux
qux
```

The Python version of the program works exactly the same.
Pass the `variants.py` script and any arguments to expand
to the `python` executable (works with both 2.x and 3.x).

Usage: python **variants.py** _text_ ...


Miscellaneous
-------------

The implementation works by traversing the dag defined by the markup
as though it was a tree. For example, the input `"a[1|2]b"` defines
the dag in Fig 1, which for expansion is traversed as though it was
the tree in Fig 2. A preliminary parsing step creates two arrays,
_down_ and _side_, with link indices that facilitate navigation on the dag:
in Fig 3, `|` and `/` are _down_ links, and `--` is a _side_ link.

          a                   a                   a
         / \                 / \                  |
        1   2               1   2                 1--2
         \ /                |   |                 | /
          b    Fig.1        b   b   Fig.2         b     Fig.3

Some more examples:

 * `It[[em|alic]iz|erat]e[d], please` (from [Rosetta Code][rc])
 * `a[1|2|3]b[4|5]c` (yields 3 x 2 = 6 variants)
 * `J[oh[ann]] W[olfgang] [v[on] ]Goethe`

Weird and invalid syntax examples:

 * `[[[foo]]]` yields only "foo" (because empty variants are not emitted)
 * `[|||]` yields no output (because empty variants are not emitted)
 * `f]o|o]` yields "fo" and "o" (unmatched closing brackets are ignored)
 * `[f[o[o` yields "foo", "fo", "f"
    (unmatched opening brackets are implicitly closed at end of input)
 * `x[]x` and `xx[` both yield "xx" and "xx"
    ([] is equivalent to [|] and the unmatched bracket is implicitly closed)
 * `x[|]x` yields 2 times "xx"
 * `x[[]]x` yields 3 times "xx"

Note that Variant Expansion is the same idea as *Brace Expansion* 
in Unix shells (e.g. in [Bash][bash]), but with a different syntax.
Also, brace expansion usually has an extra feature called range
expansion, and brace expansion is only one of many types of expansion
supported by shells (see [here][lcorg]). Implementations of brace
expansion in many languages can be found at [Rosetta Code][rc].

[rc]: https://rosettacode.org/wiki/Brace_expansion
[bash]: https://www.gnu.org/software/bash/manual/html_node/Brace-Expansion.html
[lcorg]: http://linuxcommand.org/lc3_lts0080.php


License
-------

MIT

See the LICENSE file (or read the license text
at [choosealicense.com](https://choosealicense.com/licenses/mit/)
or [opensource.org](https://opensource.org/licenses/MIT)).

