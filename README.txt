Translaterʹ��˵��

����������ִ�г��򣬲���ΪԴ�ļ�����Ҫ��������Ŀ������ļ���
�磺translater.exe filein.txt fileout.txt

�ֺź�Ϊ����ע�ͣ�ָ���Ϊ�������Ͳ�����������
���������֮���á�,���ָ��Ҫ���κοո�
������תʹ�ñ�ǩ����ǩӦ���ӵ�ĳ��ָ�������֮ǰ�����õ�������
����ȥ�ִ�Сд����ǩ��Ӧ�����ظ���ǩ
������������Ӧ��ʹ��2���ַ�����ʾʮ���������ݣ���0x1CдΪ1C��1c
Ѱַ��ʽʹ��һ���ַ���ʾ4��Ѱַ��ʽ
"0"/"1"/"2"/"3"�ֱ��Ӧֱ��/���/��ַ/���
��������ִ���ֱ���˳�������ļ���Ϊ��

ע���������Ӧ����һ�����У��������һ�����ݽ�������

ָ���ʽʵ�����£�

MOV	R1,R0	;R0->R1
ADD	R1,R0	;R0+R1->R1
SUB	R1,R0	;R0-R1->R1
AND	R1,R0	;R0&R1->R1
OR	R1,R0	;R0|R1->R1
RR	R1,R0	;R0�һ���->R1
SL	R1,R0	;R0�߼�R1����->R1
SR	R1,R0	;R0�߼�R1����->R1
INC	R1	;R1������
IN	R0,00	;�ӵ�ַ0x00���뵽R0
OUT	40,R0	;��R0���������0x40
LDI	R3,1F	;��R3��������0x1F
LDA	0,R3,3F	;��R3���ڴ�[3F]��Ԫ����
STA	0,R3,3F	;���ڴ�[3F]��Ԫ��R3����
JMP	0,LABEL	;��������ת��LABEL��
BZC	0,LABEL	;������ת��LABEL��

�üӷ�ģ��8bit*8bit�˷���������:

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
