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

`abc`:

![abc](https://cloud.githubusercontent.com/assets/5213906/16720449/09bea8ce-4769-11e6-94ef-f9c1a6f5ff77.png)

`abcb`:
![abcb](https://cloud.githubusercontent.com/assets/5213906/16720451/0ca65b9a-4769-11e6-807f-2a5bfdef24f9.png)

`abcbc`:
![abcbc](https://cloud.githubusercontent.com/assets/5213906/16720453/0e7c474a-4769-11e6-8da2-157a9661d1f9.png)