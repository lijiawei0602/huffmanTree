#include<stdio.h>
#include<stdlib.h>
#define MAXINT 32767 
typedef struct{
	int weight;
	int parent,lChild,rChild;
	char ch[256][256];
}HTNode;

int character[256][256] = {0};		//汉字 
int character1[MAXINT];
int  p[128] = {0};					//其他字符 
int p1[128];
int num,numCh=0,numP=0;							//字符的种树（叶子结点的个数） 

typedef char * huffmanCode[MAXINT];
//哈夫曼树叶子结点的个数(频度作为权值)
void numOfChar(char url[]){
	FILE *fp;
	int ch;
	char a[MAXINT];
	int i=0,j;

	if((fp=fopen(url,"r"))==NULL){
		printf("\n读取文件失败");
	} 
	else{
		while((ch=fgetc(fp))!=EOF){
			a[i]=(char)ch;
			i++;
		}
		for(i=0;i<strlen(a);i++){
			if((int)a[i]<0){
				character[-(int)a[i]][-(int)a[i+1]]++;
				i++;
			}
			else{
				p[(int)a[i]]++;
			}
		}
		
		int m = 0;
		//汉字对应的种数 
		for(i=0;i<256;i++){
			for(j=0;j<256;j++){
				if(character[i][j] != 0){
					numCh++;
					character1[m] = character[i][j];
					m++;
				}
			}
		}
		
		int n = 0;
		//其他字符对应的种数 
		for(i=0;i<158;i++){
			if(p[i] !=0){
				numP++;
				p1[n] = p[i];
				n++;
			}
		}
		num = numCh + numP;
	}
	fclose(fp);
} 



//构建哈夫曼树
void crtHuffmanTree(HTNode ht[], int w1[], int w2[] , int n) 
{
	int i,s1,s2;
	int m = 2*n-1;
	
	//初始化前n个根节点 
	for(i=1;i<=numCh;i++){
		ht[i].weight = w1[i-1];
		ht[i].parent = 0;
		ht[i].lChild = 0;
		ht[i].rChild = 0;
	}
	for(i=numCh+1;i<=n;i++){
		ht[i].weight = w2[i-numCh-1];
		ht[i].parent = 0;
		ht[i].lChild = 0;
		ht[i].rChild = 0;
	}
	
	//初始化后n-1个非根节点 
	for(i=n+1;i<=m;i++){
		ht[i].weight = 0;
		ht[i].parent = 0;
		ht[i].lChild = 0;
		ht[i].rChild = 0;
	}
	 
	// 从n+1个元素开始构造新结点 
	for(i=n+1;i<=m;i++){
		//在前i-1项中选取双亲为0且权值最小的两个结点 
		select(ht,i-1,&s1,&s2);
		ht[i].weight = ht[s1].weight + ht[s2].weight;
		ht[i].lChild = s1;
		ht[i].rChild = s2;
		ht[s1].parent = i;
		ht[s2].parent = i;
	}
}

//选择双亲为0且权值最小的两节点s1,s2
void select(HTNode ht[],int pos,int *s1,int *s2){
	//m1存放最小权值，s1是m1在数组中的下标
	//m2存放此小权值，s2是m2在数组中的下标 
	int i,j,m1,m2;
	m1=m2=MAXINT;
	for(j=1;j<=pos;j++){
		if(ht[j].weight <m1 && ht[j].parent ==0){
			m2=m1;
			*s2=*s1;
			*s1=j;
			m1=ht[j].weight;
		}
		else if(ht[j].weight<m2 && ht[j].parent ==0){
			m2 = ht[j].weight;
			*s2 = j;
		}
	}
	if(*s1>*s2){
		i = *s1;
		*s1 = *s2;
		*s2 = i;
	}
	
} 

////进行哈夫曼编码(从叶子结点到根节点，逆向求各叶子结点的哈夫曼编码) 
void crtHuffmanCode(HTNode ht[],huffmanCode hc,int n){
	char *cd;
	int i,start,c,p;
	cd = (char *)malloc(n * sizeof(char));	//编码临时存储地 
	cd[n-1] = '\0';		//最后一位存储编码结束码
	for(i=1;i<=n;i++){
		start = n-1;
		c=i;			//c为当前结点 
		p=ht[i].parent;	//p为双亲结点 
		while(p != 0){	//追溯到根节点 
			--start;
			if(ht[p].lChild == c)
				cd[start] = '0';
			else
				cd[start] = '1';
			c = p;
			p = ht[p].parent;
		}
		hc[i] = (char *)malloc((n-start) * sizeof(char));
		strcpy(hc[i] , &cd[start]);		//复制编码 
	}
	free(cd); 			//释放编码临时空间 
} 

//对文件内容进行压缩 
void writeFile(huffmanCode hc , char url[]){
	FILE *fp;
	int i;
} 

////进行哈夫曼解压 
void crthuffmanDecode(HTNode ht[],huffmanCode hc){
	
} 

void main(){
	int i;
	//压缩文件的地址 
	char url[100] = "C:/Users/lijiawei/Desktop/新建文本文档.txt";
	//分析压缩文件的内容 
	numOfChar(url);

	int m = 2*num-1;
	HTNode  ht[m+1];
	huffmanCode hc;
	//创建哈夫曼树 
	crtHuffmanTree(ht , character1 , p1 , num);
	//哈夫曼编码 
	crtHuffmanCode(ht , hc , num);
	for(i=1;i<=num;i++){
		printf("%s\n",hc[i]);
	}
	
}
