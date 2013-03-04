#include<stdio.h>
#include<math.h>
#define RN			(rand^=rt[3&rand>>2]^=~rand>>4,rand*=11)
typedef int(*Q)();
int wav(),cpu(),cmp();
Q rr[]={(Q)fopen,(Q)printf,(Q)wav,NULL};
Q g;
struct{Q*fork;int c;}RR[2];
FILE *f;
unsigned char mel[1291]="(L(J(L(N(O(N(L(J(L(J(H(GPEPE",
em[1244]="tK(uL+zQ.V1\3Z4\5\\7\6]:\a^=\fc@\rdC\16eF\20gi1hl4k'",
smem[44]="RIFFWAVEfmt \20\0\0\0\1\0\1\0D\254\0\0D\254\0\0\1\0\10\0data",
mem[65]="adbfPIUGSpbadfpIGSsdpgiBSPGISGSSg",
stack[256]=
//"\003j#k\355\231\267SK\335\351\023zC",
"\007\253*k\355\232T\004K\335\2501zC\004",
//"\370R$\002V$*\002T\003$\001ZB\004\012T\003\000",
/*"\370R$\002V$\177\002T\003$\001ZB\004\012T\003\000",
"\370R$\002V$\177\002T\003$\001ZBT\003\000",*/
*stp,*ip,*loopi[256];
int be=1,loopr[256],loopc,ist[241],rand=1,rt[4],main(),R(),l;
R(){return 1;}
void decrypt(char*p,int i)
{
l^=l;
while(l<i)
	*p+++=++l^31;
}
void encrypt(char*p,int i)
{
l^=l;
	while(i>l)
	*p++-=++l^31;
}
void esc(unsigned char*p)
{
while(*p){if(*p>31&&*p<128)putchar(*p);else printf("\\%03o",*p);p++;}
puts("");
}
wav(a,b)
{
	unsigned x,size=0,rate,tempo=200,note,len=1;
	unsigned char *v,F[]="s.wav",c;
	for(v=a;*v;size+=tempo**v,v+=2);
F[0]=b+97;
	f=(FILE*)rr[0](F,"wb"),
	fwrite(smem,4,1,f);
	ip=&x;x=size+36;
	main(8,fwrite);
	fwrite(smem+4,1<<5,1,f);x=size;
	main(8,fwrite);
	
	rate = 44100;
	v=a-1;
	while(size--)
	{
		--len||(
		len=tempo**++v,
		note = (int)(.1*
pow(1.1225,.5*(*++v%60-30))*rate));
		c = (int)128+(97*sin(1.*note*size/rate));
		fwrite(&c,1,1,f);
	}
	fclose(f);
}
cpu(){
int r1,r2,lr=0,d=0,reg[5];
loopc=0;
while(1)
{
//printf("d=%5i%*sr=%i r1=%i r2=%i lr=%i r[lr]=%i stp=%i *stp=%i\n",d,loopc,"",loopr[loopc-1],reg[4],r2,lr,reg[lr],stp-stack,*stp);
*stp&1?/*mem*/
	r1=*stp>>2,
	*stp&2?/*store*/
		mem[r1]=reg[lr]+1
	:/*load*/
		(reg[4]=mem[r1]-1)
:
	*stp&2?/*math*/
		lr=r1=*stp>>4&3,r2=*stp>>6,r2=r1^r2?r2:4,
		*stp&4?/*mul/div*/
			*stp&8?
				reg[r1]/=reg[r2]
			:
				(reg[r1]*=reg[r2])
		:/*add/set*/
			*stp&8?
				reg[r1]+=reg[r2]
			:
				(reg[r1]=reg[r2])
	:/*misc*/
		*stp&4?
			*stp&8?/*cmp*/
				r2=*stp>>4,r1=reg[*stp>>6],r1=
				(r2&16?
					r2&32?/*eq*/
						r1==reg[lr]
					:
						r1!=reg[lr]
				:
					r2&32?/*lt*/
						r1<reg[lr]
					:/*gt*/
						r1>reg[lr]),
				reg[4]=r1
			:
				*stp&16?
					r2=*stp>>6,
					*stp&32?/*branch*/
						stp=(stp+!!reg[4]*r2-stack&255)+stack
					:/*inst write*/
						(stp[1]=!!reg[r2]*4+stp[1]%4)
				:
					(lr=4),
					*stp&32?/*const*/
						reg[4]=*++stp
					:/*rand*/
						(reg[4]=RN>>7)
		:/*loop*/
			(r1=*stp>>3,
			r1?/*push a loop count*/
				reg[4]=loopr[loopc]=++r1,
				loopi[loopc++]=stp
			:/*pop*/
				loopc&&(reg[4]=--loopr[--loopc])&&
				(stp=loopi[loopc++]))
;
if(loopc==0&&(loopc==*stp||stp>stack+256&&stp--)||d++>1000)break;
stp++;
}
//for(r1=0;mem[r1];r1++)printf("%i,",mem[r1]);
}
int cmp(a,b)char*a,*b;{
int patt[144],x,p,n[2],lm=a[1],t,key[24],step[24],rhy[130];
for(x=0;x<48;x++)
{key[x/2]=step[x/2]=patt[x*3]=patt[x*3+1]=patt[x*3+2]=n[0]=n[1]=0;
}for(x=0;x<64;x++)rhy[x<<1]=rhy[(x<<1)+1]=0;
	t=0;p=1;
	for(x=0;x<2;x++)
	{
		while(*a){
			rhy[t++*2+x]=*a;
			key[a[1]%12*2+x]+=*a>>2;
			step[abs(a[1]-lm)%12*2+x]+=*a>>2;
			patt[a[1]%12*12+lm%12]+=-p;
			lm=1[a+=2];
			n[x]++;
/*printf("x=%i n=%i %c\n",x,n[x],*a);
*/
		}
a=b,p*=-1;
	}
t=10000;
	for (x=0;x<64;x++)t+=rhy[x<<1]+rhy[(x<<1)+1];
	for(x=0;x<144;x++)t+=-abs(patt[x])*20;
	for(x=0;x<24;x++){
		t+=(x%2-!(x&1))*(-key[x]+step[x]*2)*100/n[x&1];
/*		t+=key[x]/n[0]-key[x+1]/n[1];
		t+=step[x]/n[0]-step[x+1]/n[1];
*/
	}
return t+100*strlen(b);
}

main(a,b){
int best,this;
char data[1232],data2[2343];
rand=getpid()+343;rt[0]=107,rt[1]=41,rt[2]=37,rt[3]=29;
if (a==1)
{
for(a=0;a<256;a++)data2[a]=0;
for(a=0;a<10000;a++)data2[(unsigned char)RN]++;
for(a=0;a<256;a++)printf("%3i=%i\n",a,data2[a]);
ip=(char*)&be;be=!!*ip-!*ip;
g=wav;
g(mel,0);
best=10000000;
for (a=1;a<1000000;a++)
{
	memset(mem,0,65);
	stp=stack,cpu();
	if ((this=cmp(mem,mel))<best)
	{
		memcpy(data,stack,64);
		best=this;
	esc(data);
		printf("data=%i\n",best);
	}
	if (a%10000==0)
	{
	}
	/*Mutate*/
	memcpy(stack,data,64);
if(RN&1)

	memmove(stack+(RN>>4&63),stack+(RN>>4&63)+25,(RN&3)+1);
else{	stack[RN>>7&63]^=RN>>6;
	stack[RN>>6&63]+=RN>>7&7;
	stack[RN>>7&63]-=RN>>8&7;
	stack[RN>>7&63]^=RN>>5&7;
}}
	memset(mem,0,65);
stp=stack,
	memcpy(stack,data,64);
cpu();
g(mem,1);
g=printf,g("cmp=%i\n",cmp(mem,mel));
/*RR[0].fork=rr;
fork()[fork()[RR].fork]();
*/
}
else
(a<12&&a>4&&((Q)b)(ip+a-8+3*(be-1>>1&1),be&1,1,f)&&main(a+be,b));
return 0;}
