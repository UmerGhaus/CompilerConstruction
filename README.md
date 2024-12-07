# Compiler Construction

This repository contains the source code for a simple **Compiler** that processes source code written in a high-level programming language and translates it into assembly code.

## Phases of the Compiler

The compiler follows several phases, starting from tokenization and ending with the generation of assembly code. Below is an overview of the phases:

### 1. **Tokenizer (Lexical Analysis)**
The tokenizer scans the input source code and breaks it down into tokens. The tokenizer recognizes a variety of constructs in the language, such as keywords, operators, literals, and control structures.

#### Key Tokens:
- **Basic Tokens**: `T_INT`, `T_FLOAT`, `T_ID`, `T_NUM`, `T_SEMICOLON`, `T_ASSIGN`, etc.
- **Additional Tokens**: `T_BOOL`, `T_CHAR`, `T_STRING`, `T_COUT`, `T_VOID`, `T_STRING_LITERAL`, etc.
- **Control Structures**: `T_AGAR` (if condition), `T_WARNA` (else condition),  `T_WAPSI` (return), `T_JABTAK` (while loop), `T_FOR`.

#### Features:
- **Comments Handling**: Single-line (`//`) and multi-line (`/* */`) comments are supported.
- **String Literals**: Strings are handled, including escape sequences (e.g., `\n`, `\t`, `\"`).

---

### 2. **Parser (Syntax Analysis)**
The parser takes the tokens produced by the tokenizer and builds a syntactic structure, usually in the form of a parse tree or abstract syntax tree (AST). It checks whether the token sequence adheres to the grammar rules of the language.

#### Key Features:
- **Variable Declarations**: Supports `int`, `float`, `string`, `bool`, etc.
- **Functions**: Handles function declarations and calls (supports `void` and other return types).
- **Control Structures**: Parses `if-else`, `for`, and `while` loops.
- **Expressions**: Supports arithmetic and logical operations (`+`, `-`, `*`, `/`, `<`, `>`, `<=`, `>=`, `!=`, `==`, etc.).

---

### 3. **Intermediate Code Generation (TAC)**
After the parser processes the source code, the intermediate code generator (ICG) produces **Three-Address Code (TAC)**. TAC is a lower-level representation of the program that can be more easily translated into assembly code.

#### Key Features:
- **Arithmetic Operations**: Includes operations like addition, multiplication, and comparisons.
- **Function Calls**: Supports function call generation in TAC.
- **Control Flow**: Handles conditional jumps, loops, and return statements.

#### Example TAC:
```plaintext
t0 = y * 3
t1 = x + t0
sum = t1
t2 = x < 10
agar t2 goto L1
goto L2
L1:
CALL greet_func
L2:
wapsi 0

```
---

### 4. **Assembly Code Generation**
After the Intermediate Code Generation (TAC) phase, the assembly code generation phase takes over. This phase is responsible for translating the three-address code (TAC) into x86 assembly code, which can then be executed on a machine. The assembly code generation step ensures that the intermediate code is converted into low-level instructions that the CPU can understand and execute.

#### Key Features:
- **Arithmetic Operations**: Includes operations like addition, multiplication, and comparisons.
- **Function Calls**: Supports function call generation in TAC.
- **Control Flow**: Handles conditional jumps, loops, and return statements.
- **Function Calls**: The assembly code handles function calls by generating the `CALL` instruction.
- **Handling Labels**: Labels are used to mark positions in the assembly code for jumps and loops.
- **Return Statements**: The `RET` instruction is used to return control from a function.

#### Example Assembly Code:
```plaintext
MOV AX, [y]        ; Load y into AX
IMUL AX, 3         ; Multiply AX by 3
MOV [t0], AX       ; Store the result in t0

MOV AX, [x]        ; Load x into AX
ADD AX, [t0]       ; Add t0 to x
MOV [t1], AX       ; Store the result in t1

MOV AX, [x]        ; Load x into AX
CMP AX, 10         ; Compare x with 10
SETL AL            ; Set AL to 1 if x < 10, otherwise set to 0
MOV [t2], AL       ; Store the result in t2

MOV AL, [t2]       ; Load condition result (x < 10) into AL
CMP AL, 1          ; Compare with 1 (true condition)
JE L1              ; Jump to L1 if true
JMP L2             ; Jump to L2 if false

L1:
CALL greet_func    ; Call the greet_func function

L2:
MOV AX, 0          ; Load the return value (0)
RET                ; Return from the main function
```
---

### 5. **Conclusion**
The Assembly Code Generation phase is crucial in completing the translation from high-level source code to machine-level instructions. The generated assembly code serves as the final step in compiling the program, making it executable on a target system. Through this phase, the compiler achieves the goal of transforming high-level constructs (such as variable assignments, control flow, and function calls) into low-level assembly instructions that the CPU can execute directly.
