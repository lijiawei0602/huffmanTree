#include<stdio.h>
#include<stdlib.h>
#define MAXINT 32767 
typedef struct{
	int weight;
	int parent,lChild,rChild;
	char ch[256][256];
}HTNode;

int character[256][256] = {0};		//���� 
int character1[MAXINT];
int  p[128] = {0};					//�����ַ� 
int p1[128];
int num,numCh=0,numP=0;							//�ַ���������Ҷ�ӽ��ĸ����� 

typedef char * huffmanCode[MAXINT];
//��������Ҷ�ӽ��ĸ���(Ƶ����ΪȨֵ)
void numOfChar(char url[]){
	FILE *fp;
	int ch;
	char a[MAXINT];
	int i=0,j;

	if((fp=fopen(url,"r"))==NULL){
		printf("\n��ȡ�ļ�ʧ��");
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
		//���ֶ�Ӧ������ 
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
		//�����ַ���Ӧ������ 
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



//������������
void crtHuffmanTree(HTNode ht[], int w1[], int w2[] , int n) 
{
	int i,s1,s2;
	int m = 2*n-1;
	
	//��ʼ��ǰn�����ڵ� 
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
	
	//��ʼ����n-1���Ǹ��ڵ� 
	for(i=n+1;i<=m;i++){
		ht[i].weight = 0;
		ht[i].parent = 0;
		ht[i].lChild = 0;
		ht[i].rChild = 0;
	}
	 
	// ��n+1��Ԫ�ؿ�ʼ�����½�� 
	for(i=n+1;i<=m;i++){
		//��ǰi-1����ѡȡ˫��Ϊ0��Ȩֵ��С��������� 
		select(ht,i-1,&s1,&s2);
		ht[i].weight = ht[s1].weight + ht[s2].weight;
		ht[i].lChild = s1;
		ht[i].rChild = s2;
		ht[s1].parent = i;
		ht[s2].parent = i;
	}
}

//ѡ��˫��Ϊ0��Ȩֵ��С�����ڵ�s1,s2
void select(HTNode ht[],int pos,int *s1,int *s2){
	//m1�����СȨֵ��s1��m1�������е��±�
	//m2��Ŵ�СȨֵ��s2��m2�������е��±� 
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

////���й���������(��Ҷ�ӽ�㵽���ڵ㣬�������Ҷ�ӽ��Ĺ���������) 
void crtHuffmanCode(HTNode ht[],huffmanCode hc,int n){
	char *cd;
	int i,start,c,p;
	cd = (char *)malloc(n * sizeof(char));	//������ʱ�洢�� 
	cd[n-1] = '\0';		//���һλ�洢���������
	for(i=1;i<=n;i++){
		start = n-1;
		c=i;			//cΪ��ǰ��� 
		p=ht[i].parent;	//pΪ˫�׽�� 
		while(p != 0){	//׷�ݵ����ڵ� 
			--start;
			if(ht[p].lChild == c)
				cd[start] = '0';
			else
				cd[start] = '1';
			c = p;
			p = ht[p].parent;
		}
		hc[i] = (char *)malloc((n-start) * sizeof(char));
		strcpy(hc[i] , &cd[start]);		//���Ʊ��� 
	}
	free(cd); 			//�ͷű�����ʱ�ռ� 
} 

//���ļ����ݽ���ѹ�� 
void writeFile(huffmanCode hc , char url[]){
	FILE *fp;
	int i;
} 

////���й�������ѹ 
void crthuffmanDecode(HTNode ht[],huffmanCode hc){
	
} 

void main(){
	int i;
	//ѹ���ļ��ĵ�ַ 
	char url[100] = "C:/Users/lijiawei/Desktop/�½��ı��ĵ�.txt";
	//����ѹ���ļ������� 
	numOfChar(url);

	int m = 2*num-1;
	HTNode  ht[m+1];
	huffmanCode hc;
	//������������ 
	crtHuffmanTree(ht , character1 , p1 , num);
	//���������� 
	crtHuffmanCode(ht , hc , num);
	for(i=1;i<=num;i++){
		printf("%s\n",hc[i]);
	}
	
}
