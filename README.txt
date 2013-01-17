Translater使用说明

在命令行中执行程序，参数为源文件名和要保存代码的目标代码文件名
如：translater.exe filein.txt fileout.txt

分号后为程序注释，指令分为操作符和操作数两部分
多个操作数之间用“,”分割，不要加任何空格
程序跳转使用标签，标签应当加到某条指令操作符之前，不得单独成行
程序不去分大小写，标签不应出现重复标签
所有立即数据应当使用2个字符，表示十六进制数据，如0x1C写为1C或1c
寻址方式使用一个字符表示4种寻址方式
"0"/"1"/"2"/"3"分别对应直接/间接/变址/相对
汇编器发现错误将直接退出，输出文件将为空

注：代码最后应当留一个空行，否则最后一行数据将被忽略

指令格式实例如下：

MOV	R1,R0	;R0->R1
ADD	R1,R0	;R0+R1->R1
SUB	R1,R0	;R0-R1->R1
AND	R1,R0	;R0&R1->R1
OR	R1,R0	;R0|R1->R1
RR	R1,R0	;R0右环移->R1
SL	R1,R0	;R0逻辑R1右移->R1
SR	R1,R0	;R0逻辑R1左移->R1
INC	R1	;R1自增长
IN	R0,00	;从地址0x00输入到R0
OUT	40,R0	;将R0数据输出到0x40
LDI	R3,1F	;给R3置立即数0x1F
LDA	0,R3,3F	;将R3置内存[3F]单元的数
STA	0,R3,3F	;将内存[3F]单元置R3的数
JMP	0,LABEL	;无条件跳转到LABEL处
BZC	0,LABEL	;条件跳转到LABEL处

用加法模拟8bit*8bit乘法代码如下:

start:		in	r0,00		;get mul1
		in	r1,00		;get mul2
		ldi	r3,00
		sta	0,FF,r3		;make [FF] to be 0x00
		sta	0,FD,r3		;make [FD] to be 0x00
		ldi	r2,01
		sta	0,FE,r2		;make [FE] to be 0x01
loop:		lda	0,FF,r2		;get 0 to compare r1
		sub	r1,r2		;test r1
		bzc	0,end		;mul over
		add	r3,r0		;add num
		bzc	0,have
		jmp	0,addover
have:		lda	0,FD,r2		;With fc add to [FD]
		inc	r2
		sta	0,FD,r2
addover:	lda	0,FE,r2		;dec r1
		sub	r1,r2
		jmp	0,loop
end:		lda	0,FD,r2		;get high 8bit
loophere:	jmp	0,loophere
