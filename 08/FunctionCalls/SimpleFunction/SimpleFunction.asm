@START
0;JMP
(MAKETRUE)
@SP
AM=M-1
M=-1
@SP
AM=M+1
@R15
A=M
0;JMP
(START)
(_SIMPLEFUNCTION_)

(test)
@0
D=A
@SP
A=M
M=D
@SP
AM=M+1
@0
D=A
@SP
A=M
M=D
@SP
AM=M+1

@LCL
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
AM=M+1
@LCL
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
AM=M+1
@SP
AM=M-1
D=M
@SP
AM=M-1
M=D+M
@SP
AM=M+1
@SP
AM=M-1
M=!M
@SP
AM=M+1
@ARG
D=M
@0
A=D+A
D=M
@SP
A=M
M=D
@SP
AM=M+1
@SP
AM=M-1
D=M
@SP
AM=M-1
M=D+M
@SP
AM=M+1
@ARG
D=M
@1
A=D+A
D=M
@SP
A=M
M=D
@SP
AM=M+1
@SP
AM=M-1
D=M
@SP
AM=M-1
M=M-D
@SP
AM=M+1

@LCL
D=M
@FRAME
M=D
@FRAME
D=M
@5
A=D-A
D=M
@RET
M=D
@SP
AM=M-1
D=M
@ARG
A=M
M=D
@ARG
D=M+1
@SP
M=D
@FRAME
D=M
@1
A=D-A
D=M
@THAT
M=D
@FRAME
D=M
@2
A=D-A
D=M
@THIS
M=D
@FRAME
D=M
@3
A=D-A
D=M
@ARG
M=D
@FRAME
D=M
@4
A=D-A
D=M
@LCL
M=D
@RET
A=M
0;JMP

(END)
@END
0;JMP
