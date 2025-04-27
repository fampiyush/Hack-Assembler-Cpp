# Hack Assembler

A single-file Hack assembler written in C++ for the Hack assembly language from the [Nand2Tetris](https://www.nand2tetris.org/) course.

## How to Run

```bash
# Compile the program
g++ main.cpp -o assembler

# Run the assembler
./assembler <input.asm> <output.hack>
```

The assembler translates Hack assembly code (.asm) into binary machine code (.hack) for the Hack computer platform.