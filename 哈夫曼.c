/*ѹ���ͽ�ѹ�ļ���Ҫ��ȡ���ָ�����ʽ���ļ�
���Բ��ö����Ʒ�ʽ��ȡ�ļ� 
��һ���޷����ַ��ĳ���Ϊ����λ,����0~255��256���ַ�*/ 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXINT 32767
typedef struct{
	unsigned char ch;			//�洢��Ӧ�ַ���Ϣ 
	unsigned long weight;		//�ַ����ֵ�Ƶ��(��Ҷ�ӽ���Ȩ��) 
}TmpNode;

typedef struct{
	unsigned char ch;
	unsigned long weight;
	char *huffmanCode;					//�ַ���Ӧ�Ĺ��������� 
	int parent,lChild,rChild; 
}HTNode,*huffmanTree; 

////ѡ��˫��Ϊ0��Ȩֵ��С�����ڵ�s1,s2
void select(huffmanTree huf_tree,unsigned int n,int *s1,int *s2){
	// ����С
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for(i = 0; i < n; ++i)           
		if(huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s1 = i;
		}
		huf_tree[*s1].parent=1;   // ��Ǵ˽���ѱ�ѡ��

	// �Ҵ�С
	min=ULONG_MAX;
	for(i = 0; i < n; ++i)            
		if(huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s2 = i;
		}
} 

//������������
void crtHuffmanTree(huffmanTree ht , int n) 
{
	int i,s1,s2;
	int m = 2*n-1;
	
	// ��n+1��Ԫ�ؿ�ʼ�����½�� 
	for(i=n;i<m;i++){
		//��ǰi����ѡȡ˫��Ϊ0��Ȩֵ��С��������� 
		select(ht,i,&s1,&s2);
		ht[s1].parent = i;
		ht[s2].parent = i;
		ht[i].lChild = s1;
		ht[i].rChild = s2;
		ht[i].weight = ht[s1].weight + ht[s2].weight;
		ht[i].parent = 0;
	}
}


////���й���������(��Ҷ�ӽ�㵽���ڵ㣬�������Ҷ�ӽ��Ĺ���������) 
void crtHuffmanCode(huffmanTree ht , unsigned int n){
	char *cd;
	int i,start,c,p,l=0;
	cd = (char *)malloc(256 * sizeof(char));	//������ʱ�洢�� 
	cd[256-1] = '\0';		//���һλ�洢���������
	
	
	for(i=0;i<n;i++){
//		printf("\n%d",l++);
		start = 256-1;
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
		ht[i].huffmanCode = (char *)malloc((256-start) * sizeof(char));
		strcpy(ht[i].huffmanCode , &cd[start]);		//���Ʊ��� 
	}
	free(cd); 			//�ͷű�����ʱ�ռ� 

} 



void compress(char *originFile , char *newFile){
	int i,j,k;
	unsigned int n = 0;					//�ַ����� 
	unsigned int m;						//�������н���� 
	unsigned char char_temp;	//�ַ���ʱ�洢
	char code_buf[256] = "\0";	//������ʱ�洢�ռ� (�ڵ������256��������������256�����볤�����256) 
	FILE *origin,*news,*infile;	//ѹ���ļ��ͽ�ѹ�ļ� 
	TmpNode t;
	huffmanTree ht;
	unsigned long file_length=0;			//�ļ����ַ����� 
	
	TmpNode *tmp_nodes =(TmpNode *)malloc(256*sizeof(TmpNode));	
	
		// ��ʼ���ݴ���
	for(i = 0; i < 256; ++i)
	{
		tmp_nodes[i].weight = 0;
		tmp_nodes[i].ch = (unsigned char)i;		// �����256���±���256���ַ���Ӧ
	}


//�����ļ���ȡ���ַ�����Ӧ��Ƶ�ȣ���Ҷ�ӽ���Ȩֵ
	origin = fopen(originFile, "rb");	//���ö����Ʒ�ʽ��ȡ�ļ� 
	// �ж������ļ��Ƿ����
	if (origin == NULL)
		printf("��ȡ�ļ�����ʧ��");
	fread((char *)&char_temp, sizeof(unsigned char), 1, origin);		// ����һ���ַ�
	while(!feof(origin))
	{
		tmp_nodes[(unsigned int)char_temp].weight++;			//�±��Ӧ���ַ���Ȩ�� 
		file_length++;
		fread((char *)&char_temp, sizeof(unsigned char), 1, origin);		// ����һ���ַ�
	}
	fclose(origin);


	//����ʱ�洢���Ļ���������ɸѡ ������Ƶ�Ƚ������򣬽�Ƶ�Ȳ�Ϊ0�ı�������ǰ�� 
	for(i = 0; i < 256-1; ++i){
		for(j = i+1; j < 256; ++j){
		
			if(tmp_nodes[i].weight < tmp_nodes[j].weight)
			{
				t = tmp_nodes[i];
				tmp_nodes[i] = tmp_nodes[j];
				tmp_nodes[j] = t;
			}
		}
	}
	
	//ͳ��Ƶ�Ȳ�Ϊ0�Ľ��(��Ҷ�ӽ��) 
	for(i=0;i<256;i++){
		if(tmp_nodes[i].weight != 0)
			n++;
	}
	//������� 
	m = 2*n-1;

	ht = (huffmanTree)malloc(m * sizeof(HTNode));	//��̬������������������
	
	//��ʼ��Ҷ�ӽ��
	for(i=0;i<n;i++){
		ht[i].ch = tmp_nodes[i].ch;
		ht[i].weight = tmp_nodes[i].weight;
		ht[i].parent = 0;
	} 

	//�ͷ���ʱ�洢����ڴ� 
	free(tmp_nodes);	
	crtHuffmanTree(ht , n);

	crtHuffmanCode(ht , n);
	
	//д���ַ�����Ӧ��Ƶ��(����ѹ�ļ�ʱʹ��)
	if((news = fopen(newFile , "wb"))== NULL){
		printf("\nд�ļ�����ʧ��"); 
	}
	else{
		fwrite((char *)&n , sizeof(unsigned int) , 1 , news);					//д���ַ������� 
		for(i=0;i<n;i++){
			fwrite((char *)&ht[i].ch , sizeof(unsigned char) , 1 , news);		//д���ַ� 
			fwrite((char *)&ht[i].weight , sizeof(unsigned long) , 1 , news);				//д��Ȩ�� 
		}
		
		fwrite((char *)&file_length , sizeof(unsigned long) , 1 , news);		//д���ļ����ݵ��ַ����� 



		origin = fopen(originFile , "rb");
		fread((char *)&char_temp , sizeof(unsigned char) , 1 ,origin);
		while(!feof(origin)){
		
			//���ļ��е��ַ��������е��ַ���ƥ�� 
			for(i=0;i<n;i++){
				//��ƥ�䣬�򼴽���������ַ���Ӧ�ı�����ӵ���ʱ����ռ��� 
				if(char_temp == ht[i].ch)
					strcat(code_buf , ht[i].huffmanCode);
			}
			
			//һ���ֽ�(8λ)Ϊiһ������λ 
			while(strlen(code_buf) >=8){
				
				char_temp = '\0';	//����ַ���ʱ�洢����Ϊ�ַ�������ʱ�洢�ռ� 
				for(i=0;i<8;i++){
					char_temp <<= 1;	//����ʱ����ռ�����1λ
					if(code_buf[i] == '1')
						char_temp |= 1;	//�������Ϊ'1'��ͨ�������������ӵ��ֽڵ����λ 
				}
				fwrite((char *)&char_temp , sizeof(unsigned char) , 1 , news);
				strcpy(code_buf , code_buf+8);	//���뻺������ǰ8λ 
			}
			fread((char *)&char_temp , sizeof(unsigned char) , 1 ,origin);
		}
		
		//�ļ����������8λ����
		if(strlen(code_buf) >0 && strlen(code_buf) <8){
			char_temp = '0';
			for(i=0;i<strlen(code_buf);i++){
				char_temp <<= 1;
				if(code_buf[i] == '1'){
					char_temp |= 1;
				}
			}
			char_temp <<= 8-strlen(code_buf);		//�������ֶδ�β���Ƶ��ֽڵĸ�λ 
			fwrite((char *)&char_temp , sizeof(unsigned char) , 1 , news);
		} 
		fclose(origin);
		fclose(news);
	}
	for(i=0;i<n;i++){
		free(ht[i].huffmanCode);
	}
	free(ht);
}



//void extract(char *originFile , char *newFile){
//	unsigned int i,n,m;
//	unsigned long file_length;			//�ļ����� 
//	unsigned long write_length = 0;		//д�볤��
//	FILE *origin , *news;
//	huffmanTree ht;
//	unsigned char code_temp;		//������ʱ�洢
//	unsigned int root;				//���и������±� 
//	
//	if((origin = fopen(originFile , "rb")) == NULL){
//		printf("\n�ļ���ȡʧ��");
//	} 
//	else{
//		//��ȡ�ַ������� 
//		fread((char *)&n , sizeof(unsigned int) , 1 , origin);
////		printf("%d",n);
//		m = 2*n-1;
//		ht = (HTNode *)malloc(m * sizeof(HTNode));
//		for(i=0;i<n;i++){
//			//��ȡ��Ӧ���ַ���Ȩ�� 
//			fread((char *)&ht[i].ch , sizeof(unsigned char) , 1 , origin);
//			fread((char *)&ht[i].weight , sizeof(unsigned long) , 1 , origin);
//			ht[i].parent = 0; 
////			printf("%c",ht[i].ch);
//		} 
//
//		//�ؽ��������� 
//		crtHuffmanTree(ht , n);
//		
//		
//		//��ȡ�ļ�����(��Ϊfeof�����ж϶������ļ��Ľ���λ�ã�����ͨ���ļ��������ж�) 
//		fread((char *)&file_length , sizeof(unsigned long) , 1 , origin);
//	
//		if((news = fopen(newFile , "wb")) == NULL){
//			printf("\nд���ļ�����ʧ��");
//		} 
//		else{
//			//���ڵ� 
//			root = m-1;
//			while(1){
//				//��ȡһ���ֽڵı��� 
//				fread((char *)&code_temp , sizeof(unsigned char) , 1 , origin);
//				
//				 for(i=0;i<8;i++){
//				 	//�Ӹ���Ҷ�ӽ�����±��� 
//				 	if(code_temp & 128){
//				 		root = ht[root].rChild;
//					}
//					else{
//						root = ht[root].lChild;
//					}
//					if(root < n){
//						fwrite((char *)&code_temp  ,sizeof(unsigned char) , 1 , news);
//						write_length++;
//						if(write_length == file_length){
//							break;
//						} 
//						root = m-1;			//�ص����ڵ㣬�ٴν��б��� 
//					}
//					code_temp <<= 1; 		//��������һλ�� 
//				 }
//				 if(write_length == file_length)
//				 	break;
//			}		
//		}
//		fclose(origin);
//		fclose(news);
//	}
//	free(ht);
//}




// ��ѹ����
void extract(char *originFile, char *newFile)
{
	unsigned int i;
	unsigned long file_len;
	unsigned long writen_len = 0;		// �����ļ�д�볤��
	FILE *origin, *news;
	unsigned int char_kinds;		// �洢�ַ�����
	unsigned int node_num;
	huffmanTree huf_tree;
	unsigned char code_temp;		// �ݴ�8bits����
	unsigned int root;		// ������ڵ���������ƥ�����ʹ��

	origin = fopen(originFile, "rb");		// �Զ����Ʒ�ʽ��ѹ���ļ�
	// �ж������ļ��Ƿ����


	// ��ȡѹ���ļ�ǰ�˵��ַ�����Ӧ���룬�����ؽ���������
	fread((char *)&char_kinds, sizeof(unsigned int), 1, origin);     // ��ȡ�ַ�������
	

		node_num = 2 * char_kinds - 1;		// �����ַ������������㽨������������������ 
		huf_tree = (huffmanTree)malloc(node_num*sizeof(HTNode));		// ��̬��������������ռ�
		// ��ȡ�ַ�����ӦȨ�أ�������������ڵ�
		for(i = 0; i < char_kinds; ++i)     
		{
			fread((char *)&huf_tree[i].ch, sizeof(unsigned char), 1, origin);		// �����ַ�
			fread((char *)&huf_tree[i].weight, sizeof(unsigned long), 1, origin);	// �����ַ���ӦȨ��
			huf_tree[i].parent = 0;
		}
		// ��ʼ����node_num-char_kins������parent
		for(; i < node_num; ++i) 
			huf_tree[i].parent = 0;

		crtHuffmanTree(huf_tree, char_kinds);		// �ؽ�������������ѹ��ʱ��һ�£�

		// �����ַ���Ȩ����Ϣ�������Ŷ�ȡ�ļ����Ⱥͱ��룬���н���
		fread((char *)&file_len, sizeof(unsigned long), 1, origin);	// �����ļ�����
		news = fopen(newFile, "wb");		// ��ѹ�������ɵ��ļ�
		root = node_num-1;
		while(1)
		{
			fread((char *)&code_temp, sizeof(unsigned char), 1, origin);		// ��ȡһ���ַ����ȵı���

			// �����ȡ��һ���ַ����ȵı��루ͨ��Ϊ8λ��
			for(i = 0; i < 8; ++i)
			{
				// �ɸ�����ֱ��Ҷ�ڵ�����ƥ������Ӧ�ַ�
				if(code_temp & 128)
					root = huf_tree[root].rChild;
				else
					root = huf_tree[root].lChild;

				if(root < char_kinds)
				{
					fwrite((char *)&huf_tree[root].ch, sizeof(unsigned char), 1, news);
					++writen_len;
					if (writen_len == file_len) break;		// �����ļ����ȣ������ڲ�ѭ��
					root = node_num-1;        // ��λΪ��������ƥ����һ���ַ�
				}
				code_temp <<= 1;		// �����뻺�����һλ�Ƶ����λ����ƥ��
			}
			if (writen_len == file_len) break;		// �����ļ����ȣ��������ѭ��
		}

		// �ر��ļ�
		fclose(origin);
		fclose(news);

		// �ͷ��ڴ�
		free(huf_tree);
}//extract()





void main(){
	char originFile[200],newFile[200];	//��ѹ����ѹ������ļ���
	int i,j=1;
	while(j){
		system("cls"); 
		printf("\n=========================================================================================================\n");
		printf("|					      ������ѹ��/��ѹ��	 					|");
		printf("\n|													|");
		printf("\n|						1. ѹ���ļ�						|");
		printf("\n|						2. ��ѹ�ļ�						|");
		printf("\n|						3.  �˳�						|");
		printf("\n|													|");
		printf("\n=========================================================================================================\n");
		printf("\n�������������:");
		scanf("%d",&i);
		switch(i){
			case 1:{
				system("cls");
				printf("�������ѹ���ļ��ĵ�ַ:");
				scanf("%s",originFile); 
				printf("\n������ѹ�����ļ��ı����ַ:");
				scanf("%s",newFile);
				printf("\n����ѹ�������Ժ�");
				for(i=0;i<3;i++){
					printf(".");
					Sleep(1000);
					printf(".");
				}
				
				compress(originFile , newFile);
				
				printf("\n\nѹ�����");
				Sleep(1000);
				break;
			}
			case 2:{
				system("cls");
				printf("���������ѹ���ļ���ַ:");
				scanf("%s",originFile);
				printf("\n�������ѹ���ļ�����ĵ�ַ:");
				fflush(stdin);
				scanf("%s",newFile);
				
				printf("\n���ڽ�ѹ�����Ժ�");
				for(i=0;i<3;i++){
					printf(".");
					Sleep(1000);
					printf(".");
				}
				
				extract(originFile , newFile);
				printf("\n\n��ѹ���");
				Sleep(2000);
				break;
			}
			case 3:
			default:{
				j = 0;
				break;
			}
		} 
	}
}
