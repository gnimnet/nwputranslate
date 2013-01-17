/****************************************************
*			A Small ASM Translater					*
*			By	Ming	2009-1-14					*
*****************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "OPcode.h"

//***************Marcos***************//
#define ERRORMSG_LEN 512
#define READ_BUF_SIZE 256
#define MAX_PRO_LINE 512
#define MAX_LABEL 256
#define LABEL_LEN 32
#define TEMPSTR_LEN 32
//***************Vars***************//
FILE *fp_src;//fp_src for the srouse file
FILE *fp_code;//fp_code for the mechine code file
char tmpbuf[TEMPSTR_LEN];//buffer used for temp
char read_buf[READ_BUF_SIZE];//buffer used for read
int bufcount;//show the num of char in buffer
int linecount;//for solving srouse
int opcode[MAX_PRO_LINE];//Each line for output
int opcount;
char labeltable[MAX_LABEL][LABEL_LEN];//define the label table
int labelline[MAX_LABEL];//define to label to line
int labelcount;
char findlabel[MAX_LABEL][LABEL_LEN];//define the label find in srouse
int findindex[MAX_LABEL];//define the place need to replace OPcode
int findlabelcount;//num of finding labels

//***************Functions***************//
int getline(FILE * fp);//get a line to fill buffer
int getlabel();//call by soveloneline() to get label
void soveloneline();//for sovel one line srouse
int hexch2num(char ch);//change a hex char to number
int cutbuf2buf();//get word from readbuf to tmpbuf

void mystrcpy(char*s,char*d,int num);//String copy function
int mystrlen(char*s);//Get string length function
int mystrcmp(char*s,char*d);//compare two string function
void cutstrhead(char*s,int num);//cut the string head for num chars

int main(int argc,char* argv[]){
	int i,j;
	if(argc!=3){//make sure the arg nums is right
		printf("Usage:%s src_filename code_filename\n",argv[0]);
		exit(0);
	}
	if((fp_src=fopen(argv[1],"r"))==NULL){//open srouse file for read
		printf("Open srouse file (for read) failed!Please check the file:%s...\n",argv[1]);
		exit(0);
	}
	if((fp_code=fopen(argv[2],"w+"))==NULL){//open code file for write
		printf("Open code file (for write) failed!Please check the file:%s...\n",argv[2]);
		exit(0);
	}
	//***************begin to translate ASM file***************//
	linecount=0;//init line counter
	opcount=0;//init op counter
	labelcount=0;//init label counter
	findlabelcount=0;//init the find label num
	while(!feof(fp_src)){
		if(getline(fp_src)==1)//get one line over to exit
			break;
		read_buf[--bufcount]=0;//delete return*****!!!!!*****May change in Windows*****!!!!!
		//***************already get one line***************//
		linecount++;//incrase line counter
		printf("line[%d]:%s\n",linecount,read_buf);//show code by this!~
		soveloneline();//sovel one line srouse
	}
	//***************sovel labels***************//
	for(i=0;i<findlabelcount;i++){
		for(j=0;j<labelcount;j++){
			if(mystrcmp(&labeltable[j][0],&findlabel[i][0])==0){
				opcode[findindex[i]]=labelline[j];
				break;
			}
		}
		if(j==labelcount){
			printf("Not find label \"%s\",line[%d]\n",&findlabel[i][0],linecount);
			exit(0);
		}
	}
	//***************write code file and finish***************//
	//write output code
	for(i=0;i<opcount;i++){
		fprintf(fp_code,"$P",i,opcode[i]);
		if(i>0x0F)
			fprintf(fp_code," %X",i);
		else
			fprintf(fp_code," 0%X",i);
		if(opcode[i]>0x0F)
			fprintf(fp_code," %X\n",opcode[i]);
		else
			fprintf(fp_code," 0%X\n",opcode[i]);
	}
	printf("Write to the code file:%s...done\n",argv[2]);
	fclose(fp_src);//close srouse file
	fclose(fp_code);//close code file
}

int getline(FILE * fp){//return 0--file not over,return 1--file over
	bufcount=0;
	if(!feof(fp))
		read_buf[bufcount++]=fgetc(fp);
	while( (!feof(fp)) && (read_buf[bufcount-1]!='\n') )
		read_buf[bufcount++]=fgetc(fp);
	if(feof(fp))
		return 1;//file over
	else
		return 0;//file not over
}

void deltext(){//delete the no use text in srouse
	int i;
	for(i=0;i<bufcount;i++){
		if(read_buf[i]==';'){
			read_buf[i]=0;
			bufcount=i;
			return;
		}
	}
}

void lcase(){//to low the chars in srouse
	int i;
	for(i=0;i<bufcount;i++){
		if(read_buf[i]>='a' && read_buf[i]<='z'){
			read_buf[i]=read_buf[i]-'a'+'A';
		}
	}
}

void soveloneline(){//this function to translate one line srouse
	int step=0;
	int opkind=0;//OP code kind,see down table
	/*******************************************************
		0:	error!
		1:	OP(4bit)	RS(2bit)	RD(2bit)
		2:	OP(4bit)	**(2bit)	RD(2bit)
		3:	OP(4bit)	M (2bit)	RD(2bit)	D (8bit)
		4:	OP(4bit)	M (2bit)	**(2bit)	D (8bit)
		5:	OP(4bit)	**(2bit)	RD(2bit)	D (8bit)
		6:	OP(4bit)	RS(2bit)	**(2bit)	D (8bit)
	*******************************************************/
	int errnum=0;//error number,see down table
	/*******************************************************
		0:	no error
		1:	missing ','
		2:	missing reg char 'R' or 'r'
		3:	wrong reg number
		4:	more char
		5:	number error
	*******************************************************/
	//***************Step 0:Init String***************//
	lcase();//to low the char in srouse
	deltext();//del the no use text
	//printf("After step0 readbuf=\"%s\"\n",read_buf);//show readbuf after init
	//***************Step 1:Sovel Label***************//
	if(getlabel()==1){//get label if have...
		step=1;
		printf("Get Label No.%d,\"%s\"\n",labelcount,labeltable[labelcount-1]);
	}
	else{
		printf("No Lable at line %d\n",linecount);
	}
	//***************Step 2:Sovel OP***************//
	while(cutbuf2buf()!=1){
		if(step==0||step==1){//0--no label,1--have label
			if(mystrcmp("ADD",tmpbuf)==0){
				opcode[opcount]=ADD;
				opkind=1;
			}
			else if(mystrcmp("AND",tmpbuf)==0){
				opcode[opcount]=AND;
				opkind=1;
			}
			else if(mystrcmp("IN",tmpbuf)==0){
				opcode[opcount]=IN;
				opkind=5;
			}
			else if(mystrcmp("OUT",tmpbuf)==0){
				opcode[opcount]=OUT;
				opkind=6;
			}
			else if(mystrcmp("MOV",tmpbuf)==0){
				opcode[opcount]=MOV;
				opkind=1;
			}
			else if(mystrcmp("SL",tmpbuf)==0){
				opcode[opcount]=SL;
				opkind=1;
			}
			else if(mystrcmp("LDI",tmpbuf)==0){
				opcode[opcount]=LDI;
				opkind=5;
			}
			else if(mystrcmp("INC",tmpbuf)==0){
				opcode[opcount]=INC;
				opkind=2;
			}
			else if(mystrcmp("SUB",tmpbuf)==0){
				opcode[opcount]=SUB;
				opkind=1;
			}
			else if(mystrcmp("OR",tmpbuf)==0){
				opcode[opcount]=OR;
				opkind=1;
			}
			else if(mystrcmp("RR",tmpbuf)==0){
				opcode[opcount]=RR;
				opkind=1;
			}
			else if(mystrcmp("SR",tmpbuf)==0){
				opcode[opcount]=SR;
				opkind=1;
			}
			else if(mystrcmp("LDA",tmpbuf)==0){
				opcode[opcount]=LDA;
				opkind=3;
			}
			else if(mystrcmp("STA",tmpbuf)==0){
				opcode[opcount]=STA;
				opkind=3;
			}
			else if(mystrcmp("JMP",tmpbuf)==0){
				opcode[opcount]=JMP;
				opkind=4;
			}
			else if(mystrcmp("BZC",tmpbuf)==0){
				opcode[opcount]=BZC;
				opkind=4;
			}
			else{
				printf("line %d have unknow OP code:\"%s\"\n",linecount,tmpbuf);
				exit(0);
			}
			step=2;
		}
		else if(step==2){//get OP num
			if(opkind==1){
				if(tmpbuf[2]!=','){
					errnum=1;
				}
				else if(tmpbuf[0]!='R'||tmpbuf[3]!='R'){
					errnum=2;
				}
				else if(tmpbuf[1]<'0'||tmpbuf[1]>'3'){
					errnum=3;
				}
				else if(tmpbuf[4]<'0'||tmpbuf[4]>'3'){
					errnum=3;
				}
				else if(tmpbuf[5]!=0){
					errnum=4;
				}
				else{//OP num right
					opcode[opcount]=(opcode[opcount]<<4)+(tmpbuf[4]-'0')*4+(tmpbuf[1]-'0');
					printf("Get OP code:0x%X\n",opcode[opcount]);
					opcount++;
				}
			}
			else if(opkind==2){
				if(tmpbuf[0]!='R'){
					errnum=2;
				}
				else if(tmpbuf[1]<'0'||tmpbuf[1]>'3'){
					errnum=3;
				}
				else if(tmpbuf[2]!=0){
					errnum=4;
				}
				else{//OP num right
					opcode[opcount]=(opcode[opcount]<<4)+(tmpbuf[1]-'0')*4+(tmpbuf[1]-'0');
					printf("Get OP code:0x%X\n",opcode[opcount]);
					opcount++;
				}
			}
			else if(opkind==3){
				if(tmpbuf[1]!=','||tmpbuf[4]!=','){
					errnum=1;
				}
				else if(tmpbuf[5]!='R'){
					errnum=2;
				}
				else if(tmpbuf[0]<'0'||tmpbuf[0]>'3'){
					errnum=3;
				}
				else if(tmpbuf[6]<'0'||tmpbuf[6]>'3'){
					errnum=3;
				}
				else if(tmpbuf[7]!=0){
					errnum=4;
				}
				else if(hexch2num(tmpbuf[2])==-1){
					errnum=5;
				}
				else if(hexch2num(tmpbuf[3])==-1){
					errnum=5;
				}
				else{//OP num right
					opcode[opcount]=(opcode[opcount]<<4)+(tmpbuf[0]-'0')*4+(tmpbuf[6]-'0');
					printf("Get OP code:0x%X\n",opcode[opcount]);//show the OP code
					opcount++;
					opcode[opcount]=(hexch2num(tmpbuf[2])<<4)+hexch2num(tmpbuf[3]);
					printf("Get OP num:0x%X\n",opcode[opcount]);//show the OP num
					opcount++;
				}
			}
			else if(opkind==4){
				if(tmpbuf[1]!=','){
					errnum=1;
				}
				else if(tmpbuf[0]<'0'||tmpbuf[0]>'3'){
					errnum=3;
				}
				else{//OP num right
					opcode[opcount]=(opcode[opcount]<<4)+(tmpbuf[0]-'0')*4;
					printf("Get OP code:0x%X\n",opcode[opcount]);//show the OP code
					opcount++;
					cutstrhead(tmpbuf,2);
					mystrcpy(tmpbuf,&findlabel[findlabelcount][0],mystrlen(tmpbuf));
					findindex[findlabelcount++]=opcount;//record the code need replace
					printf("Get label:\"%s\"\n",tmpbuf);//show the finding label
					opcount++;
				}
			}
			else if(opkind==5){
				if(tmpbuf[2]!=','){
					errnum=1;
				}
				else if(tmpbuf[0]!='R'){
					errnum=2;
				}
				else if(tmpbuf[1]<'0'||tmpbuf[1]>'3'){
					errnum=3;
				}
				else if(tmpbuf[5]!=0){
					errnum=4;
				}
				else if(hexch2num(tmpbuf[3])==-1){
					errnum=5;
				}
				else if(hexch2num(tmpbuf[4])==-1){
					errnum=5;
				}
				else{//OP num right
					opcode[opcount]=(opcode[opcount]<<4)+(tmpbuf[1]-'0');
					printf("Get OP code:0x%X\n",opcode[opcount]);//show the OP code
					opcount++;
					opcode[opcount]=(hexch2num(tmpbuf[3])<<4)+hexch2num(tmpbuf[4]);
					printf("Get OP num:0x%X\n",opcode[opcount]);//show the OP num
					opcount++;
				}
			}
			else if(opkind==6){
				if(tmpbuf[2]!=','){
					errnum=1;
				}
				else if(tmpbuf[3]!='R'){
					errnum=2;
				}
				else if(tmpbuf[4]<'0'||tmpbuf[4]>'3'){
					errnum=3;
				}
				else if(tmpbuf[5]!=0){
					errnum=4;
				}
				else if(hexch2num(tmpbuf[0])==-1){
					errnum=5;
				}
				else if(hexch2num(tmpbuf[1])==-1){
					errnum=5;
				}
				else{//OP num right
					opcode[opcount]=(opcode[opcount]<<4)+(tmpbuf[4]-'0')*4;
					printf("Get OP code:0x%X\n",opcode[opcount]);//show the OP code
					opcount++;
					opcode[opcount]=(hexch2num(tmpbuf[0])<<4)+hexch2num(tmpbuf[1]);
					printf("Get OP num:0x%X\n",opcode[opcount]);//show the OP num
					opcount++;
				}
			}
			else{
				printf("Unknow error happen when translate line %d\n",linecount);
				exit(0);
			}
			if(errnum!=0){
				switch(errnum){
					case 1:printf("missing ',' for line %d ,OP num\n",linecount);break;
					case 2:printf("missing reg char 'R' or 'r' at line %d ,OP num\n",linecount);break;
					case 3:printf("wrong reg number at line %d ,OP num\n",linecount);break;
					case 4:printf("more char at line %d ,OP num\n",linecount);break;
					case 5:printf("number error at line %d ,OP num\n",linecount);break;
					default:printf("Undefine error happen at line %d ,OP num\n",linecount);
				}
				exit(0);
			}
			step=3;
		}
		else{
			printf("Line %d have unknow OP code:\"%s\"\n",linecount,tmpbuf);
			exit(0);
		}
		//printf("readbuf=\"%s\"\n",read_buf);//show read buffer
		//printf("tmpbuf=\"%s\"\n",tmpbuf);//show temp buffer
		
	}
	if(step==1){
		printf("Line %d only has label!\n",linecount);
		exit(0);
	}
	if(step==2){
		printf("Line %d missing OP num!\n",linecount);
		exit(0);
	}
	if(step==0){
		printf("Line %d has not OP\n",linecount);
	}
}

int getlabel(){//try to find jump label...if have,record it~
	int i,j;
	for(i=0;i<bufcount;i++){
		if(read_buf[i]==':'){
			for(j=0;j<i;j++){
				if(read_buf[j]!=' ' && read_buf[j]!='\t')
					break;//get frist not space postion
			}
			if(i==j){
				printf("Empty label at line %d!\n",linecount);
				exit(0);
			}
			labelline[labelcount]=opcount;//record op line number
			mystrcpy(&read_buf[j],&(labeltable[labelcount][0]),i-j);//copy label
			labeltable[labelcount++][i]=0;//end of string
			cutstrhead(read_buf,i+1);
			bufcount=mystrlen(read_buf);
			//printf("goto op line=%d\n",labelline[labelcount-1]);//show record op line
			//printf("label=\"%s\"\n",labeltable[labelcount-1]);//show new label
			//printf("now readbuf=\"%s\"\n",read_buf);//show read_buf after cut
			//printf("now len(readbuf)=\"%d\"\n",bufcount);//show bufcount after cut
			return 1;
		}
	}
	return 0;
}

int cutbuf2buf(){//get a word from readbuf to tempbuf
	int i,j,k;
	int rtn=0;
	for(i=0;i<bufcount;i++){
		if(read_buf[i]!='\t' && read_buf[i]!=' '){
			for(j=i+1;j<bufcount;j++){
				if(read_buf[j]=='\t' || read_buf[j]==' '){
					 break;//get the tail
				}
			}
			j--;//del space or go back from overflow
			break;//get the head
		}
	}
	if(i>=bufcount)
		return 1;//string has cut over,should not go on
	for(k=0;i<=j;i++,k++)
		tmpbuf[k]=read_buf[i];//copy string
	tmpbuf[k]=0;//add the end char to temp buffer
	cutstrhead(read_buf,j+1);//delete the head of readbuf which now in temp buffer
	bufcount=mystrlen(read_buf);
	return rtn;
}

int hexch2num(char ch){//change a hex char to integer,if not hex,return -1
	if(ch>='0' && ch<='9')
		return (ch-'0');
	else if(ch>='A' && ch<='F')
		return (ch-'A'+10);
	else
		return -1;
}

//***************String functions***************//
void mystrcpy(char*s,char*d,int num){//String copy function
	while(num>0){
		*(d++)=*(s++);
		num--;
	}
}

int mystrlen(char*s){//Get string length function
	int i=0;
	while(*(s++)!=0)
		i++;
	return i;
}

int mystrcmp(char*s,char*d){//return 1--string not equle,0--string equle
	while( (*s!=0) && (*d!=0) ){
		if(*(s++)!=*(d++))
			return 1;//compare data in string
	}
	if(*s!=*d)
		return 1;//finish of string not equle
	else
		return 0;//finish of string equle
}

void cutstrhead(char*s,int num){//cut frist num chars
	mystrcpy(s+num,s,mystrlen(s)-num+1);
}
