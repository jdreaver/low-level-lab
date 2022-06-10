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

(INIT) // Start of outer loop. Initialize i = 0
@i
M=0

(LOOP) // Loop over screen, 256 * 512 / 16 = 8192 times

// if i == 8192, goto INIT
@8192
D=A
@i
D=D-M
@INIT
D;JEQ

// Store current screen location in @screen_i
@SCREEN
D=A
@i
D=D+M
@screen_i
M=D

// i += 1
@i
M=M+1

// Check if key pressed
@KBD
D=M
// if key == 0, goto BLACK, else WHITE
@BLACK
D;JGT
@WHITE
0;JEQ

(BLACK) // Black is 1
@screen_i
A=M
M=-1 // -1 is all 1's for word (1111111111111111)
@LOOP
0;JEQ

(WHITE) // White i 0
@screen_i
A=M
M=0 // 0 is white
@LOOP
0;JEQ
