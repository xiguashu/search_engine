#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#define MAXTABLESIZE 100000
#define KEYLENGTH 15                   // 关键词字符串的最大长度 
#define TABLE_SIZE 5000


typedef char ElementType[KEYLENGTH + 1];  //关键词的类型为字符串
typedef unsigned long Index;                     //散列的地址类型为int

typedef struct LNode *PtrToLNode;
struct LNode {
	ElementType Data; //关键字
	int times;  //总共出现的次数
	unsigned long  *docID; //所在文档的ID		
	int *docTimes;
	PtrToLNode Next;
};
typedef PtrToLNode Position;
typedef PtrToLNode List;


typedef struct TblNode *HashTable;
struct TblNode {
	int TableSize; //表的最大长度
	List Heads;   //存放链表头指针的数组
};

int NextPrime(int N);
unsigned long Hash(char* key, int tablesize);
HashTable CreateTable(int TableSize);
Position Find(HashTable H, ElementType Key);
void Insert(HashTable H);
void DestroyTable(HashTable H);
void transfer(char string[], char result[]); //将输入的单词转化为字符串形式的数值
void FindMost(unsigned long a[], int number, unsigned long index[]);


int main()
{
	FILE *fp1;
	char query[160]; //查询语句
	ElementType words[10]; //拆分后的关键词
	Position p[10];
	int flag = 0;
	HashTable H;
	char valueOfquery[10][13] = { 0 };
	int i, j, k=0;
	int k1 = 0;
	int k2 = 0;
	int len;
	int start, finish;
	int times;
	int numOfDoc = 0;
	unsigned long *sortedID;
	unsigned long index[1000];
	unsigned long min;
	int minP;
	int indexOfid[10] = { 0 };
	start = clock();
	H = CreateTable(TABLE_SIZE);
	Insert(H);
	finish = clock();
	printf("构建哈希表耗时：%dms\n", (finish - start));
	printf("Input words to start your query\n");
	//把查询语句分解成一个个单词
	gets(query); //读入查询
	start = clock();

	len = strlen(query); //计算长度
	for (i = 0; i <= len; i++)
	{
		if (query[i] != ' ') //遇到空格则开始下一个单词
		{
			words[k1][k2] = query[i]; //将字符保存成单词
			k2++;
		}
		else {
			words[k1][k2] = '\0';//上个词结束
			transfer(words[k1], valueOfquery[k1]);//把刚结束的词转换成数值
			k2 = 0;   //下个词开始
			k1++;
		}

	}
	words[k1][k2] = '\0';//上个词结束
	transfer(words[k1], valueOfquery[k1]);//把最后一个词转换成数值

	int numOfwords = k1+1; //查询的总单词数
	for (i = 0; i <numOfwords; i++)
		p[i] = Find(H, valueOfquery[i]);  //在哈希表中查找每个关键词

	for (i = 0; i <numOfwords; i++)
	{
		if (p[i])
			numOfDoc = numOfDoc + p[i]->times;//计算总文档数
	}
	if (numOfDoc == 0) //没找到
	{
		printf("Please Inpur More Information!\n");//输出提示
		system("pause");
		return 0;
	}
	sortedID = (unsigned long*)malloc(sizeof(unsigned long)*numOfDoc);//申请内存

	//以下操作将每个单词的查询结果有序的保存在数组sortedID中
	for (j = 0; j < numOfDoc; j++) //一共是numOfDoc 个数
	{

		while(1) { //将min初始化
			if (p[k] && (indexOfid[k]<p[k]->times)) //如果p[k]查到的结果不为空,且此时p[k]->docID中还有元素
			{
				min = p[k]->docID[indexOfid[k]];//将min赋值为 p[k]->docID数组中当前的最小值，也就是下标为indexOfid[k]对应的值
				k = 0;//为下一次赋值做初始化
				break;
			}
			k++;//下一个查询结果
		}
		for (i = 0; i < numOfwords; i++)//查找当前p[0]到p[n]中的最小值
		{
			if (p[i]&&(indexOfid[i]<p[i]->times)&&(p[i]->docID[indexOfid[i]] <= min))//比min小
			{
				min = p[i]->docID[indexOfid[i]]; //更新min
				minP = i; //记录是第i个查询结果
			}
		}
		indexOfid[minP]++; //第i个查询结果的下标加一
		sortedID[j] = min; //将最小值插入数组
    }
	

	FindMost(sortedID, numOfDoc, index);//找到sortedID中出现次数最多的稳定id的下标
	i = 0;
	printf("Here are the results:\n");//输出结果
	while (index[i] != -1) //下标不等于-1
	{
		printf("Doc %lu  ", sortedID[index[i]]);//输出下标对应的值
		i++;
		if (!(i%5))
			printf("\n");//每五个结果换一次行
	}

	finish = clock();
	printf(" \n");

	printf("查询耗时：%dms", (finish - start) );

	system("pause");
}

int NextPrime(int N)
{ /* 返回大于N且不超过MAXTABLESIZE的最小素数 */
	int i, p = (N % 2) ? N + 2 : N + 1; /*从大于N的下一个奇数开始 */

	while (p <= MAXTABLESIZE) {
		for (i = (int)sqrt(p); i>2; i--)
			if (!(p%i)) break; /* p不是素数 */
		if (i == 2) break; /* for正常结束，说明p是素数 */
		else  p += 2; /* 否则试探下一个奇数 */
	}
	return p;
}

unsigned long Hash(char* key, int tablesize) //哈希函数
{
	unsigned long h = 0;
	while (*key != 0)  //位移映射
	{
		h = (h << 5) + *key++;
	}
	return h%tablesize;

}


HashTable CreateTable(int TableSize) //创建哈希表
{
	HashTable H;
	int i;

	H = (HashTable)malloc(sizeof(struct TblNode));//申请内存
	H->TableSize = NextPrime(TableSize);
	H->Heads = (List)malloc(H->TableSize*sizeof(struct LNode)); //为指针申请内存
	for (i = 0; i<H->TableSize; i++) { //初始化
		H->Heads[i].Data[0] = '\0';
		H->Heads[i].Next = NULL;
	}

	return H;
}

Position Find(HashTable H, ElementType Key) //查找关键字
{
	Position P; //记录位置
	Index Pos; //哈希值，即head数组的下标
	Pos = Hash(Key, H->TableSize);  //计算哈希值
	P = H->Heads[Pos].Next;
	while (P && strcmp(P->Data, Key)) //依次比较链表中每个关键词
		P = P->Next;

	return P; //返回查找结果，关键词相等或空指针
}

void Insert(HashTable H) //插入函数
{
	Position P, NewCell; //存放要插入的新关键词
	Index Pos; //下标
	FILE *fp1;   //文件指针
	ElementType Key; //关键词
	int i, j;  //循环变量

	 fopen_s(&fp1,"d:\\Index.txt", "r"); //只读形式打开文件

	while (1)
	{

		if (fscanf_s(fp1, "%s", Key,16) == EOF) //读取关键词
			break;
		if ((Key[0] == '#') && (Key[1] == '#')) //以标识符开头
		{
			P = Find(H, Key + 2);
			if (!P) //未找到关键词，可以插入
			{
				NewCell = (Position)malloc(sizeof(struct LNode)); //申请空间
				strcpy_s(NewCell->Data, Key + 2); //把关键词储存

				if (fscanf_s(fp1, "%d", &(NewCell->times)) == EOF) //读取出现的频率
					break;
				NewCell->docID = (unsigned long*)malloc(sizeof(unsigned long)*NewCell->times);
				NewCell->docTimes = (int*)malloc(sizeof(int)*NewCell->times);

				for (i = 0; i < NewCell->times; i++) //读取所有文件id
				{
					if (fscanf_s(fp1, "%lu", &(NewCell->docID[i])) == EOF)
						break;
					if (fscanf_s(fp1, "%d", &(NewCell->docTimes)) == EOF) //读取在某个文件中出现的次数
						break;
				}
				Pos = Hash(Key + 2, H->TableSize); /* 初始散列位置 */
												   /* 将NewCell插入为H->Heads[Pos]链表的第1个结点 */
				NewCell->Next = H->Heads[Pos].Next;
				H->Heads[Pos].Next = NewCell;
			}
		}

	}

	fclose(fp1);

}

void DestroyTable(HashTable H)
{
	int i;
	Position P, Tmp;

	/* 释放每个链表的结点 */
	for (i = 0; i<H->TableSize; i++) {
		P = H->Heads[i].Next;
		while (P) {
			Tmp = P->Next;
			free(P);
			P = Tmp;
		}
	}
	free(H->Heads); /* 释放头结点数组 */
	free(H);        /* 释放散列表结点 */
}
void transfer(char string[],char result[])//将字符串转换成哈希值
{
	unsigned long value; //哈希值
	int i = 0;
	int lenth = 0; //哈希值的长度
	unsigned long value0; 
	value = 0; //初始化
	while (string[i]) //没到末尾
	{
		if (string[i] >= 'A'&&string[i] <= 'Z') //若是大写字母，把它变成小写
			string[i] = string[i] - 'A' + 'a';
		if (string[i]>='a'&&string[i]<='z') {
			
			value = (value << 5) + string[i] - 'a'+1;//移位
		}
		i++;
	}
	value0 = value; //复制
	do
	{
		lenth++;
		value0 = value0 / 10;//计算他的长度

	} while (value0);
	result[lenth] = '\0'; //字符串末尾
	for (i = lenth - 1; i >= 0; i--)//数字转换成字符串
	{
		result[i] = value % 10+'0';
		value = value / 10;
	}
}

void FindMost(unsigned long sortedID[], int number, unsigned long index[])//找到顺序数组sortedID中所有重复次数最多的元素的下标index
{
	int i,j;//循环变量
	int count=1;//计数
	index[0] = 0;
	int max = 1; //初始化重复最多次数
	j = 0; //index的下标
	for (i = 1; i < number; i++) //对于每一个元素
	{
		if (sortedID[i] == sortedID[i - 1])//如果它等于上一个元素
		{
			count++; //计数器加一
			if (count == max) //如果该元素重复的次数等于最大重复次数
			{
				index[++j] = i; //记录其下标
				index[j + 1] = -1; //并作出结尾标记
			}
			if (count > max)//如果该元素重复次数大于最大重复次数
			{
				j = 0; //重置j
				index[j] = i; //此时index中只有该元素对应下标
				index[j + 1] = -1;//结尾标记
				max = count; //更新max
			}
		}
		else { //若与上一元素不一样
			count = 1;//重新技术
			if (count == max) //同上
			{
				index[++j] = i;
				index[j + 1] = -1;
			}
		}
	}


}
