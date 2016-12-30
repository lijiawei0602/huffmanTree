/*压缩和解压文件需要读取各种各样格式的文件
所以采用二进制方式读取文件 
以一个无符号字符的长度为处理单位,共有0~255即256种字符*/ 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXINT 32767
typedef struct{
	unsigned char ch;			//存储对应字符信息 
	unsigned long weight;		//字符出现的频度(即叶子结点的权重) 
}TmpNode;

typedef struct{
	unsigned char ch;
	unsigned long weight;
	char *huffmanCode;					//字符对应的哈夫曼编码 
	int parent,lChild,rChild; 
}HTNode,*huffmanTree; 

////选择双亲为0且权值最小的两节点s1,s2
void select(huffmanTree huf_tree,unsigned int n,int *s1,int *s2){
	// 找最小
	unsigned int i;
	unsigned long min = ULONG_MAX;
	for(i = 0; i < n; ++i)           
		if(huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s1 = i;
		}
		huf_tree[*s1].parent=1;   // 标记此结点已被选中

	// 找次小
	min=ULONG_MAX;
	for(i = 0; i < n; ++i)            
		if(huf_tree[i].parent == 0 && huf_tree[i].weight < min)
		{
			min = huf_tree[i].weight;
			*s2 = i;
		}
} 

//构建哈夫曼树
void crtHuffmanTree(huffmanTree ht , int n) 
{
	int i,s1,s2;
	int m = 2*n-1;
	
	// 从n+1个元素开始构造新结点 
	for(i=n;i<m;i++){
		//在前i项中选取双亲为0且权值最小的两个结点 
		select(ht,i,&s1,&s2);
		ht[s1].parent = i;
		ht[s2].parent = i;
		ht[i].lChild = s1;
		ht[i].rChild = s2;
		ht[i].weight = ht[s1].weight + ht[s2].weight;
		ht[i].parent = 0;
	}
}


////进行哈夫曼编码(从叶子结点到根节点，逆向求各叶子结点的哈夫曼编码) 
void crtHuffmanCode(huffmanTree ht , unsigned int n){
	char *cd;
	int i,start,c,p,l=0;
	cd = (char *)malloc(256 * sizeof(char));	//编码临时存储地 
	cd[256-1] = '\0';		//最后一位存储编码结束码
	
	
	for(i=0;i<n;i++){
//		printf("\n%d",l++);
		start = 256-1;
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
		ht[i].huffmanCode = (char *)malloc((256-start) * sizeof(char));
		strcpy(ht[i].huffmanCode , &cd[start]);		//复制编码 
	}
	free(cd); 			//释放编码临时空间 

} 



void compress(char *originFile , char *newFile){
	int i,j,k;
	unsigned int n = 0;					//字符种类 
	unsigned int m;						//树的所有结点数 
	unsigned char char_temp;	//字符临时存储
	char code_buf[256] = "\0";	//编码临时存储空间 (节点数最多256个，树的深度最多256，编码长度最大256) 
	FILE *origin,*news,*infile;	//压缩文件和解压文件 
	TmpNode t;
	huffmanTree ht;
	unsigned long file_length=0;			//文件的字符数量 
	
	TmpNode *tmp_nodes =(TmpNode *)malloc(256*sizeof(TmpNode));	
	
		// 初始化暂存结点
	for(i = 0; i < 256; ++i)
	{
		tmp_nodes[i].weight = 0;
		tmp_nodes[i].ch = (unsigned char)i;		// 数组的256个下标与256种字符对应
	}


//遍历文件，取得字符所对应的频度，即叶子结点的权值
	origin = fopen(originFile, "rb");	//采用二进制方式读取文件 
	// 判断输入文件是否存在
	if (origin == NULL)
		printf("读取文件操做失败");
	fread((char *)&char_temp, sizeof(unsigned char), 1, origin);		// 读入一个字符
	while(!feof(origin))
	{
		tmp_nodes[(unsigned int)char_temp].weight++;			//下标对应的字符的权重 
		file_length++;
		fread((char *)&char_temp, sizeof(unsigned char), 1, origin);		// 读入一个字符
	}
	fclose(origin);


	//对临时存储结点的缓存区进行筛选 ，并将频度进行排序，将频度不为0的编码排在前面 
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
	
	//统计频度不为0的结点(即叶子结点) 
	for(i=0;i<256;i++){
		if(tmp_nodes[i].weight != 0)
			n++;
	}
	//结点总数 
	m = 2*n-1;

	ht = (huffmanTree)malloc(m * sizeof(HTNode));	//动态建立哈夫曼树所需结点
	
	//初始化叶子结点
	for(i=0;i<n;i++){
		ht[i].ch = tmp_nodes[i].ch;
		ht[i].weight = tmp_nodes[i].weight;
		ht[i].parent = 0;
	} 

	//释放临时存储结点内存 
	free(tmp_nodes);	
	crtHuffmanTree(ht , n);

	crtHuffmanCode(ht , n);
	
	//写入字符和相应的频度(供解压文件时使用)
	if((news = fopen(newFile , "wb"))== NULL){
		printf("\n写文件操作失败"); 
	}
	else{
		fwrite((char *)&n , sizeof(unsigned int) , 1 , news);					//写入字符种类数 
		for(i=0;i<n;i++){
			fwrite((char *)&ht[i].ch , sizeof(unsigned char) , 1 , news);		//写入字符 
			fwrite((char *)&ht[i].weight , sizeof(unsigned long) , 1 , news);				//写入权重 
		}
		
		fwrite((char *)&file_length , sizeof(unsigned long) , 1 , news);		//写入文件内容的字符总数 



		origin = fopen(originFile , "rb");
		fread((char *)&char_temp , sizeof(unsigned char) , 1 ,origin);
		while(!feof(origin)){
		
			//将文件中的字符与编码表中的字符相匹配 
			for(i=0;i<n;i++){
				//若匹配，则即将编码表中字符对应的编码添加到临时编码空间种 
				if(char_temp == ht[i].ch)
					strcat(code_buf , ht[i].huffmanCode);
			}
			
			//一个字节(8位)为i一个处理单位 
			while(strlen(code_buf) >=8){
				
				char_temp = '\0';	//清空字符临时存储，改为字符编码临时存储空间 
				for(i=0;i<8;i++){
					char_temp <<= 1;	//将临时编码空间左移1位
					if(code_buf[i] == '1')
						char_temp |= 1;	//如果编码为'1'，通过或操作将其添加到字节的最低位 
				}
				fwrite((char *)&char_temp , sizeof(unsigned char) , 1 , news);
				strcpy(code_buf , code_buf+8);	//编码缓存跳过前8位 
			}
			fread((char *)&char_temp , sizeof(unsigned char) , 1 ,origin);
		}
		
		//文件内容最后不足8位编码
		if(strlen(code_buf) >0 && strlen(code_buf) <8){
			char_temp = '0';
			for(i=0;i<strlen(code_buf);i++){
				char_temp <<= 1;
				if(code_buf[i] == '1'){
					char_temp |= 1;
				}
			}
			char_temp <<= 8-strlen(code_buf);		//将编码字段从尾部移到字节的高位 
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
//	unsigned long file_length;			//文件长度 
//	unsigned long write_length = 0;		//写入长度
//	FILE *origin , *news;
//	huffmanTree ht;
//	unsigned char code_temp;		//编码临时存储
//	unsigned int root;				//数中根结点的下标 
//	
//	if((origin = fopen(originFile , "rb")) == NULL){
//		printf("\n文件读取失败");
//	} 
//	else{
//		//读取字符种类数 
//		fread((char *)&n , sizeof(unsigned int) , 1 , origin);
////		printf("%d",n);
//		m = 2*n-1;
//		ht = (HTNode *)malloc(m * sizeof(HTNode));
//		for(i=0;i<n;i++){
//			//读取相应的字符和权重 
//			fread((char *)&ht[i].ch , sizeof(unsigned char) , 1 , origin);
//			fread((char *)&ht[i].weight , sizeof(unsigned long) , 1 , origin);
//			ht[i].parent = 0; 
////			printf("%c",ht[i].ch);
//		} 
//
//		//重建哈夫曼树 
//		crtHuffmanTree(ht , n);
//		
//		
//		//读取文件长度(因为feof难以判断二进制文件的结束位置，所以通过文件长度来判断) 
//		fread((char *)&file_length , sizeof(unsigned long) , 1 , origin);
//	
//		if((news = fopen(newFile , "wb")) == NULL){
//			printf("\n写入文件操作失败");
//		} 
//		else{
//			//根节点 
//			root = m-1;
//			while(1){
//				//读取一个字节的编码 
//				fread((char *)&code_temp , sizeof(unsigned char) , 1 , origin);
//				
//				 for(i=0;i<8;i++){
//				 	//从根到叶子结点向下遍历 
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
//						root = m-1;			//回到根节点，再次进行遍历 
//					}
//					code_temp <<= 1; 		//编码左移一位， 
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




// 解压函数
void extract(char *originFile, char *newFile)
{
	unsigned int i;
	unsigned long file_len;
	unsigned long writen_len = 0;		// 控制文件写入长度
	FILE *origin, *news;
	unsigned int char_kinds;		// 存储字符种类
	unsigned int node_num;
	huffmanTree huf_tree;
	unsigned char code_temp;		// 暂存8bits编码
	unsigned int root;		// 保存根节点索引，供匹配编码使用

	origin = fopen(originFile, "rb");		// 以二进制方式打开压缩文件
	// 判断输入文件是否存在


	// 读取压缩文件前端的字符及对应编码，用于重建哈夫曼树
	fread((char *)&char_kinds, sizeof(unsigned int), 1, origin);     // 读取字符种类数
	

		node_num = 2 * char_kinds - 1;		// 根据字符种类数，计算建立哈夫曼树所需结点数 
		huf_tree = (huffmanTree)malloc(node_num*sizeof(HTNode));		// 动态分配哈夫曼树结点空间
		// 读取字符及对应权重，存入哈夫曼树节点
		for(i = 0; i < char_kinds; ++i)     
		{
			fread((char *)&huf_tree[i].ch, sizeof(unsigned char), 1, origin);		// 读入字符
			fread((char *)&huf_tree[i].weight, sizeof(unsigned long), 1, origin);	// 读入字符对应权重
			huf_tree[i].parent = 0;
		}
		// 初始化后node_num-char_kins个结点的parent
		for(; i < node_num; ++i) 
			huf_tree[i].parent = 0;

		crtHuffmanTree(huf_tree, char_kinds);		// 重建哈夫曼树（与压缩时的一致）

		// 读完字符和权重信息，紧接着读取文件长度和编码，进行解码
		fread((char *)&file_len, sizeof(unsigned long), 1, origin);	// 读入文件长度
		news = fopen(newFile, "wb");		// 打开压缩后将生成的文件
		root = node_num-1;
		while(1)
		{
			fread((char *)&code_temp, sizeof(unsigned char), 1, origin);		// 读取一个字符长度的编码

			// 处理读取的一个字符长度的编码（通常为8位）
			for(i = 0; i < 8; ++i)
			{
				// 由根向下直至叶节点正向匹配编码对应字符
				if(code_temp & 128)
					root = huf_tree[root].rChild;
				else
					root = huf_tree[root].lChild;

				if(root < char_kinds)
				{
					fwrite((char *)&huf_tree[root].ch, sizeof(unsigned char), 1, news);
					++writen_len;
					if (writen_len == file_len) break;		// 控制文件长度，跳出内层循环
					root = node_num-1;        // 复位为根索引，匹配下一个字符
				}
				code_temp <<= 1;		// 将编码缓存的下一位移到最高位，供匹配
			}
			if (writen_len == file_len) break;		// 控制文件长度，跳出外层循环
		}

		// 关闭文件
		fclose(origin);
		fclose(news);

		// 释放内存
		free(huf_tree);
}//extract()





void main(){
	char originFile[200],newFile[200];	//待压缩和压缩后的文件名
	int i,j=1;
	while(j){
		system("cls"); 
		printf("\n=========================================================================================================\n");
		printf("|					      光明牌压缩/解压器	 					|");
		printf("\n|													|");
		printf("\n|						1. 压缩文件						|");
		printf("\n|						2. 解压文件						|");
		printf("\n|						3.  退出						|");
		printf("\n|													|");
		printf("\n=========================================================================================================\n");
		printf("\n请输入所需操作:");
		scanf("%d",&i);
		switch(i){
			case 1:{
				system("cls");
				printf("请输入待压缩文件的地址:");
				scanf("%s",originFile); 
				printf("\n请输入压缩后文件的保存地址:");
				scanf("%s",newFile);
				printf("\n正在压缩，请稍后");
				for(i=0;i<3;i++){
					printf(".");
					Sleep(1000);
					printf(".");
				}
				
				compress(originFile , newFile);
				
				printf("\n\n压缩完毕");
				Sleep(1000);
				break;
			}
			case 2:{
				system("cls");
				printf("请输入待解压的文件地址:");
				scanf("%s",originFile);
				printf("\n请输入解压后文件保存的地址:");
				fflush(stdin);
				scanf("%s",newFile);
				
				printf("\n正在解压，请稍后");
				for(i=0;i<3;i++){
					printf(".");
					Sleep(1000);
					printf(".");
				}
				
				extract(originFile , newFile);
				printf("\n\n解压完毕");
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
