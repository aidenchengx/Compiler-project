#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#define boolean int
#define true 1
#define false 0
#define null NULL
using namespace std;
int printhtml(struct format f1,int p);
/*Ã“Ã¯Â·Â¨Ã–Ã†ÂµÂ¼Â½Ã¡Â¹Â¹ÃŠÃ·*/


/*字符流归类*/ 
boolean isDigit(char ch){		//数字类
	if(ch>='0'&&ch<='9') return true;
	return false;
}

boolean isLet(char ch){			//字母类 
	if(ch>='a'&&ch<='z') return true;
	else if(ch>='A'&&ch<='Z') return true;
	return false;
}

int isSep(char ch){			
		switch(ch){
			case '(':return 6;
			case ')':return 7;
			case '{':return 8;
			case '}':return 9;
			case '_':return 10;
			case '^':return 11;
			default: return 0;	
		}
	
}


static char  id1[100][20];							//词法的内容存储
static int  id1type[100];							//词法的类型存储 
static int  id1num=0;								//词法的数量统计

static int levelleft[100];
static int maxlevel=0;


typedef struct format{
		int left;
		int top;
		int size;
		int style;									//html显示字体风格
		int kind;									//归约的类型
		int parent;		
}*ftptr,ftnode;
/*0 B-{T} 1_T 2^T 3_^TT 4UTTT 5 STTT*/
struct ftreestack
	{	ftnode ftlist[50];
		int top;
	};          									//后序遍历语法制导树
static ftreestack ft1;

void print2(){
	cout<<endl;
for(int i=0;i<ft1.top;i++)
cout<<ft1.ftlist[i].kind;
cout<<endl;
for(int i=0;i<ft1.top;i++)
cout<<ft1.ftlist[i].parent;
cout<<endl;
for(int i=0;i<ft1.top;i++)
cout<<ft1.ftlist[i].left<<' ';
cout<<endl;}

void leveladjust(int k){
	for(int i=0;i<=k-2;i++){
		levelleft[i]=levelleft[k];
	}

}
/*归约BFS处理结构*/
ftnode fpop(struct ftreestack &fs)
{	fs.top--;	
	return fs.ftlist[fs.top];
	
};
void fpush(struct ftreestack &fs,ftnode p)
{		
	fs.ftlist[fs.top++]=p;
};

void fonpop(struct format &s,int p)
{	if(p==-1){										//空运算 left回传	
	if(s.left>ft1.ftlist[s.parent].left) ft1.ftlist[s.parent].left=s.left;
	return;
	}

	if(s.kind==12||s.kind==0) {
					s.left=ft1.ftlist[s.parent].left;return;}
	
	if(s.kind==11||s.kind==40) {ft1.ftlist[s.parent].left=s.left+strlen(id1[p])*s.size/2;
					s.left=ft1.ftlist[s.parent].left;return;}
	if(s.kind==50){ft1.ftlist[s.parent].left=s.left+s.size/2;
					s.left=ft1.ftlist[s.parent].left;return;}
	if(s.kind==32){if(s.left>ft1.ftlist[s.parent].left) ft1.ftlist[s.parent].left=s.left;return;}
};
void fonchange(struct format &s){
	if(s.kind==12||s.kind==33||s.kind==40||s.kind==0){
		if(s.left<ft1.ftlist[s.parent].left) s.left=ft1.ftlist[s.parent].left;return;
	}

}
/*词法分析器*/
/*0终止符 1数字常量 2字母数字混合变量 3\blank 	4\int 	5\sum 	6( 	7) 	8{    	9} 	10_ 	11^	12_^		*/  

int scanner(int &p,char *rs,char *tk){	
		int co=0;int type=0,c=0;
		for(int i=0;i<20;i++)
			tk[i]='\0'; 
		
		while(rs[p]==' ') p++;
		if(isDigit(rs[p])==true) 
			{tk[co++]=rs[p++]; 							//符合类型 向后扫描
					while(isDigit(rs[p])==true||rs[p]=='.')
						tk[co++]=rs[p++];
					tk[co]='\0';
					 type=1;
			}
				else if (isLet(rs[p])==true){ 			//符合类型 向后扫描
					tk[co++]=rs[p++]; 
					while(isDigit(rs[p])==true||isLet(rs[p])==true)
						tk[co++]=rs[p++];
							tk[co]='\0';
						type=2;
				}
				else if((c=isSep(rs[p]))&&(c!=0)){
				if(c==10){
					int c1=isSep(rs[p+1]);
					if(c1==11) {
						tk[co++]=rs[p++];
						tk[co++]=rs[p++];
						tk[co]='\0';
						type=12;
					}
					else {tk[co++]=rs[p++];
					  		tk[co]='\0';
					  	type=c;type=10;}
				}
				else {tk[co++]=rs[p++];
					  tk[co]='\0';
					  type=c;
				}
				}
				
				else if(rs[p]=='\\'){						
							p++;
							if(rs[p]=='b'&&rs[p+1]=='l'&&rs[p+2]=='a'&&rs[p+3]=='n'&&rs[p+4]=='k')
								{p=p+5;
								strcpy(tk,"\\blank"); type=3;
								 }
							else if(rs[p]=='i'&&rs[p+1]=='n'&&rs[p+2]=='t') 
									{p=p+3;
									strcpy(tk,"\\int"); type=4;
								 	}
							else if(rs[p]=='s'&&rs[p+1]=='u'&&rs[p+2]=='m') 
										{p=p+3;
								 			strcpy(tk,"\\sum"); type=5;	}
												}
						else if(rs[p]=='$') 
							{type=0;tk[co++]='$';tk[co]='\0';}
						else {type=-1;p++;				//错误的输入 词法分析器报错
						}	
		return type;
		
} 

/*语法分析器*/

struct stacklist{										/*语法分析器栈结构*/
	char stack[100];
	int top; 											//栈顶
};

char pop(struct stacklist &s1){
	char e;
	e=s1.stack[s1.top];
	s1.stack[s1.top]='\0';
	s1.top--;
	return e;
}

void push(struct stacklist &s1,char e){
	s1.top++;
	s1.stack[s1.top]=e;
	s1.stack[s1.top+1]='\0';
	return;
}

void grammarprinter(struct stacklist &s1,int &p,char *r)
{			/*语法分析输入函数*/
 			cout.setf(std::ios::left);
 			cout.width(15);
			cout<<s1.stack;			
			
			for(int i=p;i<id1num;i++){
			cout<<id1[i];}
			cout<<endl;
			
			cout.width(80);
			cout<<" ";
			cout<<r<<endl;
			print2();
			}
			

int grammarredu(struct stacklist &s1,int type,int &p){
	/*规约过程*/	
	char top=s1.stack[s1.top];
	char e;
	char *ch;
	struct format s;
	if (top=='S') {  
		if(type==0) { 
			e=pop(s1);
			push(s1,'$');push(s1,'B');push(s1,'$');
				{s.left=500;s.size=50;s.top=175;s.kind=0;s.parent=-1;
				fpush(ft1,s);
				}
			grammarprinter(s1,p,"S->$B$");
			return 1;
		}
		return -1;
	}
	
	if(top=='B'){
		if(type==1){  	
		/* B->NDB*/ 
			push(s1,'D');
			push(s1,'N');
			{	s=fpop(ft1);
				fonchange(s);
				struct format d,n;
				d=s;n=s;
				n.kind=11;d.kind=12;
				n.parent=ft1.top;d.parent=ft1.top;
				fpush(ft1,s);fpush(ft1,d);fpush(ft1,n);
			 } 
			ch="B->NDB";
			grammarprinter(s1,p,ch);
			return 1;
		}
		if(type==2){
		/* B->VDB*/
			push(s1,'D');
			push(s1,'V');
			{	s=fpop(ft1);
				fonchange(s);
				struct format d,v;
				d=s;v=s;
				v.kind=11;d.kind=12;
				v.parent=ft1.top;d.parent=ft1.top;
				fpush(ft1,s);fpush(ft1,d);fpush(ft1,v);
			 } 
			ch="B->VDB";
			grammarprinter(s1,p,ch);
			return 1;
		}
		if(type==3){
		/* B->KB*/
			push(s1,'K'); 
			ch="B->KB";
				{s=fpop(ft1);
			 	struct format k;
			 	fonchange(s);
			 	k=s;
			 	k.kind=50;
			 	k.parent=ft1.top;
			 	fpush(ft1,s);
			 	fpush(ft1,k);}
			grammarprinter(s1,p,ch);
			return 1;
		}
		if(type==4){
		/* B->ITTTB*/	
			pop(s1);
			push(s1,'B');
			push(s1,'T');push(s1,'T');push(s1,'T');
			push(s1,'I');
				{s=fpop(ft1);
				struct format u,t1,t2,t3,b;
				 fonchange(s);
				 u=s;t1=s;t2=s;t3=s;b=s;
				 u.size=u.size*8/5;
				 u.top=u.top-u.size*2/7;
				 u.kind=21;

				  t1.size=t1.size*3/5;
				 t1.top=t1.top+t1.size*6/5;
				 t1.left=u.left+u.size*3/4;//
				 t1.parent=ft1.top;
				 t1.kind=12;

				 b.left=t1.left;

				  t2.size=t2.size*3/5;
				 t2.top=t2.top-t2.size*5/4;
				 t2.kind=32;
				 t2.parent=t1.parent;
				 t2.left=u.left+u.size*3/4;//

				 t3.left=t2.left;
				 t3.parent=t1.parent;
				 t3.kind=12;

				 fpush(ft1,b);
				 fpush(ft1,t3);
				 fpush(ft1,t2);
				 fpush(ft1,t1);
				 fpush(ft1,u);
				}
			ch="B->ITTTB";
			grammarprinter(s1,p,ch);
			return 1;
		}
		if(type==5){
		/* B->UTTTB*/	
			pop(s1);
			push(s1,'B');
			push(s1,'T');push(s1,'T');push(s1,'T');
			push(s1,'U');
				{s=fpop(ft1);
				struct format u,t1,t2,t3,b;
				 fonchange(s);
				 u=s;t1=s;t2=s;t3=s;b=s;
				 u.size=u.size*8/5;
				 u.top=u.top-u.size*2/7;
				 u.kind=22;

				  t1.size=t1.size*3/5;
				 t1.top=t1.top+t1.size*6/5;
				 t1.left=u.left+u.size*3/4;//
				 t1.parent=ft1.top;
				 t1.kind=12;

				 b.left=t1.left;

				  t2.size=t2.size*3/5;
				 t2.top=t2.top-t2.size*5/4;
				 t2.kind=32;
				 t2.parent=t1.parent;
				 t2.left=u.left+u.size*3/4;//

				 t3.left=t2.left;
				 t3.parent=t1.parent;
				 t3.kind=12;

				 fpush(ft1,b);
				 fpush(ft1,t3);
				 fpush(ft1,t2);
				 fpush(ft1,t1);
				 fpush(ft1,u);
				}
			ch="B->UTTTB";
			grammarprinter(s1,p,ch);
			return 1;
		}
		if(type==6){
			/*B->(B)DB*/
			pop(s1);
			push(s1,'B');
			push(s1,'D');
			push(s1,')');
			push(s1,'B');
			push(s1,'(');
			
			ch="B->(B)DB";
				{struct format l,r,b2,b1,d;
					s=fpop(ft1);
					fonchange(s);
					l=s;r=s;b1=s;b2=s;d=s;
					l.parent=ft1.top;
					//r.parent=l.parent;
					l.kind=40;r.kind=40;b1.kind=12;d.kind=12;b2.kind=12;
					//b2=s;//继承
					b1.parent=l.parent;
					d.parent=b1.parent;
					r.parent=d.parent;

					fpush(ft1,b2);
					fpush(ft1,d);
					fpush(ft1,r);
					fpush(ft1,b1);
					fpush(ft1,l);
				}
			grammarprinter(s1,p,ch);
			return 1;
		} 
		if(type==0||type==9||type==7){
			/*B->null*/
			 pop(s1);
			 	{	s=fpop(ft1);
			 		fonpop(s,-1);
			 	}
			ch="B->null";
			grammarprinter(s1,p,ch);
			return 1;
		}
		return -1;
	}
	
	
	if(top=='D'){
		if(type==0||type==4||type==5||type==7||type==9||type==1||type==2){
			/*D->kong*/
			 pop(s1);
			 	 {s=fpop(ft1);
			 		fonpop(s,-1);
			 	 }
			ch="D->null";
			grammarprinter(s1,p,ch);
			return 1; 
		}
		if(type==11){
			/*D->^T*/
			pop(s1); 
			push(s1,'T');
			push(s1,'^');
				{s=fpop(ft1);
				 fonchange(s);
				 fonpop(s,-1);
				 s.size=s.size*3/5;
				 s.top=s.top-s.size/2;
				 fpush(ft1,s);
				}
			ch="D->^T";
			grammarprinter(s1,p,ch);
			return 1; 
		}
		if(type==10){
			/*D->_T*/
			pop(s1); 
			push(s1,'T');
			push(s1,'_');
				{s=fpop(ft1);
					fonchange(s);
					fonpop(s,-1);
				 s.size=s.size*3/5;
				 s.top=s.top+s.size*5/4;
				 fpush(ft1,s);
				}
			ch="D->_T";
			grammarprinter(s1,p,ch);
			return 1; 
		}
		if(type==12){
			/*D->_^TT*/
			pop(s1); 
			push(s1,'T');
			push(s1,'T');
			push(s1,'^');
			push(s1,'_');
				{struct format t1,t2;
				 s=fpop(ft1);
				 fonchange(s);
				 t1=s;t2=s;
				  t1.size=t1.size*3/5;
				 t1.top=t1.top+t1.size*5/4;
				 t1.kind=12;
				  t2.size=t2.size*3/5;
				 t2.top=t2.top-t2.size/2;
				 t2.kind=32;
				 fpush(ft1,t2);
				 fpush(ft1,t1);
				}
			ch="D->_^TT";
			grammarprinter(s1,p,ch);
			pop(s1);
			pop(s1);
			p++;
			ch="shift-in as expected"; 
			grammarprinter(s1,p,ch);
			return 1; 
		}
		if(type==3){
		/* D->KD*/
			push(s1,'K'); 
			ch="D->KD";
				{s=fpop(ft1);
			 	struct format k;
			 	fonchange(s);
			 	k=s;
			 	k.kind=50;
			 	k.parent=ft1.top;
			 	fpush(ft1,s);
			 	fpush(ft1,k);
				}
			grammarprinter(s1,p,ch);
			return 1;
		}
	return -1;
	}

	if(top=='N'){
		if(type==1){
				{s=fpop(ft1);
				s.style=0;
				printhtml(s,p);
				fonpop(s,p);
				}  
			pop(s1);
			p++;
			ch="shift-reduce and in as N->num";
			grammarprinter(s1,p,ch);
			return 1;
		}
	return -1;
	}
	
	
	if(top=='V'){
		if(type==2){
			/*V->vec*/
			{	s=fpop(ft1);
				s.style=1;
				printhtml(s,p);
				fonpop(s,p);
			}
			pop(s1);
			p++;
			grammarprinter(s1,p,"shift-reduce and in as V->vec");
			return 1;
		}

	return -1;
	}
	if(top=='T'){
		if(type==8){
			/*T->{B}*/
			pop(s1);
			push(s1,'}');
			push(s1,'B');
			push(s1,'{');
			{	s=fpop(ft1);
				fonchange(s);
				fpush(ft1,s);
				}
			ch="T->{B}" ;

			grammarprinter(s1,p,ch);
			return 1;	
			}

		
		if(type==3){
		/* T->KT*/
			push(s1,'K'); 
			ch="T->KT";
				{s=fpop(ft1);
			 	struct format k;
			 	fonchange(s);
			 	k=s;
			 	k.kind=50;
			 	k.parent=ft1.top;
			 	fpush(ft1,s);
			 	fpush(ft1,k);}
			grammarprinter(s1,p,ch);
			return 1;
		}
	return -1;
	}
	 
	if(top=='I'){
		if(type==4){
			/*I->\int*/
				{s=fpop(ft1);
				printhtml(s,p);
				} 
			pop(s1);
			p++;
			grammarprinter(s1,p,"shift-reduce and in as I->\\int");
			return 1;
		}
	
	return -1;
	}
	
	
	if(top=='U'){
		if(type==5){
			/*U->\sum*/ 
				{s=fpop(ft1);
				printhtml(s,p);
				}
			pop(s1);
			p++;
			grammarprinter(s1,p,"shift-reduce and in as U->\\sum");
			return 1;
		}

	return -1;
	}
	
	
	if(top=='K') {
		if(type==3){
		/* K->\space*/
			pop(s1);
			p++; 
			ch="shift-reduce and in as K->\\space";
			grammarprinter(s1,p,ch);
				{s=fpop(ft1);
				printhtml(s,p);
				fonpop(s,p);
				return 1;
				}
		}
	}return 0; 
} 

static ofstream out("E://program/output11.html");
int grammarenter(struct stacklist &s1,int &p){
	/*移进过程*/
	int top=s1.top;
	char *ch,e;
	e=pop(s1);
	
	if(e==id1[p][0])
		{if(e=='('||e==')'){
					struct format s;
					s=fpop(ft1);
					fonchange(s);
					printhtml(s,p);
					fonpop(s,p);
				}
			p++;		
		ch="shift-in as expected"; 
		grammarprinter(s1,p,ch);
		return 1;
		}
		return -1;
}


int grammarscanner(struct stacklist &s1){
	/*语法分析器主过程*/
	cout<<endl<<s1.stack<<endl;
	int p=0;
	int i=0;
	int k2=0;
	int a=id1num;
	while(p!=a){
		i++;
		int k=grammarredu(s1,id1type[p],p);
		if(k==-1)	
			{cout<<"error 02 错误的归约式"<<endl;return -1;}
		if(k==0) { k2=grammarenter(s1,p);
		}
		if(k2==-1) cout<<"error 03 错误的输入 预期输入'"<<id1[p]<<"'未找到"<<endl;
	
	}
	
} 

/*语法制导输出过程*/ 
int printhtml(struct format f1,int p){
	char *ch;
	int left;
	out.seekp(0,ios::end);
	if(f1.style==1) ch="oblique";
	else ch="normal";
	if(f1.kind==22||f1.kind==21||f1.kind==50)	{string m;
		if(f1.kind==22)	m="∑";
		else if(f1.kind==21) m="∫";
        else if(f1.kind==50) m=" ";
	cout<<left;
	out<<"<div style=\"position: absolute; top:"
	<<f1.top<<"px; left:"
	<<f1.left<<"px;\">"
	<<"<span style=\"font-size:"<<f1.size
	<<"px; font-style:"<<
	ch<<"; line-height:100%;\">"<<m
	<<"</span></div>"<<endl;	return 0;}
	if(f1.kind<=31)	left=ft1.ftlist[f1.parent].left;
	if(f1.kind==32||f1.kind==40) left=f1.left;
	out<<"<div style=\"position: absolute; top:"
	<<f1.top<<"px; left:"
	<<left<<"px;\">"
	<<"<span style=\"font-size:"<<f1.size
	<<"px; font-style:"<<ch
	<<"; line-height:100%;\">"<<id1[p]
	<<"</span></div>"<<endl;
}


int createhtml(struct stacklist &s1){
	using namespace std;
	
    if(!out){			//文件打开失败
        cout << "Failed to create file...\n";
        exit(0);
    }
    out <<"<html>"<<endl<<"<head>"<<endl<<"<META content=\"text/html; charset=gb2312\">"<<endl<<"</head>"<<endl<<"<body>"<<endl; //ÃÃ²ÃŽÃ„Â¼Ã¾ÃÂ´ÃŠÃ½Â¾Ã
   	grammarscanner(s1);
    out <<"</body>"<<endl<<"</html>"<<endl;
    out.close();		//文件关闭
    return 0;
}
/*0Ã–Ã•Ã–Â¹Â·Ã» 1ÃŠÃ½Ã—Ã–Â³Â£ÃÂ¿ 2Ã—Ã–Ã„Â¸ÃŠÃ½Ã—Ã–Â»Ã¬ÂºÃÂ±Ã¤ÃÂ¿ 3/blank 4/int 5/sum 6( 7) 8{ 9} 10_ 11^ 12_^ */
int main(){
	int p;									//扫描指针
	p=0;
	FILE *fp,*fp1;
	char resource[1000];					//词法扫描存储
	char token[100]; 
	if(!(fp=fopen("E://program/input11.txt","r"))) {cout << "Open file failed!";exit(0);}
	resource[p]=fgetc(fp);
	do{	p++;
		resource[p]=fgetc(fp);}
		while(resource[p]!='$');			
		cout<<resource;
	fclose(fp);
	p=0;
	if(resource[p]!='$') {cout<<"error 00 字符流未以$开头"<<endl;}
	p++;
	cout<<endl;
	id1[0][0]='$';id1[0][1]='\0';
	id1type[0]=0;
	/*词法分析器主过程*/ 
	for(int i=1;i<100;i++){
	id1type[i]=scanner(p,resource,token); 
	strcpy(id1[i],token);
	cout<<id1[i]<<","<<id1type[i]<<endl;
	if(id1type[i]==0) 
	{id1num=i+1;break;}
	if(id1type[i]==-1)
	{cout<<"error 01 不合法的词法流"<<endl;return 0;}
	}

	for(int i=0;i<p;i++){
		cout<<id1[i];
	}
	struct stacklist s1;
	s1.top=0;
	s1.stack[0]='S';
	s1.stack[1]='\0';
	ft1.top=0;
setlocale(LC_ALL,"Greek");
	createhtml(s1);
}
