// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

// Put your code here.

    // set R0 to -1
    // R0 represent for the last black index
    @0
    M=-1
(READ)
    // detect keyboard input
    @KBD
    D=M
    @WHITE
    D;JEQ // if (D== 0) goto WHITE
    @BLACK
    0;JMP // else goto BLACK

(BLACK)
    // if (R0 == 8191) goto READ
    @8191
    D=A
    @0
    D=D-M // D = 8191 - R0
    @READ
    D;JEQ

    @0
    M=M+1 // R0++

    @SCREEN
    D=A
    @0
    D=D+M // D = SCREEN+R0
    A=D
    M=-1 // set M[SCREEN+R0] to 0xFFFF

    @READ
    0;JMP

(WHITE)
    // if (R0 < 0) goto READ
    @0
    D=M
    @READ
    D;JLT

    @SCREEN
    D=A
    @0
    D=D+M // D = SCREEN+R0
    A=D
    M=0 // set M[SCREEN+R0] to 0xFFFF

    @0
    M=M-1 // R0--

    @READ
    0;JMP