## Generate Suffix Automaton (SAM) PNG

### files

* `solution.cc`: SAM demo.
* `input.txt`: input string for creating SAM.
* `output.txt`: generated DOT source code.

### usage

1. `make` invoke `clange++` to compile `solution.cc`, generate a `solution` executable. If you are a `gcc` user, you know what to do (:
2. type some arbitrary string to `input.txt` , i.e. `abcbc`.
3. `./solution`, run the program to generate dot source file.
4. `dot -T png -o output.png output.txt`, generate PNG.

### example

