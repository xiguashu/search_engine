/* 提取词干 前面部分是porter词干提取算法的C语言开源代码*/

#include <iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<string>
#include<io.h>
#include <fstream>
#include<fstream>
#include<string>
#include<iomanip>
#include<time.h>
#include<math.h>
#include<windows.h>
#define MAXTABLESIZE 100000
#define KEYLENGTH 20                  // 关键词字符串的最大长度 
#define TABLE_SIZE 5000

using namespace std;
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
void print(int a);
int stem(char * p, int i, int j);
int NextPrime(int N);
unsigned long Hash(char* key, int tablesize);
HashTable CreateTable(int TableSize);
Position Find(HashTable H, ElementType Key);
void Insert(HashTable H);
void DestroyTable(HashTable H);
void transfer(char string[], char result[]); //将输入的单词转化为字符串形式的数值
void FindMost(unsigned long a[], int number, unsigned long index[]);
void Query(HashTable H, char query[]);
int stop(char* word);
#include <string.h>  /* for memmove */
#include <stdio.h>
#include <stdlib.h>      /* for malloc, free */
#include <ctype.h>       /* for isupper, islower, tolower */
#define TRUE 1
#define FALSE 0

/* The main part of the stemming algorithm starts here. b is a buffer
holding a word to be stemmed. The letters are in b[k0], b[k0+1] ...
ending at b[k]. In fact k0 = 0 in this demo program. k is readjusted
downwards as the stemming progresses. Zero termination is not in fact
used in the algorithm.

Note that only lower case sequences are stemmed. Forcing to lower case
should be done before stem(...) is called.
*/

static char * b;       /* buffer for word to be stemmed */
static int k, k0, j;     /* j is a general offset into the string */
						 /* cons(i) is TRUE <=> b[i] is a consonant. */

static int cons(int i)
{
	switch (b[i])
	{
	case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;
	case 'y': return (i == k0) ? TRUE : !cons(i - 1);
	default: return TRUE;
	}
}

/* m() measures the number of consonant sequences between k0 and j. if c is
a consonant sequence and v a vowel sequence, and <..> indicates arbitrary
presence,

<c><v>       gives 0
<c>vc<v>     gives 1
<c>vcvc<v>   gives 2
<c>vcvcvc<v> gives 3
....
*/

static int m()
{
	int n = 0;
	int i = k0;
	while (TRUE)
	{
		if (i > j) return n;
		if (!cons(i)) break; i++;
	}
	i++;
	while (TRUE)
	{
		while (TRUE)
		{
			if (i > j) return n;
			if (cons(i)) break;
			i++;
		}
		i++;
		n++;
		while (TRUE)
		{
			if (i > j) return n;
			if (!cons(i)) break;
			i++;
		}
		i++;
	}
}

/* vowelinstem() is TRUE <=> k0,...j contains a vowel */

static int vowelinstem()
{
	int i; for (i = k0; i <= j; i++) if (!cons(i)) return TRUE;
	return FALSE;
}

/* doublec(j) is TRUE <=> j,(j-1) contain a double consonant. */

static int doublec(int j)
{
	if (j < k0 + 1) return FALSE;
	if (b[j] != b[j - 1]) return FALSE;
	return cons(j);
}

/* cvc(i) is TRUE <=> i-2,i-1,i has the form consonant - vowel - consonant
and also if the second c is not w,x or y. this is used when trying to
restore an e at the end of a short word. e.g.

cav(e), lov(e), hop(e), crim(e), but
snow, box, tray.

*/

static int cvc(int i)
{
	if (i < k0 + 2 || !cons(i) || cons(i - 1) || !cons(i - 2)) return FALSE;
	{  int ch = b[i];
	if (ch == 'w' || ch == 'x' || ch == 'y') return FALSE;
	}
	return TRUE;
}

/* ends(s) is TRUE <=> k0,...k ends with the string s. */

static int ends(char * s)
{
	int length = s[0];
	if (s[length] != b[k]) return FALSE; /* tiny speed-up */
	if (length > k - k0 + 1) return FALSE;
	if (memcmp(b + k - length + 1, s + 1, length) != 0) return FALSE;
	j = k - length;
	return TRUE;
}

/* setto(s) sets (j+1),...k to the characters in the string s, readjusting
k. */

static void setto(char * s)
{
	int length = s[0];
	memmove(b + j + 1, s + 1, length);
	k = j + length;
}

/* r(s) is used further down. */

static void r(char * s) { if (m() > 0) setto(s); }

/* step1ab() gets rid of plurals and -ed or -ing. e.g.

caresses  ->  caress
ponies    ->  poni
ties      ->  ti
caress    ->  caress
cats      ->  cat

feed      ->  feed
agreed    ->  agree
disabled  ->  disable

matting   ->  mat
mating    ->  mate
meeting   ->  meet
milling   ->  mill
messing   ->  mess

meetings  ->  meet

*/

static void step1ab()
{
	if (b[k] == 's')
	{
		if (ends("\04" "sses")) k -= 2; else
			if (ends("\03" "ies")) setto("\01" "i"); else
				if (b[k - 1] != 's') k--;
	}
	if (ends("\03" "eed")) { if (m() > 0) k--; }
	else
		if ((ends("\02" "ed") || ends("\03" "ing")) && vowelinstem())
		{
			k = j;
			if (ends("\02" "at")) setto("\03" "ate"); else
				if (ends("\02" "bl")) setto("\03" "ble"); else
					if (ends("\02" "iz")) setto("\03" "ize"); else
						if (doublec(k))
						{
							k--;
							{  int ch = b[k];
							if (ch == 'l' || ch == 's' || ch == 'z') k++;
							}
						}
						else if (m() == 1 && cvc(k)) setto("\01" "e");
		}
}

static void step1c() { if (ends("\01" "y") && vowelinstem()) b[k] = 'i'; }


static void step2() {
	switch (b[k - 1])
	{
	case 'a': if (ends("\07" "ational")) { r("\03" "ate"); break; }
			  if (ends("\06" "tional")) { r("\04" "tion"); break; }
			  break;
	case 'c': if (ends("\04" "enci")) { r("\04" "ence"); break; }
			  if (ends("\04" "anci")) { r("\04" "ance"); break; }
			  break;
	case 'e': if (ends("\04" "izer")) { r("\03" "ize"); break; }
			  break;
	case 'l': if (ends("\03" "bli")) { r("\03" "ble"); break; } /*-DEPARTURE-*/

																/* To match the published algorithm, replace this line with
																case 'l': if (ends("\04" "abli")) { r("\04" "able"); break; } */

			  if (ends("\04" "alli")) { r("\02" "al"); break; }
			  if (ends("\05" "entli")) { r("\03" "ent"); break; }
			  if (ends("\03" "eli")) { r("\01" "e"); break; }
			  if (ends("\05" "ousli")) { r("\03" "ous"); break; }
			  break;
	case 'o': if (ends("\07" "ization")) { r("\03" "ize"); break; }
			  if (ends("\05" "ation")) { r("\03" "ate"); break; }
			  if (ends("\04" "ator")) { r("\03" "ate"); break; }
			  break;
	case 's': if (ends("\05" "alism")) { r("\02" "al"); break; }
			  if (ends("\07" "iveness")) { r("\03" "ive"); break; }
			  if (ends("\07" "fulness")) { r("\03" "ful"); break; }
			  if (ends("\07" "ousness")) { r("\03" "ous"); break; }
			  break;
	case 't': if (ends("\05" "aliti")) { r("\02" "al"); break; }
			  if (ends("\05" "iviti")) { r("\03" "ive"); break; }
			  if (ends("\06" "biliti")) { r("\03" "ble"); break; }
			  break;
	case 'g': if (ends("\04" "logi")) { r("\03" "log"); break; } /*-DEPARTURE-*/

																 /* To match the published algorithm, delete this line */

	}
}

/* step3() deals with -ic-, -full, -ness etc. similar strategy to step2. */

static void step3() {
	switch (b[k])
	{
	case 'e': if (ends("\05" "icate")) { r("\02" "ic"); break; }
			  if (ends("\05" "ative")) { r("\00" ""); break; }
			  if (ends("\05" "alize")) { r("\02" "al"); break; }
			  break;
	case 'i': if (ends("\05" "iciti")) { r("\02" "ic"); break; }
			  break;
	case 'l': if (ends("\04" "ical")) { r("\02" "ic"); break; }
			  if (ends("\03" "ful")) { r("\00" ""); break; }
			  break;
	case 's': if (ends("\04" "ness")) { r("\00" ""); break; }
			  break;
	}
}

/* step4() takes off -ant, -ence etc., in context <c>vcvc<v>. */

static void step4()
{
	switch (b[k - 1])
	{
	case 'a': if (ends("\02" "al")) break; return;
	case 'c': if (ends("\04" "ance")) break;
		if (ends("\04" "ence")) break; return;
	case 'e': if (ends("\02" "er")) break; return;
	case 'i': if (ends("\02" "ic")) break; return;
	case 'l': if (ends("\04" "able")) break;
		if (ends("\04" "ible")) break; return;
	case 'n': if (ends("\03" "ant")) break;
		if (ends("\05" "ement")) break;
		if (ends("\04" "ment")) break;
		if (ends("\03" "ent")) break; return;
	case 'o': if (ends("\03" "ion") && j >= k0 && (b[j] == 's' || b[j] == 't')) break;
		if (ends("\02" "ou")) break; return;
		/* takes care of -ous */
	case 's': if (ends("\03" "ism")) break; return;
	case 't': if (ends("\03" "ate")) break;
		if (ends("\03" "iti")) break; return;
	case 'u': if (ends("\03" "ous")) break; return;
	case 'v': if (ends("\03" "ive")) break; return;
	case 'z': if (ends("\03" "ize")) break; return;
	default: return;
	}
	if (m() > 1) k = j;
}

/* step5() removes a final -e if m() > 1, and changes -ll to -l if
m() > 1. */

static void step5()
{
	j = k;
	if (b[k] == 'e')
	{
		int a = m();
		if (a > 1 || a == 1 && !cvc(k - 1)) k--;
	}
	if (b[k] == 'l' && doublec(k) && m() > 1) k--;
}


int stem(char * p, int i, int j)
{
	b = p; k = j; k0 = i; /* copy the parameters into statics */
	if (k <= k0 + 1) return k; /*-DEPARTURE-*/

							   /* With this line, strings of length 1 or 2 don't go through the
							   stemming process, although no mention is made of this in the
							   published algorithm. Remove the line to match the published
							   algorithm. */

	step1ab();
	if (k > k0) {
		step1c(); step2(); step3(); step4(); step5();
	}
	return k;
}

/*--------------------stemmer definition ends here------------------------*/



static char * s;         /* a char * (=string) pointer; passed into b above */
FILE* out; FILE *f;/*用于输出的文件指针 */
#define INC 50           /* size units in which s is increased */
static int i_max = INC;  /* maximum offset in s */

void increase_s()
{
	i_max += INC;
	{  char * new_s = (char *)malloc(i_max + 1);
	{ int i; for (i = 0; i < i_max; i++) new_s[i] = s[i]; } /* copy across */
	free(s); s = new_s;
	}
}

#define LETTER(ch) (isupper(ch) || islower(ch))
static void stemword(char* word)
{
	int k = 0;
	int ch = word[k];
	k++;
	if (ch == 0) return;
	if (LETTER(ch))
	{
		int i = 0;
		while (TRUE)
		{
			if (i == i_max) increase_s();

			ch = tolower(ch); /* forces lower case */
			s[i] = ch; i++;
			ch = word[k];
			k++;
			if (ch == 0)
				break;
			//if (!LETTER(ch)) { ungetc(ch, stdin); break; }
		}
		s[stem(s, 0, i - 1) + 1] = 0;
		/* the previous line calls the stemmer and uses its result to
		zero-terminate the string in s */
		//strcpy_s(word, s);
	}
}

int main()
{


	ifstream test_in("D:\\test.txt");
	LARGE_INTEGER start;
	LARGE_INTEGER end;
	LARGE_INTEGER frequency;
	HashTable H;
	char query[420]; //查询语句
	H = CreateTable(TABLE_SIZE);
	Insert(H);
	printf("Input words to start your query(less than 20 words):\n");
	while (1)
	{
		 if(test_in.fail())
			break;
		test_in.getline(query, sizeof(query));
	//gets_s(query);
		Query(H, query);
	
	}
	system("pause");
}

void Query(HashTable H, char query[])
{
	ofstream test_out("D:\\result.txt", ios::app);
	ElementType words[20]; //拆分后的关键词
	Position p[20];
	int flag = 0;
	char valueOfquery[20][13] = { 0 };
	int i, j, k = 0;
	int k1 = 0;
	int k2 = 0;
	int len;
	int times;
	int numOfDoc = 0;
	unsigned long *sortedID;
	unsigned long index[1000] = { 0 };
	unsigned long min;
	int minP;
	int indexOfid[20] = { 0 };
	//把查询语句分解成一个个单词
	len = strlen(query); //计算长度
	for (i = 0; i <= len; i++)
	{
		if (query[i] >= 'a'&&query[i] <= 'z' || query[i] >= 'A'&&query[i] <= 'Z') //遇到空格则开始下一个单词
		{
			words[k1][k2] = query[i]; //将字符保存成单词
			k2++;
		}
		else {
			words[k1][k2] = '\0';//上个词结束
			for (j = 0; j < k1; j++) //判断是否重复出现
				if (!strcmp(words[j], words[k1]))
				{
					strcpy_s(words[k1], "NOSUCHWORD");
					break;
				}
			flag = stop(words[k1]);
			if (flag)
			{
				s = (char *)malloc(i_max + 1);
				stemword(words[k1]);
				transfer(s, valueOfquery[k1]);
				k2 = 0;   //下个词开始
				k1++;
			}
			else
			{
				k2 = 0;
			}
		}

	}
	if (k1 == 0 && k2 == 0)
	{
		printf("Please Input More Information!\n");//输出提示
		system("pause");
		return;
	}
	words[k1][k2] = '\0';//上个词结束
	stemword(words[k1]);
	transfer(s, valueOfquery[k1]);

	int numOfwords = k1 + 1; //查询的总单词数
	for (i = 0; i < numOfwords; i++) {
		p[i] = Find(H, valueOfquery[i]);  //在哈希表中查找每个关键词

	}
	for (i = 0; i < numOfwords; i++)
	{
		if (p[i])
			numOfDoc = numOfDoc + p[i]->times;//计算总文档数
	}
	if (numOfDoc == 0) //没找到
	{
		printf("Please Input More Information!\n");//输出提示
		system("pause");
		return;
	}
	sortedID = (unsigned long*)malloc(sizeof(unsigned long)*numOfDoc);//申请内存

																	  //以下操作将每个单词的查询结果有序的保存在数组sortedID中
	for (j = 0; j < numOfDoc; j++) //一共是numOfDoc 个数
	{

		while (1) { //将min初始化
			if (p[k] && (indexOfid[k] < p[k]->times)) //如果p[k]查到的结果不为空,且此时p[k]->docID中还有元素
			{
				min = p[k]->docID[indexOfid[k]];//将min赋值为 p[k]->docID数组中当前的最小值，也就是下标为indexOfid[k]对应的值
				k = 0;//为下一次赋值做初始化
				break;
			}
			k++;//下一个查询结果
		}
		for (i = 0; i < numOfwords; i++)//查找当前p[0]到p[n]中的最小值
		{
			if (p[i] && (indexOfid[i] < p[i]->times) && (p[i]->docID[indexOfid[i]] <= min))//比min小
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
	//printf("Here are the results:\n");//输出结果
	test_out << "###########################the result of " << query <<"#############################"<<endl;
	while (index[i] != -1) //下标不等于-1
	{		
		print(sortedID[index[i]]);
		//printf("Doc %lu  ", sortedID[index[i]]);//输出下标对应的值
		i++;

	}

	printf(" \n");


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

	fopen_s(&fp1, "d:\\Index.txt", "r"); //只读形式打开文件

	while (1)
	{

		if (fscanf_s(fp1, "%s", Key, 16) == EOF) //读取关键词
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
void transfer(char string[], char result[])//将字符串转换成哈希值
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
		if (string[i] >= 'a'&&string[i] <= 'z') {

			value = (value << 5) + string[i] - 'a' + 1;//移位
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
		result[i] = value % 10 + '0';
		value = value / 10;
	}
	/**	printf("value=");
	puts(result);
	printf("\n");*/

}

void FindMost(unsigned long sortedID[], int number, unsigned long index[])//找到顺序数组sortedID中所有重复次数最多的元素的下标index
{
	int i, j;//循环变量
	int count = 1;//计数
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
	//	printf("max=%d\n", max);
}


int stop(char* word)
{
	FILE *stopword;   //文件指针
	if (fopen_s(&stopword, "d:\\stopwords.txt", "r") == EOF)
		cout << "打开失败" << endl;
	char stword[20];
	while (fscanf_s(stopword, "%s", stword, 20) != EOF)
	{
		if (!strcmp(word, stword))
			return 0;
	}
	return 1;
}

void print(int a)
{
	ofstream test_out("D:\\result.txt", ios::app);
	switch (a) {
	case 0: test_out << "1henryiv.1.1.txt" << endl; break;
	case 1: test_out << "1henryiv.1.2.txt" << endl; break;
	case 2: test_out << "1henryiv.1.3.txt" << endl; break;
	case 3: test_out << "1henryiv.2.1.txt" << endl; break;
	case 4: test_out << "1henryiv.2.2.txt" << endl; break;
	case 5: test_out << "1henryiv.2.3.txt" << endl; break;
	case 6: test_out << "1henryiv.2.4.txt" << endl; break;
	case 7: test_out << "1henryiv.3.1.txt" << endl; break;
	case 8: test_out << "1henryiv.3.2.txt" << endl; break;
	case 9: test_out << "1henryiv.4.1.txt" << endl; break;
	case 10: test_out << "1henryiv.4.2.txt" << endl; break;
	case 11: test_out << "1henryiv.4.3.txt" << endl; break;
	case 12: test_out << "1henryiv.4.4.txt" << endl; break;
	case 13: test_out << "1henryiv.5.1.txt" << endl; break;
	case 14: test_out << "1henryiv.5.2.txt" << endl; break;
	case 15: test_out << "1henryiv.5.3.txt" << endl; break;
	case 16: test_out << "1henryiv.5.4.txt" << endl; break;
	case 17: test_out << "1henryiv.5.5.txt" << endl; break;
	case 18: test_out << "1henryvi.1.1.txt" << endl; break;
	case 19: test_out << "1henryvi.1.2.txt" << endl; break;
	case 20: test_out << "1henryvi.1.3.txt" << endl; break;
	case 21: test_out << "1henryvi.1.4.txt" << endl; break;
	case 22: test_out << "1henryvi.1.5.txt" << endl; break;
	case 23: test_out << "1henryvi.1.6.txt" << endl; break;
	case 24: test_out << "1henryvi.2.1.txt" << endl; break;
	case 25: test_out << "1henryvi.2.2.txt" << endl; break;
	case 26: test_out << "1henryvi.2.3.txt" << endl; break;
	case 27: test_out << "1henryvi.2.4.txt" << endl; break;
	case 28: test_out << "1henryvi.2.5.txt" << endl; break;
	case 29: test_out << "1henryvi.3.1.txt" << endl; break;
	case 30: test_out << "1henryvi.3.2.txt" << endl; break;
	case 31: test_out << "1henryvi.3.3.txt" << endl; break;
	case 32: test_out << "1henryvi.3.4.txt" << endl; break;
	case 33: test_out << "1henryvi.4.1.txt" << endl; break;
	case 34: test_out << "1henryvi.4.2.txt" << endl; break;
	case 35: test_out << "1henryvi.4.3.txt" << endl; break;
	case 36: test_out << "1henryvi.4.4.txt" << endl; break;
	case 37: test_out << "1henryvi.4.5.txt" << endl; break;
	case 38: test_out << "1henryvi.4.6.txt" << endl; break;
	case 39: test_out << "1henryvi.4.7.txt" << endl; break;
	case 40: test_out << "1henryvi.5.1.txt" << endl; break;
	case 41: test_out << "1henryvi.5.2.txt" << endl; break;
	case 42: test_out << "1henryvi.5.3.txt" << endl; break;
	case 43: test_out << "1henryvi.5.4.txt" << endl; break;
	case 44: test_out << "1henryvi.5.5.txt" << endl; break;
	case 45: test_out << "2henryiv.0.0.txt" << endl; break;
	case 46: test_out << "2henryiv.1.1.txt" << endl; break;
	case 47: test_out << "2henryiv.1.2.txt" << endl; break;
	case 48: test_out << "2henryiv.1.3.txt" << endl; break;
	case 49: test_out << "2henryiv.2.1.txt" << endl; break;
	case 50: test_out << "2henryiv.2.2.txt" << endl; break;
	case 51: test_out << "2henryiv.2.3.txt" << endl; break;
	case 52: test_out << "2henryiv.2.4.txt" << endl; break;
	case 53: test_out << "2henryiv.3.1.txt" << endl; break;
	case 54: test_out << "2henryiv.3.2.txt" << endl; break;
	case 55: test_out << "2henryiv.4.1.txt" << endl; break;
	case 56: test_out << "2henryiv.4.2.txt" << endl; break;
	case 57: test_out << "2henryiv.4.3.txt" << endl; break;
	case 58: test_out << "2henryiv.4.4.txt" << endl; break;
	case 59: test_out << "2henryiv.4.5.txt" << endl; break;
	case 60: test_out << "2henryiv.5.1.txt" << endl; break;
	case 61: test_out << "2henryiv.5.2.txt" << endl; break;
	case 62: test_out << "2henryiv.5.3.txt" << endl; break;
	case 63: test_out << "2henryiv.5.4.txt" << endl; break;
	case 64: test_out << "2henryiv.5.5.txt" << endl; break;
	case 65: test_out << "2henryvi.1.1.txt" << endl; break;
	case 66: test_out << "2henryvi.1.2.txt" << endl; break;
	case 67: test_out << "2henryvi.1.3.txt" << endl; break;
	case 68: test_out << "2henryvi.1.4.txt" << endl; break;
	case 69: test_out << "2henryvi.2.1.txt" << endl; break;
	case 70: test_out << "2henryvi.2.2.txt" << endl; break;
	case 71: test_out << "2henryvi.2.3.txt" << endl; break;
	case 72: test_out << "2henryvi.2.4.txt" << endl; break;
	case 73: test_out << "2henryvi.3.1.txt" << endl; break;
	case 74: test_out << "2henryvi.3.2.txt" << endl; break;
	case 75: test_out << "2henryvi.3.3.txt" << endl; break;
	case 76: test_out << "2henryvi.4.1.txt" << endl; break;
	case 77: test_out << "2henryvi.4.10.txt" << endl; break;
	case 78: test_out << "2henryvi.4.2.txt" << endl; break;
	case 79: test_out << "2henryvi.4.3.txt" << endl; break;
	case 80: test_out << "2henryvi.4.4.txt" << endl; break;
	case 81: test_out << "2henryvi.4.5.txt" << endl; break;
	case 82: test_out << "2henryvi.4.6.txt" << endl; break;
	case 83: test_out << "2henryvi.4.7.txt" << endl; break;
	case 84: test_out << "2henryvi.4.8.txt" << endl; break;
	case 85: test_out << "2henryvi.4.9.txt" << endl; break;
	case 86: test_out << "2henryvi.5.1.txt" << endl; break;
	case 87: test_out << "2henryvi.5.2.txt" << endl; break;
	case 88: test_out << "2henryvi.5.3.txt" << endl; break;
	case 89: test_out << "3henryvi.1.1.txt" << endl; break;
	case 90: test_out << "3henryvi.1.2.txt" << endl; break;
	case 91: test_out << "3henryvi.1.3.txt" << endl; break;
	case 92: test_out << "3henryvi.1.4.txt" << endl; break;
	case 93: test_out << "3henryvi.2.1.txt" << endl; break;
	case 94: test_out << "3henryvi.2.2.txt" << endl; break;
	case 95: test_out << "3henryvi.2.3.txt" << endl; break;
	case 96: test_out << "3henryvi.2.4.txt" << endl; break;
	case 97: test_out << "3henryvi.2.5.txt" << endl; break;
	case 98: test_out << "3henryvi.2.6.txt" << endl; break;
	case 99: test_out << "3henryvi.3.1.txt" << endl; break;
	case 100: test_out << "3henryvi.3.2.txt" << endl; break;
	case 101: test_out << "3henryvi.3.3.txt" << endl; break;
	case 102: test_out << "3henryvi.4.1.txt" << endl; break;
	case 103: test_out << "3henryvi.4.2.txt" << endl; break;
	case 104: test_out << "3henryvi.4.3.txt" << endl; break;
	case 105: test_out << "3henryvi.4.4.txt" << endl; break;
	case 106: test_out << "3henryvi.4.5.txt" << endl; break;
	case 107: test_out << "3henryvi.4.6.txt" << endl; break;
	case 108: test_out << "3henryvi.4.7.txt" << endl; break;
	case 109: test_out << "3henryvi.4.8.txt" << endl; break;
	case 110: test_out << "3henryvi.5.1.txt" << endl; break;
	case 111: test_out << "3henryvi.5.2.txt" << endl; break;
	case 112: test_out << "3henryvi.5.3.txt" << endl; break;
	case 113: test_out << "3henryvi.5.4.txt" << endl; break;
	case 114: test_out << "3henryvi.5.5.txt" << endl; break;
	case 115: test_out << "3henryvi.5.6.txt" << endl; break;
	case 116: test_out << "3henryvi.5.7.txt" << endl; break;
	case 117: test_out << "allswell.1.1.txt" << endl; break;
	case 118: test_out << "allswell.1.2.txt" << endl; break;
	case 119: test_out << "allswell.1.3.txt" << endl; break;
	case 120: test_out << "allswell.2.1.txt" << endl; break;
	case 121: test_out << "allswell.2.2.txt" << endl; break;
	case 122: test_out << "allswell.2.3.txt" << endl; break;
	case 123: test_out << "allswell.2.4.txt" << endl; break;
	case 124: test_out << "allswell.2.5.txt" << endl; break;
	case 125: test_out << "allswell.3.1.txt" << endl; break;
	case 126: test_out << "allswell.3.2.txt" << endl; break;
	case 127: test_out << "allswell.3.3.txt" << endl; break;
	case 128: test_out << "allswell.3.4.txt" << endl; break;
	case 129: test_out << "allswell.3.5.txt" << endl; break;
	case 130: test_out << "allswell.3.6.txt" << endl; break;
	case 131: test_out << "allswell.3.7.txt" << endl; break;
	case 132: test_out << "allswell.4.1.txt" << endl; break;
	case 133: test_out << "allswell.4.2.txt" << endl; break;
	case 134: test_out << "allswell.4.3.txt" << endl; break;
	case 135: test_out << "allswell.4.4.txt" << endl; break;
	case 136: test_out << "allswell.4.5.txt" << endl; break;
	case 137: test_out << "allswell.5.1.txt" << endl; break;
	case 138: test_out << "allswell.5.2.txt" << endl; break;
	case 139: test_out << "allswell.5.3.txt" << endl; break;
	case 140: test_out << "asyoulikeit.1.1.txt" << endl; break;
	case 141: test_out << "asyoulikeit.1.2.txt" << endl; break;
	case 142: test_out << "asyoulikeit.1.3.txt" << endl; break;
	case 143: test_out << "asyoulikeit.2.1.txt" << endl; break;
	case 144: test_out << "asyoulikeit.2.2.txt" << endl; break;
	case 145: test_out << "asyoulikeit.2.3.txt" << endl; break;
	case 146: test_out << "asyoulikeit.2.4.txt" << endl; break;
	case 147: test_out << "asyoulikeit.2.5.txt" << endl; break;
	case 148: test_out << "asyoulikeit.2.6.txt" << endl; break;
	case 149: test_out << "asyoulikeit.2.7.txt" << endl; break;
	case 150: test_out << "asyoulikeit.3.1.txt" << endl; break;
	case 151: test_out << "asyoulikeit.3.2.txt" << endl; break;
	case 152: test_out << "asyoulikeit.3.3.txt" << endl; break;
	case 153: test_out << "asyoulikeit.3.4.txt" << endl; break;
	case 154: test_out << "asyoulikeit.3.5.txt" << endl; break;
	case 155: test_out << "asyoulikeit.4.1.txt" << endl; break;
	case 156: test_out << "asyoulikeit.4.2.txt" << endl; break;
	case 157: test_out << "asyoulikeit.4.3.txt" << endl; break;
	case 158: test_out << "asyoulikeit.5.1.txt" << endl; break;
	case 159: test_out << "asyoulikeit.5.2.txt" << endl; break;
	case 160: test_out << "asyoulikeit.5.3.txt" << endl; break;
	case 161: test_out << "asyoulikeit.5.4.txt" << endl; break;
	case 162: test_out << "cleopatra.1.1.txt" << endl; break;
	case 163: test_out << "cleopatra.1.2.txt" << endl; break;
	case 164: test_out << "cleopatra.1.3.txt" << endl; break;
	case 165: test_out << "cleopatra.1.4.txt" << endl; break;
	case 166: test_out << "cleopatra.1.5.txt" << endl; break;
	case 167: test_out << "cleopatra.2.1.txt" << endl; break;
	case 168: test_out << "cleopatra.2.2.txt" << endl; break;
	case 169: test_out << "cleopatra.2.3.txt" << endl; break;
	case 170: test_out << "cleopatra.2.4.txt" << endl; break;
	case 171: test_out << "cleopatra.2.5.txt" << endl; break;
	case 172: test_out << "cleopatra.2.6.txt" << endl; break;
	case 173: test_out << "cleopatra.2.7.txt" << endl; break;
	case 174: test_out << "cleopatra.3.1.txt" << endl; break;
	case 175: test_out << "cleopatra.3.10.txt" << endl; break;
	case 176: test_out << "cleopatra.3.11.txt" << endl; break;
	case 177: test_out << "cleopatra.3.12.txt" << endl; break;
	case 178: test_out << "cleopatra.3.13.txt" << endl; break;
	case 179: test_out << "cleopatra.3.2.txt" << endl; break;
	case 180: test_out << "cleopatra.3.3.txt" << endl; break;
	case 181: test_out << "cleopatra.3.4.txt" << endl; break;
	case 182: test_out << "cleopatra.3.5.txt" << endl; break;
	case 183: test_out << "cleopatra.3.6.txt" << endl; break;
	case 184: test_out << "cleopatra.3.7.txt" << endl; break;
	case 185: test_out << "cleopatra.3.8.txt" << endl; break;
	case 186: test_out << "cleopatra.3.9.txt" << endl; break;
	case 187: test_out << "cleopatra.4.1.txt" << endl; break;
	case 188: test_out << "cleopatra.4.10.txt" << endl; break;
	case 189: test_out << "cleopatra.4.11.txt" << endl; break;
	case 190: test_out << "cleopatra.4.12.txt" << endl; break;
	case 191: test_out << "cleopatra.4.13.txt" << endl; break;
	case 192: test_out << "cleopatra.4.14.txt" << endl; break;
	case 193: test_out << "cleopatra.4.15.txt" << endl; break;
	case 194: test_out << "cleopatra.4.2.txt" << endl; break;
	case 195: test_out << "cleopatra.4.3.txt" << endl; break;
	case 196: test_out << "cleopatra.4.4.txt" << endl; break;
	case 197: test_out << "cleopatra.4.5.txt" << endl; break;
	case 198: test_out << "cleopatra.4.6.txt" << endl; break;
	case 199: test_out << "cleopatra.4.7.txt" << endl; break;
	case 200: test_out << "cleopatra.4.8.txt" << endl; break;
	case 201: test_out << "cleopatra.4.9.txt" << endl; break;
	case 202: test_out << "cleopatra.5.1.txt" << endl; break;
	case 203: test_out << "cleopatra.5.2.txt" << endl; break;
	case 204: test_out << "comedy_errors.1.1.txt" << endl; break;
	case 205: test_out << "comedy_errors.1.2.txt" << endl; break;
	case 206: test_out << "comedy_errors.2.1.txt" << endl; break;
	case 207: test_out << "comedy_errors.2.2.txt" << endl; break;
	case 208: test_out << "comedy_errors.3.1.txt" << endl; break;
	case 209: test_out << "comedy_errors.3.2.txt" << endl; break;
	case 210: test_out << "comedy_errors.4.1.txt" << endl; break;
	case 211: test_out << "comedy_errors.4.2.txt" << endl; break;
	case 212: test_out << "comedy_errors.4.3.txt" << endl; break;
	case 213: test_out << "comedy_errors.4.4.txt" << endl; break;
	case 214: test_out << "comedy_errors.5.1.txt" << endl; break;
	case 215: test_out << "coriolanus.1.1.txt" << endl; break;
	case 216: test_out << "coriolanus.1.10.txt" << endl; break;
	case 217: test_out << "coriolanus.1.2.txt" << endl; break;
	case 218: test_out << "coriolanus.1.3.txt" << endl; break;
	case 219: test_out << "coriolanus.1.4.txt" << endl; break;
	case 220: test_out << "coriolanus.1.5.txt" << endl; break;
	case 221: test_out << "coriolanus.1.6.txt" << endl; break;
	case 222: test_out << "coriolanus.1.7.txt" << endl; break;
	case 223: test_out << "coriolanus.1.8.txt" << endl; break;
	case 224: test_out << "coriolanus.1.9.txt" << endl; break;
	case 225: test_out << "coriolanus.2.1.txt" << endl; break;
	case 226: test_out << "coriolanus.2.2.txt" << endl; break;
	case 227: test_out << "coriolanus.2.3.txt" << endl; break;
	case 228: test_out << "coriolanus.3.1.txt" << endl; break;
	case 229: test_out << "coriolanus.3.2.txt" << endl; break;
	case 230: test_out << "coriolanus.3.3.txt" << endl; break;
	case 231: test_out << "coriolanus.4.1.txt" << endl; break;
	case 232: test_out << "coriolanus.4.2.txt" << endl; break;
	case 233: test_out << "coriolanus.4.3.txt" << endl; break;
	case 234: test_out << "coriolanus.4.4.txt" << endl; break;
	case 235: test_out << "coriolanus.4.5.txt" << endl; break;
	case 236: test_out << "coriolanus.4.6.txt" << endl; break;
	case 237: test_out << "coriolanus.4.7.txt" << endl; break;
	case 238: test_out << "coriolanus.5.1.txt" << endl; break;
	case 239: test_out << "coriolanus.5.2.txt" << endl; break;
	case 240: test_out << "coriolanus.5.3.txt" << endl; break;
	case 241: test_out << "coriolanus.5.4.txt" << endl; break;
	case 242: test_out << "coriolanus.5.5.txt" << endl; break;
	case 243: test_out << "coriolanus.5.6.txt" << endl; break;
	case 244: test_out << "cymbeline.1.1.txt" << endl; break;
	case 245: test_out << "cymbeline.1.2.txt" << endl; break;
	case 246: test_out << "cymbeline.1.3.txt" << endl; break;
	case 247: test_out << "cymbeline.1.4.txt" << endl; break;
	case 248: test_out << "cymbeline.1.5.txt" << endl; break;
	case 249: test_out << "cymbeline.1.6.txt" << endl; break;
	case 250: test_out << "cymbeline.2.1.txt" << endl; break;
	case 251: test_out << "cymbeline.2.2.txt" << endl; break;
	case 252: test_out << "cymbeline.2.4.txt" << endl; break;
	case 253: test_out << "cymbeline.2.5.txt" << endl; break;
	case 254: test_out << "cymbeline.3.1.txt" << endl; break;
	case 255: test_out << "cymbeline.3.2.txt" << endl; break;
	case 256: test_out << "cymbeline.3.3.txt" << endl; break;
	case 257: test_out << "cymbeline.3.4.txt" << endl; break;
	case 258: test_out << "cymbeline.3.5.txt" << endl; break;
	case 259: test_out << "cymbeline.3.6.txt" << endl; break;
	case 260: test_out << "cymbeline.3.7.txt" << endl; break;
	case 261: test_out << "cymbeline.4.1.txt" << endl; break;
	case 262: test_out << "cymbeline.4.2.txt" << endl; break;
	case 263: test_out << "cymbeline.4.3.txt" << endl; break;
	case 264: test_out << "cymbeline.4.4.txt" << endl; break;
	case 265: test_out << "cymbeline.5.1.txt" << endl; break;
	case 266: test_out << "cymbeline.5.2.txt" << endl; break;
	case 267: test_out << "cymbeline.5.3.txt" << endl; break;
	case 268: test_out << "cymbeline.5.4.txt" << endl; break;
	case 269: test_out << "cymbeline.5.5.txt" << endl; break;
	case 270: test_out << "elegy.txt" << endl; break;
	case 271: test_out << "hamlet.1.1.txt" << endl; break;
	case 272: test_out << "hamlet.1.2.txt" << endl; break;
	case 273: test_out << "hamlet.1.3.txt" << endl; break;
	case 274: test_out << "hamlet.1.4.txt" << endl; break;
	case 275: test_out << "hamlet.1.5.txt" << endl; break;
	case 276: test_out << "hamlet.2.1.txt" << endl; break;
	case 277: test_out << "hamlet.2.2.txt" << endl; break;
	case 278: test_out << "hamlet.3.1.txt" << endl; break;
	case 279: test_out << "hamlet.3.2.txt" << endl; break;
	case 280: test_out << "hamlet.3.3.txt" << endl; break;
	case 281: test_out << "hamlet.3.4.txt" << endl; break;
	case 282: test_out << "hamlet.4.1.txt" << endl; break;
	case 283: test_out << "hamlet.4.2.txt" << endl; break;
	case 284: test_out << "hamlet.4.3.txt" << endl; break;
	case 285: test_out << "hamlet.4.4.txt" << endl; break;
	case 286: test_out << "hamlet.4.5.txt" << endl; break;
	case 287: test_out << "hamlet.4.6.txt" << endl; break;
	case 288: test_out << "hamlet.4.7.txt" << endl; break;
	case 289: test_out << "hamlet.5.1.txt" << endl; break;
	case 290: test_out << "hamlet.5.2.txt" << endl; break;
	case 291: test_out << "henryv.1.0.txt" << endl; break;
	case 292: test_out << "henryv.1.1.txt" << endl; break;
	case 293: test_out << "henryv.1.2.txt" << endl; break;
	case 294: test_out << "henryv.2.0.txt" << endl; break;
	case 295: test_out << "henryv.2.1.txt" << endl; break;
	case 296: test_out << "henryv.2.2.txt" << endl; break;
	case 297: test_out << "henryv.2.3.txt" << endl; break;
	case 298: test_out << "henryv.2.4.txt" << endl; break;
	case 299: test_out << "henryv.3.0.txt" << endl; break;
	case 300: test_out << "henryv.3.1.txt" << endl; break;
	case 301: test_out << "henryv.3.2.txt" << endl; break;
	case 302: test_out << "henryv.3.3.txt" << endl; break;
	case 303: test_out << "henryv.3.4.txt" << endl; break;
	case 304: test_out << "henryv.3.5.txt" << endl; break;
	case 305: test_out << "henryv.3.6.txt" << endl; break;
	case 306: test_out << "henryv.3.7.txt" << endl; break;
	case 307: test_out << "henryv.4.0.txt" << endl; break;
	case 308: test_out << "henryv.4.1.txt" << endl; break;
	case 309: test_out << "henryv.4.2.txt" << endl; break;
	case 310: test_out << "henryv.4.3.txt" << endl; break;
	case 311: test_out << "henryv.4.4.txt" << endl; break;
	case 312: test_out << "henryv.4.5.txt" << endl; break;
	case 313: test_out << "henryv.4.6.txt" << endl; break;
	case 314: test_out << "henryv.4.7.txt" << endl; break;
	case 315: test_out << "henryv.4.8.txt" << endl; break;
	case 316: test_out << "henryv.5.0.txt" << endl; break;
	case 317: test_out << "henryv.5.1.txt" << endl; break;
	case 318: test_out << "henryv.5.2.txt" << endl; break;
	case 319: test_out << "henryviii.1.0.txt" << endl; break;
	case 320: test_out << "henryviii.1.1.txt" << endl; break;
	case 321: test_out << "henryviii.1.2.txt" << endl; break;
	case 322: test_out << "henryviii.1.3.txt" << endl; break;
	case 323: test_out << "henryviii.1.4.txt" << endl; break;
	case 324: test_out << "henryviii.2.1.txt" << endl; break;
	case 325: test_out << "henryviii.2.2.txt" << endl; break;
	case 326: test_out << "henryviii.2.3.txt" << endl; break;
	case 327: test_out << "henryviii.2.4.txt" << endl; break;
	case 328: test_out << "henryviii.3.1.txt" << endl; break;
	case 329: test_out << "henryviii.3.2.txt" << endl; break;
	case 330: test_out << "henryviii.4.1.txt" << endl; break;
	case 331: test_out << "henryviii.4.2.txt" << endl; break;
	case 332: test_out << "henryviii.5.1.txt" << endl; break;
	case 333: test_out << "henryviii.5.2.txt" << endl; break;
	case 334: test_out << "henryviii.5.3.txt" << endl; break;
	case 335: test_out << "henryviii.5.4.txt" << endl; break;
	case 336: test_out << "henryviii.5.5.txt" << endl; break;
	case 337: test_out << "john.1.1.txt" << endl; break;
	case 338: test_out << "john.2.1.txt" << endl; break;
	case 339: test_out << "john.3.1.txt" << endl; break;
	case 340: test_out << "john.3.2.txt" << endl; break;
	case 341: test_out << "john.3.3.txt" << endl; break;
	case 342: test_out << "john.3.4.txt" << endl; break;
	case 343: test_out << "john.4.1.txt" << endl; break;
	case 344: test_out << "john.4.2.txt" << endl; break;
	case 345: test_out << "john.4.3.txt" << endl; break;
	case 346: test_out << "john.5.1.txt" << endl; break;
	case 347: test_out << "john.5.2.txt" << endl; break;
	case 348: test_out << "john.5.3.txt" << endl; break;
	case 349: test_out << "john.5.4.txt" << endl; break;
	case 350: test_out << "john.5.5.txt" << endl; break;
	case 351: test_out << "john.5.6.txt" << endl; break;
	case 352: test_out << "john.5.7.txt" << endl; break;
	case 353: test_out << "julius_caesar.1.1.txt" << endl; break;
	case 354: test_out << "julius_caesar.1.2.txt" << endl; break;
	case 355: test_out << "julius_caesar.1.3.txt" << endl; break;
	case 356: test_out << "julius_caesar.2.1.txt" << endl; break;
	case 357: test_out << "julius_caesar.2.2.txt" << endl; break;
	case 358: test_out << "julius_caesar.2.3.txt" << endl; break;
	case 359: test_out << "julius_caesar.2.4.txt" << endl; break;
	case 360: test_out << "julius_caesar.3.1.txt" << endl; break;
	case 361: test_out << "julius_caesar.3.2.txt" << endl; break;
	case 362: test_out << "julius_caesar.3.3.txt" << endl; break;
	case 363: test_out << "julius_caesar.4.1.txt" << endl; break;
	case 364: test_out << "julius_caesar.4.2.txt" << endl; break;
	case 365: test_out << "julius_caesar.4.3.txt" << endl; break;
	case 366: test_out << "julius_caesar.5.1.txt" << endl; break;
	case 367: test_out << "julius_caesar.5.2.txt" << endl; break;
	case 368: test_out << "julius_caesar.5.3.txt" << endl; break;
	case 369: test_out << "julius_caesar.5.4.txt" << endl; break;
	case 370: test_out << "julius_caesar.5.5.txt" << endl; break;
	case 371: test_out << "lear.1.1.txt" << endl; break;
	case 372: test_out << "lear.1.2.txt" << endl; break;
	case 373: test_out << "lear.1.3.txt" << endl; break;
	case 374: test_out << "lear.1.4.txt" << endl; break;
	case 375: test_out << "lear.1.5.txt" << endl; break;
	case 376: test_out << "lear.2.1.txt" << endl; break;
	case 377: test_out << "lear.2.2.txt" << endl; break;
	case 378: test_out << "lear.2.3.txt" << endl; break;
	case 379: test_out << "lear.2.4.txt" << endl; break;
	case 380: test_out << "lear.3.1.txt" << endl; break;
	case 381: test_out << "lear.3.2.txt" << endl; break;
	case 382: test_out << "lear.3.3.txt" << endl; break;
	case 383: test_out << "lear.3.4.txt" << endl; break;
	case 384: test_out << "lear.3.5.txt" << endl; break;
	case 385: test_out << "lear.3.6.txt" << endl; break;
	case 386: test_out << "lear.3.7.txt" << endl; break;
	case 387: test_out << "lear.4.1.txt" << endl; break;
	case 388: test_out << "lear.4.2.txt" << endl; break;
	case 389: test_out << "lear.4.3.txt" << endl; break;
	case 390: test_out << "lear.4.4.txt" << endl; break;
	case 391: test_out << "lear.4.5.txt" << endl; break;
	case 392: test_out << "lear.4.6.txt" << endl; break;
	case 393: test_out << "lear.4.7.txt" << endl; break;
	case 394: test_out << "lear.5.1.txt" << endl; break;
	case 395: test_out << "lear.5.2.txt" << endl; break;
	case 396: test_out << "lear.5.3.txt" << endl; break;
	case 397: test_out << "lll.1.1.txt" << endl; break;
	case 398: test_out << "lll.1.2.txt" << endl; break;
	case 399: test_out << "lll.2.1.txt" << endl; break;
	case 400: test_out << "lll.3.1.txt" << endl; break;
	case 401: test_out << "lll.4.1.txt" << endl; break;
	case 402: test_out << "lll.4.2.txt" << endl; break;
	case 403: test_out << "lll.4.3.txt" << endl; break;
	case 404: test_out << "lll.5.1.txt" << endl; break;
	case 405: test_out << "lll.5.2.txt" << endl; break;
	case 406: test_out << "LoversComplaint.txt" << endl; break;
	case 407: test_out << "macbeth.1.1.txt" << endl; break;
	case 408: test_out << "macbeth.1.2.txt" << endl; break;
	case 409: test_out << "macbeth.1.3.txt" << endl; break;
	case 410: test_out << "macbeth.1.4.txt" << endl; break;
	case 411: test_out << "macbeth.1.5.txt" << endl; break;
	case 412: test_out << "macbeth.1.6.txt" << endl; break;
	case 413: test_out << "macbeth.1.7.txt" << endl; break;
	case 414: test_out << "macbeth.2.1.txt" << endl; break;
	case 415: test_out << "macbeth.2.2.txt" << endl; break;
	case 416: test_out << "macbeth.2.3.txt" << endl; break;
	case 417: test_out << "macbeth.2.4.txt" << endl; break;
	case 418: test_out << "macbeth.3.1.txt" << endl; break;
	case 419: test_out << "macbeth.3.2.txt" << endl; break;
	case 420: test_out << "macbeth.3.3.txt" << endl; break;
	case 421: test_out << "macbeth.3.4.txt" << endl; break;
	case 422: test_out << "macbeth.3.5.txt" << endl; break;
	case 423: test_out << "macbeth.3.6.txt" << endl; break;
	case 424: test_out << "macbeth.4.1.txt" << endl; break;
	case 425: test_out << "macbeth.4.2.txt" << endl; break;
	case 426: test_out << "macbeth.4.3.txt" << endl; break;
	case 427: test_out << "macbeth.5.1.txt" << endl; break;
	case 428: test_out << "macbeth.5.2.txt" << endl; break;
	case 429: test_out << "macbeth.5.3.txt" << endl; break;
	case 430: test_out << "macbeth.5.4.txt" << endl; break;
	case 431: test_out << "macbeth.5.5.txt" << endl; break;
	case 432: test_out << "macbeth.5.6.txt" << endl; break;
	case 433: test_out << "macbeth.5.7.txt" << endl; break;
	case 434: test_out << "macbeth.5.8.txt" << endl; break;
	case 435: test_out << "measure.1.1.txt" << endl; break;
	case 436: test_out << "measure.1.2.txt" << endl; break;
	case 437: test_out << "measure.1.3.txt" << endl; break;
	case 438: test_out << "measure.1.4.txt" << endl; break;
	case 439: test_out << "measure.2.1.txt" << endl; break;
	case 440: test_out << "measure.2.2.txt" << endl; break;
	case 441: test_out << "measure.2.3.txt" << endl; break;
	case 442: test_out << "measure.2.4.txt" << endl; break;
	case 443: test_out << "measure.3.1.txt" << endl; break;
	case 444: test_out << "measure.3.2.txt" << endl; break;
	case 445: test_out << "measure.4.1.txt" << endl; break;
	case 446: test_out << "measure.4.2.txt" << endl; break;
	case 447: test_out << "measure.4.3.txt" << endl; break;
	case 448: test_out << "measure.4.4.txt" << endl; break;
	case 449: test_out << "measure.4.5.txt" << endl; break;
	case 450: test_out << "measure.4.6.txt" << endl; break;
	case 451: test_out << "measure.5.1.txt" << endl; break;
	case 452: test_out << "merchant.1.1.txt" << endl; break;
	case 453: test_out << "merchant.1.2.txt" << endl; break;
	case 454: test_out << "merchant.1.3.txt" << endl; break;
	case 455: test_out << "merchant.2.1.txt" << endl; break;
	case 456: test_out << "merchant.2.2.txt" << endl; break;
	case 457: test_out << "merchant.2.3.txt" << endl; break;
	case 458: test_out << "merchant.2.4.txt" << endl; break;
	case 459: test_out << "merchant.2.5.txt" << endl; break;
	case 460: test_out << "merchant.2.6.txt" << endl; break;
	case 461: test_out << "merchant.2.7.txt" << endl; break;
	case 462: test_out << "merchant.2.8.txt" << endl; break;
	case 463: test_out << "merchant.2.9.txt" << endl; break;
	case 464: test_out << "merchant.3.1.txt" << endl; break;
	case 465: test_out << "merchant.3.2.txt" << endl; break;
	case 466: test_out << "merchant.3.3.txt" << endl; break;
	case 467: test_out << "merchant.3.4.txt" << endl; break;
	case 468: test_out << "merchant.3.5.txt" << endl; break;
	case 469: test_out << "merchant.4.1.txt" << endl; break;
	case 470: test_out << "merchant.4.2.txt" << endl; break;
	case 471: test_out << "merchant.5.1.txt" << endl; break;
	case 472: test_out << "merry_wives.1.1.txt" << endl; break;
	case 473: test_out << "merry_wives.1.2.txt" << endl; break;
	case 474: test_out << "merry_wives.1.3.txt" << endl; break;
	case 475: test_out << "merry_wives.1.4.txt" << endl; break;
	case 476: test_out << "merry_wives.2.1.txt" << endl; break;
	case 477: test_out << "merry_wives.2.2.txt" << endl; break;
	case 478: test_out << "merry_wives.2.3.txt" << endl; break;
	case 479: test_out << "merry_wives.3.1.txt" << endl; break;
	case 480: test_out << "merry_wives.3.2.txt" << endl; break;
	case 481: test_out << "merry_wives.3.3.txt" << endl; break;
	case 482: test_out << "merry_wives.3.4.txt" << endl; break;
	case 483: test_out << "merry_wives.3.5.txt" << endl; break;
	case 484: test_out << "merry_wives.4.1.txt" << endl; break;
	case 485: test_out << "merry_wives.4.2.txt" << endl; break;
	case 486: test_out << "merry_wives.4.3.txt" << endl; break;
	case 487: test_out << "merry_wives.4.4.txt" << endl; break;
	case 488: test_out << "merry_wives.4.5.txt" << endl; break;
	case 489: test_out << "merry_wives.4.6.txt" << endl; break;
	case 490: test_out << "merry_wives.5.1.txt" << endl; break;
	case 491: test_out << "merry_wives.5.2.txt" << endl; break;
	case 492: test_out << "merry_wives.5.3.txt" << endl; break;
	case 493: test_out << "merry_wives.5.4.txt" << endl; break;
	case 494: test_out << "merry_wives.5.5.txt" << endl; break;
	case 495: test_out << "midsummer.1.1.txt" << endl; break;
	case 496: test_out << "midsummer.1.2.txt" << endl; break;
	case 497: test_out << "midsummer.2.1.txt" << endl; break;
	case 498: test_out << "midsummer.2.2.txt" << endl; break;
	case 499: test_out << "midsummer.3.1.txt" << endl; break;
	case 500: test_out << "midsummer.3.2.txt" << endl; break;
	case 501: test_out << "midsummer.4.1.txt" << endl; break;
	case 502: test_out << "midsummer.4.2.txt" << endl; break;
	case 503: test_out << "midsummer.5.1.txt" << endl; break;
	case 504: test_out << "much_ado.1.1.txt" << endl; break;
	case 505: test_out << "much_ado.1.2.txt" << endl; break;
	case 506: test_out << "much_ado.1.3.txt" << endl; break;
	case 507: test_out << "much_ado.2.1.txt" << endl; break;
	case 508: test_out << "much_ado.2.2.txt" << endl; break;
	case 509: test_out << "much_ado.2.3.txt" << endl; break;
	case 510: test_out << "much_ado.3.1.txt" << endl; break;
	case 511: test_out << "much_ado.3.2.txt" << endl; break;
	case 512: test_out << "much_ado.3.3.txt" << endl; break;
	case 513: test_out << "much_ado.3.4.txt" << endl; break;
	case 514: test_out << "much_ado.3.5.txt" << endl; break;
	case 515: test_out << "much_ado.4.1.txt" << endl; break;
	case 516: test_out << "much_ado.4.2.txt" << endl; break;
	case 517: test_out << "much_ado.5.1.txt" << endl; break;
	case 518: test_out << "much_ado.5.2.txt" << endl; break;
	case 519: test_out << "much_ado.5.3.txt" << endl; break;
	case 520: test_out << "much_ado.5.4.txt" << endl; break;
	case 521: test_out << "othello.1.1.txt" << endl; break;
	case 522: test_out << "othello.1.2.txt" << endl; break;
	case 523: test_out << "othello.1.3.txt" << endl; break;
	case 524: test_out << "othello.2.1.txt" << endl; break;
	case 525: test_out << "othello.2.2.txt" << endl; break;
	case 526: test_out << "othello.2.3.txt" << endl; break;
	case 527: test_out << "othello.3.1.txt" << endl; break;
	case 528: test_out << "othello.3.2.txt" << endl; break;
	case 529: test_out << "othello.3.3.txt" << endl; break;
	case 530: test_out << "othello.3.4.txt" << endl; break;
	case 531: test_out << "othello.4.1.txt" << endl; break;
	case 532: test_out << "othello.4.2.txt" << endl; break;
	case 533: test_out << "othello.4.3.txt" << endl; break;
	case 534: test_out << "othello.5.1.txt" << endl; break;
	case 535: test_out << "othello.5.2.txt" << endl; break;
	case 536: test_out << "pericles.1.0.txt" << endl; break;
	case 537: test_out << "pericles.1.1.txt" << endl; break;
	case 538: test_out << "pericles.1.2.txt" << endl; break;
	case 539: test_out << "pericles.1.3.txt" << endl; break;
	case 540: test_out << "pericles.1.4.txt" << endl; break;
	case 541: test_out << "pericles.2.1.txt" << endl; break;
	case 542: test_out << "pericles.2.2.txt" << endl; break;
	case 543: test_out << "pericles.2.3.txt" << endl; break;
	case 544: test_out << "pericles.2.4.txt" << endl; break;
	case 545: test_out << "pericles.2.5.txt" << endl; break;
	case 546: test_out << "pericles.3.2.txt" << endl; break;
	case 547: test_out << "pericles.3.3.txt" << endl; break;
	case 548: test_out << "pericles.3.4.txt" << endl; break;
	case 549: test_out << "pericles.4.1.txt" << endl; break;
	case 550: test_out << "pericles.4.2.txt" << endl; break;
	case 551: test_out << "pericles.4.3.txt" << endl; break;
	case 552: test_out << "pericles.4.5.txt" << endl; break;
	case 553: test_out << "pericles.4.6.txt" << endl; break;
	case 554: test_out << "pericles.5.1.txt" << endl; break;
	case 555: test_out << "pericles.5.3.txt" << endl; break;
	case 556: test_out << "RapeOfLucrece.txt" << endl; break;
	case 557: test_out << "richardii.1.1.txt" << endl; break;
	case 558: test_out << "richardii.1.2.txt" << endl; break;
	case 559: test_out << "richardii.1.3.txt" << endl; break;
	case 560: test_out << "richardii.1.4.txt" << endl; break;
	case 561: test_out << "richardii.2.1.txt" << endl; break;
	case 562: test_out << "richardii.2.2.txt" << endl; break;
	case 563: test_out << "richardii.2.3.txt" << endl; break;
	case 564: test_out << "richardii.2.4.txt" << endl; break;
	case 565: test_out << "richardii.3.1.txt" << endl; break;
	case 566: test_out << "richardii.3.2.txt" << endl; break;
	case 567: test_out << "richardii.3.3.txt" << endl; break;
	case 568: test_out << "richardii.3.4.txt" << endl; break;
	case 569: test_out << "richardii.4.1.txt" << endl; break;
	case 570: test_out << "richardii.5.1.txt" << endl; break;
	case 571: test_out << "richardii.5.2.txt" << endl; break;
	case 572: test_out << "richardii.5.3.txt" << endl; break;
	case 573: test_out << "richardii.5.4.txt" << endl; break;
	case 574: test_out << "richardii.5.5.txt" << endl; break;
	case 575: test_out << "richardii.5.6.txt" << endl; break;
	case 576: test_out << "richardiii.1.1.txt" << endl; break;
	case 577: test_out << "richardiii.1.2.txt" << endl; break;
	case 578: test_out << "richardiii.1.3.txt" << endl; break;
	case 579: test_out << "richardiii.1.4.txt" << endl; break;
	case 580: test_out << "richardiii.2.1.txt" << endl; break;
	case 581: test_out << "richardiii.2.2.txt" << endl; break;
	case 582: test_out << "richardiii.2.3.txt" << endl; break;
	case 583: test_out << "richardiii.2.4.txt" << endl; break;
	case 584: test_out << "richardiii.3.1.txt" << endl; break;
	case 585: test_out << "richardiii.3.2.txt" << endl; break;
	case 586: test_out << "richardiii.3.3.txt" << endl; break;
	case 587: test_out << "richardiii.3.4.txt" << endl; break;
	case 588: test_out << "richardiii.3.5.txt" << endl; break;
	case 589: test_out << "richardiii.3.6.txt" << endl; break;
	case 590: test_out << "richardiii.3.7.txt" << endl; break;
	case 591: test_out << "richardiii.4.1.txt" << endl; break;
	case 592: test_out << "richardiii.4.2.txt" << endl; break;
	case 593: test_out << "richardiii.4.3.txt" << endl; break;
	case 594: test_out << "richardiii.4.4.txt" << endl; break;
	case 595: test_out << "richardiii.4.5.txt" << endl; break;
	case 596: test_out << "richardiii.5.1.txt" << endl; break;
	case 597: test_out << "richardiii.5.2.txt" << endl; break;
	case 598: test_out << "richardiii.5.3.txt" << endl; break;
	case 599: test_out << "richardiii.5.4.txt" << endl; break;
	case 600: test_out << "richardiii.5.5.txt" << endl; break;
	case 601: test_out << "romeo_juliet.1.0.txt" << endl; break;
	case 602: test_out << "romeo_juliet.1.1.txt" << endl; break;
	case 603: test_out << "romeo_juliet.1.2.txt" << endl; break;
	case 604: test_out << "romeo_juliet.1.3.txt" << endl; break;
	case 605: test_out << "romeo_juliet.1.4.txt" << endl; break;
	case 606: test_out << "romeo_juliet.1.5.txt" << endl; break;
	case 607: test_out << "romeo_juliet.2.0.txt" << endl; break;
	case 608: test_out << "romeo_juliet.2.1.txt" << endl; break;
	case 609: test_out << "romeo_juliet.2.2.txt" << endl; break;
	case 610: test_out << "romeo_juliet.2.3.txt" << endl; break;
	case 611: test_out << "romeo_juliet.2.4.txt" << endl; break;
	case 612: test_out << "romeo_juliet.2.5.txt" << endl; break;
	case 613: test_out << "romeo_juliet.2.6.txt" << endl; break;
	case 614: test_out << "romeo_juliet.3.1.txt" << endl; break;
	case 615: test_out << "romeo_juliet.3.2.txt" << endl; break;
	case 616: test_out << "romeo_juliet.3.3.txt" << endl; break;
	case 617: test_out << "romeo_juliet.3.4.txt" << endl; break;
	case 618: test_out << "romeo_juliet.3.5.txt" << endl; break;
	case 619: test_out << "romeo_juliet.4.1.txt" << endl; break;
	case 620: test_out << "romeo_juliet.4.2.txt" << endl; break;
	case 621: test_out << "romeo_juliet.4.3.txt" << endl; break;
	case 622: test_out << "romeo_juliet.4.4.txt" << endl; break;
	case 623: test_out << "romeo_juliet.4.5.txt" << endl; break;
	case 624: test_out << "romeo_juliet.5.1.txt" << endl; break;
	case 625: test_out << "romeo_juliet.5.2.txt" << endl; break;
	case 626: test_out << "romeo_juliet.5.3.txt" << endl; break;
	case 627: test_out << "sonnet.C.txt" << endl; break;
	case 628: test_out << "sonnet.CI.txt" << endl; break;
	case 629: test_out << "sonnet.CII.txt" << endl; break;
	case 630: test_out << "sonnet.CIII.txt" << endl; break;
	case 631: test_out << "sonnet.CIV.txt" << endl; break;
	case 632: test_out << "sonnet.CIX.txt" << endl; break;
	case 633: test_out << "sonnet.CL.txt" << endl; break;
	case 634: test_out << "sonnet.CLI.txt" << endl; break;
	case 635: test_out << "sonnet.CLII.txt" << endl; break;
	case 636: test_out << "sonnet.CLIII.txt" << endl; break;
	case 637: test_out << "sonnet.CLIV.txt" << endl; break;
	case 638: test_out << "sonnet.CV.txt" << endl; break;
	case 639: test_out << "sonnet.CVI.txt" << endl; break;
	case 640: test_out << "sonnet.CVII.txt" << endl; break;
	case 641: test_out << "sonnet.CVIII.txt" << endl; break;
	case 642: test_out << "sonnet.CX.txt" << endl; break;
	case 643: test_out << "sonnet.CXI.txt" << endl; break;
	case 644: test_out << "sonnet.CXII.txt" << endl; break;
	case 645: test_out << "sonnet.CXIII.txt" << endl; break;
	case 646: test_out << "sonnet.CXIV.txt" << endl; break;
	case 647: test_out << "sonnet.CXIX.txt" << endl; break;
	case 648: test_out << "sonnet.CXL.txt" << endl; break;
	case 649: test_out << "sonnet.CXLI.txt" << endl; break;
	case 650: test_out << "sonnet.CXLII.txt" << endl; break;
	case 651: test_out << "sonnet.CXLIII.txt" << endl; break;
	case 652: test_out << "sonnet.CXLIV.txt" << endl; break;
	case 653: test_out << "sonnet.CXLIX.txt" << endl; break;
	case 654: test_out << "sonnet.CXLV.txt" << endl; break;
	case 655: test_out << "sonnet.CXLVI.txt" << endl; break;
	case 656: test_out << "sonnet.CXLVII.txt" << endl; break;
	case 657: test_out << "sonnet.CXLVIII.txt" << endl; break;
	case 658: test_out << "sonnet.CXV.txt" << endl; break;
	case 659: test_out << "sonnet.CXVI.txt" << endl; break;
	case 660: test_out << "sonnet.CXVII.txt" << endl; break;
	case 661: test_out << "sonnet.CXVIII.txt" << endl; break;
	case 662: test_out << "sonnet.CXX.txt" << endl; break;
	case 663: test_out << "sonnet.CXXI.txt" << endl; break;
	case 664: test_out << "sonnet.CXXII.txt" << endl; break;
	case 665: test_out << "sonnet.CXXIII.txt" << endl; break;
	case 666: test_out << "sonnet.CXXIV.txt" << endl; break;
	case 667: test_out << "sonnet.CXXIX.txt" << endl; break;
	case 668: test_out << "sonnet.CXXV.txt" << endl; break;
	case 669: test_out << "sonnet.CXXVI.txt" << endl; break;
	case 670: test_out << "sonnet.CXXVII.txt" << endl; break;
	case 671: test_out << "sonnet.CXXVIII.txt" << endl; break;
	case 672: test_out << "sonnet.CXXX.txt" << endl; break;
	case 673: test_out << "sonnet.CXXXI.txt" << endl; break;
	case 674: test_out << "sonnet.CXXXII.txt" << endl; break;
	case 675: test_out << "sonnet.CXXXIII.txt" << endl; break;
	case 676: test_out << "sonnet.CXXXIV.txt" << endl; break;
	case 677: test_out << "sonnet.CXXXIX.txt" << endl; break;
	case 678: test_out << "sonnet.CXXXV.txt" << endl; break;
	case 679: test_out << "sonnet.CXXXVI.txt" << endl; break;
	case 680: test_out << "sonnet.CXXXVII.txt" << endl; break;
	case 681: test_out << "sonnet.CXXXVIII.txt" << endl; break;
	case 682: test_out << "sonnet.I.txt" << endl; break;
	case 683: test_out << "sonnet.II.txt" << endl; break;
	case 684: test_out << "sonnet.III.txt" << endl; break;
	case 685: test_out << "sonnet.IV.txt" << endl; break;
	case 686: test_out << "sonnet.IX.txt" << endl; break;
	case 687: test_out << "sonnet.L.txt" << endl; break;
	case 688: test_out << "sonnet.LI.txt" << endl; break;
	case 689: test_out << "sonnet.LII.txt" << endl; break;
	case 690: test_out << "sonnet.LIII.txt" << endl; break;
	case 691: test_out << "sonnet.LIV.txt" << endl; break;
	case 692: test_out << "sonnet.LIX.txt" << endl; break;
	case 693: test_out << "sonnet.LV.txt" << endl; break;
	case 694: test_out << "sonnet.LVI.txt" << endl; break;
	case 695: test_out << "sonnet.LVII.txt" << endl; break;
	case 696: test_out << "sonnet.LVIII.txt" << endl; break;
	case 697: test_out << "sonnet.LX.txt" << endl; break;
	case 698: test_out << "sonnet.LXI.txt" << endl; break;
	case 699: test_out << "sonnet.LXII.txt" << endl; break;
	case 700: test_out << "sonnet.LXIII.txt" << endl; break;
	case 701: test_out << "sonnet.LXIV.txt" << endl; break;
	case 702: test_out << "sonnet.LXIX.txt" << endl; break;
	case 703: test_out << "sonnet.LXV.txt" << endl; break;
	case 704: test_out << "sonnet.LXVI.txt" << endl; break;
	case 705: test_out << "sonnet.LXVII.txt" << endl; break;
	case 706: test_out << "sonnet.LXVIII.txt" << endl; break;
	case 707: test_out << "sonnet.LXX.txt" << endl; break;
	case 708: test_out << "sonnet.LXXI.txt" << endl; break;
	case 709: test_out << "sonnet.LXXII.txt" << endl; break;
	case 710: test_out << "sonnet.LXXIII.txt" << endl; break;
	case 711: test_out << "sonnet.LXXIV.txt" << endl; break;
	case 712: test_out << "sonnet.LXXIX.txt" << endl; break;
	case 713: test_out << "sonnet.LXXV.txt" << endl; break;
	case 714: test_out << "sonnet.LXXVI.txt" << endl; break;
	case 715: test_out << "sonnet.LXXVII.txt" << endl; break;
	case 716: test_out << "sonnet.LXXVIII.txt" << endl; break;
	case 717: test_out << "sonnet.LXXX.txt" << endl; break;
	case 718: test_out << "sonnet.LXXXI.txt" << endl; break;
	case 719: test_out << "sonnet.LXXXII.txt" << endl; break;
	case 720: test_out << "sonnet.LXXXIII.txt" << endl; break;
	case 721: test_out << "sonnet.LXXXIV.txt" << endl; break;
	case 722: test_out << "sonnet.LXXXIX.txt" << endl; break;
	case 723: test_out << "sonnet.LXXXV.txt" << endl; break;
	case 724: test_out << "sonnet.LXXXVI.txt" << endl; break;
	case 725: test_out << "sonnet.LXXXVII.txt" << endl; break;
	case 726: test_out << "sonnet.LXXXVIII.txt" << endl; break;
	case 727: test_out << "sonnet.V.txt" << endl; break;
	case 728: test_out << "sonnet.VI.txt" << endl; break;
	case 729: test_out << "sonnet.VII.txt" << endl; break;
	case 730: test_out << "sonnet.VIII.txt" << endl; break;
	case 731: test_out << "sonnet.X.txt" << endl; break;
	case 732: test_out << "sonnet.XC.txt" << endl; break;
	case 733: test_out << "sonnet.XCI.txt" << endl; break;
	case 734: test_out << "sonnet.XCII.txt" << endl; break;
	case 735: test_out << "sonnet.XCIII.txt" << endl; break;
	case 736: test_out << "sonnet.XCIV.txt" << endl; break;
	case 737: test_out << "sonnet.XCIX.txt" << endl; break;
	case 738: test_out << "sonnet.XCV.txt" << endl; break;
	case 739: test_out << "sonnet.XCVI.txt" << endl; break;
	case 740: test_out << "sonnet.XCVII.txt" << endl; break;
	case 741: test_out << "sonnet.XCVIII.txt" << endl; break;
	case 742: test_out << "sonnet.XI.txt" << endl; break;
	case 743: test_out << "sonnet.XII.txt" << endl; break;
	case 744: test_out << "sonnet.XIII.txt" << endl; break;
	case 745: test_out << "sonnet.XIV.txt" << endl; break;
	case 746: test_out << "sonnet.XIX.txt" << endl; break;
	case 747: test_out << "sonnet.XL.txt" << endl; break;
	case 748: test_out << "sonnet.XLI.txt" << endl; break;
	case 749: test_out << "sonnet.XLII.txt" << endl; break;
	case 750: test_out << "sonnet.XLIII.txt" << endl; break;
	case 751: test_out << "sonnet.XLIV.txt" << endl; break;
	case 752: test_out << "sonnet.XLIX.txt" << endl; break;
	case 753: test_out << "sonnet.XLV.txt" << endl; break;
	case 754: test_out << "sonnet.XLVI.txt" << endl; break;
	case 755: test_out << "sonnet.XLVII.txt" << endl; break;
	case 756: test_out << "sonnet.XLVIII.txt" << endl; break;
	case 757: test_out << "sonnet.XV.txt" << endl; break;
	case 758: test_out << "sonnet.XVI.txt" << endl; break;
	case 759: test_out << "sonnet.XVII.txt" << endl; break;
	case 760: test_out << "sonnet.XVIII.txt" << endl; break;
	case 761: test_out << "sonnet.XX.txt" << endl; break;
	case 762: test_out << "sonnet.XXI.txt" << endl; break;
	case 763: test_out << "sonnet.XXII.txt" << endl; break;
	case 764: test_out << "sonnet.XXIII.txt" << endl; break;
	case 765: test_out << "sonnet.XXIV.txt" << endl; break;
	case 766: test_out << "sonnet.XXIX.txt" << endl; break;
	case 767: test_out << "sonnet.XXV.txt" << endl; break;
	case 768: test_out << "sonnet.XXVI.txt" << endl; break;
	case 769: test_out << "sonnet.XXVII.txt" << endl; break;
	case 770: test_out << "sonnet.XXVIII.txt" << endl; break;
	case 771: test_out << "sonnet.XXX.txt" << endl; break;
	case 772: test_out << "sonnet.XXXI.txt" << endl; break;
	case 773: test_out << "sonnet.XXXII.txt" << endl; break;
	case 774: test_out << "sonnet.XXXIII.txt" << endl; break;
	case 775: test_out << "sonnet.XXXIV.txt" << endl; break;
	case 776: test_out << "sonnet.XXXIX.txt" << endl; break;
	case 777: test_out << "sonnet.XXXV.txt" << endl; break;
	case 778: test_out << "sonnet.XXXVI.txt" << endl; break;
	case 779: test_out << "sonnet.XXXVII.txt" << endl; break;
	case 780: test_out << "sonnet.XXXVIII.txt" << endl; break;
	case 781: test_out << "sonnets.txt" << endl; break;
	case 782: test_out << "taming_shrew.0.1.txt" << endl; break;
	case 783: test_out << "taming_shrew.0.2.txt" << endl; break;
	case 784: test_out << "taming_shrew.1.1.txt" << endl; break;
	case 785: test_out << "taming_shrew.1.2.txt" << endl; break;
	case 786: test_out << "taming_shrew.2.1.txt" << endl; break;
	case 787: test_out << "taming_shrew.3.1.txt" << endl; break;
	case 788: test_out << "taming_shrew.3.2.txt" << endl; break;
	case 789: test_out << "taming_shrew.4.1.txt" << endl; break;
	case 790: test_out << "taming_shrew.4.2.txt" << endl; break;
	case 791: test_out << "taming_shrew.4.3.txt" << endl; break;
	case 792: test_out << "taming_shrew.4.4.txt" << endl; break;
	case 793: test_out << "taming_shrew.4.5.txt" << endl; break;
	case 794: test_out << "taming_shrew.5.1.txt" << endl; break;
	case 795: test_out << "taming_shrew.5.2.txt" << endl; break;
	case 796: test_out << "tempest.1.1.txt" << endl; break;
	case 797: test_out << "tempest.1.2.txt" << endl; break;
	case 798: test_out << "tempest.2.1.txt" << endl; break;
	case 799: test_out << "tempest.2.2.txt" << endl; break;
	case 800: test_out << "tempest.3.1.txt" << endl; break;
	case 801: test_out << "tempest.3.2.txt" << endl; break;
	case 802: test_out << "tempest.3.3.txt" << endl; break;
	case 803: test_out << "tempest.4.1.txt" << endl; break;
	case 804: test_out << "tempest.5.1.txt" << endl; break;
	case 805: test_out << "timon.1.1.txt" << endl; break;
	case 806: test_out << "timon.1.2.txt" << endl; break;
	case 807: test_out << "timon.2.1.txt" << endl; break;
	case 808: test_out << "timon.2.2.txt" << endl; break;
	case 809: test_out << "timon.3.1.txt" << endl; break;
	case 810: test_out << "timon.3.2.txt" << endl; break;
	case 811: test_out << "timon.3.3.txt" << endl; break;
	case 812: test_out << "timon.3.4.txt" << endl; break;
	case 813: test_out << "timon.3.5.txt" << endl; break;
	case 814: test_out << "timon.3.6.txt" << endl; break;
	case 815: test_out << "timon.4.1.txt" << endl; break;
	case 816: test_out << "timon.4.2.txt" << endl; break;
	case 817: test_out << "timon.4.3.txt" << endl; break;
	case 818: test_out << "timon.5.1.txt" << endl; break;
	case 819: test_out << "timon.5.2.txt" << endl; break;
	case 820: test_out << "timon.5.3.txt" << endl; break;
	case 821: test_out << "timon.5.4.txt" << endl; break;
	case 822: test_out << "titus.1.1.txt" << endl; break;
	case 823: test_out << "titus.2.1.txt" << endl; break;
	case 824: test_out << "titus.2.2.txt" << endl; break;
	case 825: test_out << "titus.2.3.txt" << endl; break;
	case 826: test_out << "titus.2.4.txt" << endl; break;
	case 827: test_out << "titus.3.1.txt" << endl; break;
	case 828: test_out << "titus.3.2.txt" << endl; break;
	case 829: test_out << "titus.4.1.txt" << endl; break;
	case 830: test_out << "titus.4.2.txt" << endl; break;
	case 831: test_out << "titus.4.3.txt" << endl; break;
	case 832: test_out << "titus.4.4.txt" << endl; break;
	case 833: test_out << "titus.5.1.txt" << endl; break;
	case 834: test_out << "titus.5.2.txt" << endl; break;
	case 835: test_out << "titus.5.3.txt" << endl; break;
	case 836: test_out << "troilus_cressida.1.0.txt" << endl; break;
	case 837: test_out << "troilus_cressida.1.1.txt" << endl; break;
	case 838: test_out << "troilus_cressida.1.2.txt" << endl; break;
	case 839: test_out << "troilus_cressida.1.3.txt" << endl; break;
	case 840: test_out << "troilus_cressida.2.1.txt" << endl; break;
	case 841: test_out << "troilus_cressida.2.2.txt" << endl; break;
	case 842: test_out << "troilus_cressida.2.3.txt" << endl; break;
	case 843: test_out << "troilus_cressida.3.1.txt" << endl; break;
	case 844: test_out << "troilus_cressida.3.2.txt" << endl; break;
	case 845: test_out << "troilus_cressida.3.3.txt" << endl; break;
	case 846: test_out << "troilus_cressida.4.1.txt" << endl; break;
	case 847: test_out << "troilus_cressida.4.2.txt" << endl; break;
	case 848: test_out << "troilus_cressida.4.3.txt" << endl; break;
	case 849: test_out << "troilus_cressida.4.4.txt" << endl; break;
	case 850: test_out << "troilus_cressida.4.5.txt" << endl; break;
	case 851: test_out << "troilus_cressida.5.1.txt" << endl; break;
	case 852: test_out << "troilus_cressida.5.10.txt" << endl; break;
	case 853: test_out << "troilus_cressida.5.2.txt" << endl; break;
	case 854: test_out << "troilus_cressida.5.3.txt" << endl; break;
	case 855: test_out << "troilus_cressida.5.4.txt" << endl; break;
	case 856: test_out << "troilus_cressida.5.5.txt" << endl; break;
	case 857: test_out << "troilus_cressida.5.6.txt" << endl; break;
	case 858: test_out << "troilus_cressida.5.7.txt" << endl; break;
	case 859: test_out << "troilus_cressida.5.8.txt" << endl; break;
	case 860: test_out << "troilus_cressida.5.9.txt" << endl; break;
	case 861: test_out << "twelfth_night.1.1.txt" << endl; break;
	case 862: test_out << "twelfth_night.1.2.txt" << endl; break;
	case 863: test_out << "twelfth_night.1.3.txt" << endl; break;
	case 864: test_out << "twelfth_night.1.4.txt" << endl; break;
	case 865: test_out << "twelfth_night.1.5.txt" << endl; break;
	case 866: test_out << "twelfth_night.2.1.txt" << endl; break;
	case 867: test_out << "twelfth_night.2.2.txt" << endl; break;
	case 868: test_out << "twelfth_night.2.3.txt" << endl; break;
	case 869: test_out << "twelfth_night.2.4.txt" << endl; break;
	case 870: test_out << "twelfth_night.2.5.txt" << endl; break;
	case 871: test_out << "twelfth_night.3.1.txt" << endl; break;
	case 872: test_out << "twelfth_night.3.2.txt" << endl; break;
	case 873: test_out << "twelfth_night.3.3.txt" << endl; break;
	case 874: test_out << "twelfth_night.3.4.txt" << endl; break;
	case 875: test_out << "twelfth_night.4.1.txt" << endl; break;
	case 876: test_out << "twelfth_night.4.2.txt" << endl; break;
	case 877: test_out << "twelfth_night.4.3.txt" << endl; break;
	case 878: test_out << "twelfth_night.5.1.txt" << endl; break;
	case 879: test_out << "two_gentlemen.1.1.txt" << endl; break;
	case 880: test_out << "two_gentlemen.1.2.txt" << endl; break;
	case 881: test_out << "two_gentlemen.1.3.txt" << endl; break;
	case 882: test_out << "two_gentlemen.2.1.txt" << endl; break;
	case 883: test_out << "two_gentlemen.2.2.txt" << endl; break;
	case 884: test_out << "two_gentlemen.2.3.txt" << endl; break;
	case 885: test_out << "two_gentlemen.2.4.txt" << endl; break;
	case 886: test_out << "two_gentlemen.2.5.txt" << endl; break;
	case 887: test_out << "two_gentlemen.2.6.txt" << endl; break;
	case 888: test_out << "two_gentlemen.2.7.txt" << endl; break;
	case 889: test_out << "two_gentlemen.3.1.txt" << endl; break;
	case 890: test_out << "two_gentlemen.3.2.txt" << endl; break;
	case 891: test_out << "two_gentlemen.4.1.txt" << endl; break;
	case 892: test_out << "two_gentlemen.4.2.txt" << endl; break;
	case 893: test_out << "two_gentlemen.4.3.txt" << endl; break;
	case 894: test_out << "two_gentlemen.4.4.txt" << endl; break;
	case 895: test_out << "two_gentlemen.5.1.txt" << endl; break;
	case 896: test_out << "two_gentlemen.5.2.txt" << endl; break;
	case 897: test_out << "two_gentlemen.5.3.txt" << endl; break;
	case 898: test_out << "two_gentlemen.5.4.txt" << endl; break;
	case 899: test_out << "VenusAndAdonis.txt" << endl; break;
	case 900: test_out << "winters_tale.1.1.txt" << endl; break;
	case 901: test_out << "winters_tale.1.2.txt" << endl; break;
	case 902: test_out << "winters_tale.2.1.txt" << endl; break;
	case 903: test_out << "winters_tale.2.2.txt" << endl; break;
	case 904: test_out << "winters_tale.2.3.txt" << endl; break;
	case 905: test_out << "winters_tale.3.1.txt" << endl; break;
	case 906: test_out << "winters_tale.3.2.txt" << endl; break;
	case 907: test_out << "winters_tale.3.3.txt" << endl; break;
	case 908: test_out << "winters_tale.4.2.txt" << endl; break;
	case 909: test_out << "winters_tale.4.3.txt" << endl; break;
	case 910: test_out << "winters_tale.4.4.txt" << endl; break;
	case 911: test_out << "winters_tale.5.1.txt" << endl; break;
	case 912: test_out << "winters_tale.5.2.txt" << endl; break;
	case 913: test_out << "winters_tale.5.3.txt" << endl; break;
	/*case 0: printf("1henryiv.1.1.txt\n"); break;
	case 1: printf("1henryiv.1.2.txt\n"); break;
	case 2: printf("1henryiv.1.3.txt\n"); break;
	case 3: printf("1henryiv.2.1.txt\n"); break;
	case 4: printf("1henryiv.2.2.txt\n"); break;
	case 5: printf("1henryiv.2.3.txt\n"); break;
	case 6: printf("1henryiv.2.4.txt\n"); break;
	case 7: printf("1henryiv.3.1.txt\n"); break;
	case 8: printf("1henryiv.3.2.txt\n"); break;
	case 9: printf("1henryiv.4.1.txt\n"); break;
	case 10: printf("1henryiv.4.2.txt\n"); break;
	case 11: printf("1henryiv.4.3.txt\n"); break;
	case 12: printf("1henryiv.4.4.txt\n"); break;
	case 13: printf("1henryiv.5.1.txt\n"); break;
	case 14: printf("1henryiv.5.2.txt\n"); break;
	case 15: printf("1henryiv.5.3.txt\n"); break;
	case 16: printf("1henryiv.5.4.txt\n"); break;
	case 17: printf("1henryiv.5.5.txt\n"); break;
	case 18: printf("1henryvi.1.1.txt\n"); break;
	case 19: printf("1henryvi.1.2.txt\n"); break;
	case 20: printf("1henryvi.1.3.txt\n"); break;
	case 21: printf("1henryvi.1.4.txt\n"); break;
	case 22: printf("1henryvi.1.5.txt\n"); break;
	case 23: printf("1henryvi.1.6.txt\n"); break;
	case 24: printf("1henryvi.2.1.txt\n"); break;
	case 25: printf("1henryvi.2.2.txt\n"); break;
	case 26: printf("1henryvi.2.3.txt\n"); break;
	case 27: printf("1henryvi.2.4.txt\n"); break;
	case 28: printf("1henryvi.2.5.txt\n"); break;
	case 29: printf("1henryvi.3.1.txt\n"); break;
	case 30: printf("1henryvi.3.2.txt\n"); break;
	case 31: printf("1henryvi.3.3.txt\n"); break;
	case 32: printf("1henryvi.3.4.txt\n"); break;
	case 33: printf("1henryvi.4.1.txt\n"); break;
	case 34: printf("1henryvi.4.2.txt\n"); break;
	case 35: printf("1henryvi.4.3.txt\n"); break;
	case 36: printf("1henryvi.4.4.txt\n"); break;
	case 37: printf("1henryvi.4.5.txt\n"); break;
	case 38: printf("1henryvi.4.6.txt\n"); break;
	case 39: printf("1henryvi.4.7.txt\n"); break;
	case 40: printf("1henryvi.5.1.txt\n"); break;
	case 41: printf("1henryvi.5.2.txt\n"); break;
	case 42: printf("1henryvi.5.3.txt\n"); break;
	case 43: printf("1henryvi.5.4.txt\n"); break;
	case 44: printf("1henryvi.5.5.txt\n"); break;
	case 45: printf("2henryiv.0.0.txt\n"); break;
	case 46: printf("2henryiv.1.1.txt\n"); break;
	case 47: printf("2henryiv.1.2.txt\n"); break;
	case 48: printf("2henryiv.1.3.txt\n"); break;
	case 49: printf("2henryiv.2.1.txt\n"); break;
	case 50: printf("2henryiv.2.2.txt\n"); break;
	case 51: printf("2henryiv.2.3.txt\n"); break;
	case 52: printf("2henryiv.2.4.txt\n"); break;
	case 53: printf("2henryiv.3.1.txt\n"); break;
	case 54: printf("2henryiv.3.2.txt\n"); break;
	case 55: printf("2henryiv.4.1.txt\n"); break;
	case 56: printf("2henryiv.4.2.txt\n"); break;
	case 57: printf("2henryiv.4.3.txt\n"); break;
	case 58: printf("2henryiv.4.4.txt\n"); break;
	case 59: printf("2henryiv.4.5.txt\n"); break;
	case 60: printf("2henryiv.5.1.txt\n"); break;
	case 61: printf("2henryiv.5.2.txt\n"); break;
	case 62: printf("2henryiv.5.3.txt\n"); break;
	case 63: printf("2henryiv.5.4.txt\n"); break;
	case 64: printf("2henryiv.5.5.txt\n"); break;
	case 65: printf("2henryvi.1.1.txt\n"); break;
	case 66: printf("2henryvi.1.2.txt\n"); break;
	case 67: printf("2henryvi.1.3.txt\n"); break;
	case 68: printf("2henryvi.1.4.txt\n"); break;
	case 69: printf("2henryvi.2.1.txt\n"); break;
	case 70: printf("2henryvi.2.2.txt\n"); break;
	case 71: printf("2henryvi.2.3.txt\n"); break;
	case 72: printf("2henryvi.2.4.txt\n"); break;
	case 73: printf("2henryvi.3.1.txt\n"); break;
	case 74: printf("2henryvi.3.2.txt\n"); break;
	case 75: printf("2henryvi.3.3.txt\n"); break;
	case 76: printf("2henryvi.4.1.txt\n"); break;
	case 77: printf("2henryvi.4.10.txt\n"); break;
	case 78: printf("2henryvi.4.2.txt\n"); break;
	case 79: printf("2henryvi.4.3.txt\n"); break;
	case 80: printf("2henryvi.4.4.txt\n"); break;
	case 81: printf("2henryvi.4.5.txt\n"); break;
	case 82: printf("2henryvi.4.6.txt\n"); break;
	case 83: printf("2henryvi.4.7.txt\n"); break;
	case 84: printf("2henryvi.4.8.txt\n"); break;
	case 85: printf("2henryvi.4.9.txt\n"); break;
	case 86: printf("2henryvi.5.1.txt\n"); break;
	case 87: printf("2henryvi.5.2.txt\n"); break;
	case 88: printf("2henryvi.5.3.txt\n"); break;
	case 89: printf("3henryvi.1.1.txt\n"); break;
	case 90: printf("3henryvi.1.2.txt\n"); break;
	case 91: printf("3henryvi.1.3.txt\n"); break;
	case 92: printf("3henryvi.1.4.txt\n"); break;
	case 93: printf("3henryvi.2.1.txt\n"); break;
	case 94: printf("3henryvi.2.2.txt\n"); break;
	case 95: printf("3henryvi.2.3.txt\n"); break;
	case 96: printf("3henryvi.2.4.txt\n"); break;
	case 97: printf("3henryvi.2.5.txt\n"); break;
	case 98: printf("3henryvi.2.6.txt\n"); break;
	case 99: printf("3henryvi.3.1.txt\n"); break;
	case 100: printf("3henryvi.3.2.txt\n"); break;
	case 101: printf("3henryvi.3.3.txt\n"); break;
	case 102: printf("3henryvi.4.1.txt\n"); break;
	case 103: printf("3henryvi.4.2.txt\n"); break;
	case 104: printf("3henryvi.4.3.txt\n"); break;
	case 105: printf("3henryvi.4.4.txt\n"); break;
	case 106: printf("3henryvi.4.5.txt\n"); break;
	case 107: printf("3henryvi.4.6.txt\n"); break;
	case 108: printf("3henryvi.4.7.txt\n"); break;
	case 109: printf("3henryvi.4.8.txt\n"); break;
	case 110: printf("3henryvi.5.1.txt\n"); break;
	case 111: printf("3henryvi.5.2.txt\n"); break;
	case 112: printf("3henryvi.5.3.txt\n"); break;
	case 113: printf("3henryvi.5.4.txt\n"); break;
	case 114: printf("3henryvi.5.5.txt\n"); break;
	case 115: printf("3henryvi.5.6.txt\n"); break;
	case 116: printf("3henryvi.5.7.txt\n"); break;
	case 117: printf("allswell.1.1.txt\n"); break;
	case 118: printf("allswell.1.2.txt\n"); break;
	case 119: printf("allswell.1.3.txt\n"); break;
	case 120: printf("allswell.2.1.txt\n"); break;
	case 121: printf("allswell.2.2.txt\n"); break;
	case 122: printf("allswell.2.3.txt\n"); break;
	case 123: printf("allswell.2.4.txt\n"); break;
	case 124: printf("allswell.2.5.txt\n"); break;
	case 125: printf("allswell.3.1.txt\n"); break;
	case 126: printf("allswell.3.2.txt\n"); break;
	case 127: printf("allswell.3.3.txt\n"); break;
	case 128: printf("allswell.3.4.txt\n"); break;
	case 129: printf("allswell.3.5.txt\n"); break;
	case 130: printf("allswell.3.6.txt\n"); break;
	case 131: printf("allswell.3.7.txt\n"); break;
	case 132: printf("allswell.4.1.txt\n"); break;
	case 133: printf("allswell.4.2.txt\n"); break;
	case 134: printf("allswell.4.3.txt\n"); break;
	case 135: printf("allswell.4.4.txt\n"); break;
	case 136: printf("allswell.4.5.txt\n"); break;
	case 137: printf("allswell.5.1.txt\n"); break;
	case 138: printf("allswell.5.2.txt\n"); break;
	case 139: printf("allswell.5.3.txt\n"); break;
	case 140: printf("asyoulikeit.1.1.txt\n"); break;
	case 141: printf("asyoulikeit.1.2.txt\n"); break;
	case 142: printf("asyoulikeit.1.3.txt\n"); break;
	case 143: printf("asyoulikeit.2.1.txt\n"); break;
	case 144: printf("asyoulikeit.2.2.txt\n"); break;
	case 145: printf("asyoulikeit.2.3.txt\n"); break;
	case 146: printf("asyoulikeit.2.4.txt\n"); break;
	case 147: printf("asyoulikeit.2.5.txt\n"); break;
	case 148: printf("asyoulikeit.2.6.txt\n"); break;
	case 149: printf("asyoulikeit.2.7.txt\n"); break;
	case 150: printf("asyoulikeit.3.1.txt\n"); break;
	case 151: printf("asyoulikeit.3.2.txt\n"); break;
	case 152: printf("asyoulikeit.3.3.txt\n"); break;
	case 153: printf("asyoulikeit.3.4.txt\n"); break;
	case 154: printf("asyoulikeit.3.5.txt\n"); break;
	case 155: printf("asyoulikeit.4.1.txt\n"); break;
	case 156: printf("asyoulikeit.4.2.txt\n"); break;
	case 157: printf("asyoulikeit.4.3.txt\n"); break;
	case 158: printf("asyoulikeit.5.1.txt\n"); break;
	case 159: printf("asyoulikeit.5.2.txt\n"); break;
	case 160: printf("asyoulikeit.5.3.txt\n"); break;
	case 161: printf("asyoulikeit.5.4.txt\n"); break;
	case 162: printf("cleopatra.1.1.txt\n"); break;
	case 163: printf("cleopatra.1.2.txt\n"); break;
	case 164: printf("cleopatra.1.3.txt\n"); break;
	case 165: printf("cleopatra.1.4.txt\n"); break;
	case 166: printf("cleopatra.1.5.txt\n"); break;
	case 167: printf("cleopatra.2.1.txt\n"); break;
	case 168: printf("cleopatra.2.2.txt\n"); break;
	case 169: printf("cleopatra.2.3.txt\n"); break;
	case 170: printf("cleopatra.2.4.txt\n"); break;
	case 171: printf("cleopatra.2.5.txt\n"); break;
	case 172: printf("cleopatra.2.6.txt\n"); break;
	case 173: printf("cleopatra.2.7.txt\n"); break;
	case 174: printf("cleopatra.3.1.txt\n"); break;
	case 175: printf("cleopatra.3.10.txt\n"); break;
	case 176: printf("cleopatra.3.11.txt\n"); break;
	case 177: printf("cleopatra.3.12.txt\n"); break;
	case 178: printf("cleopatra.3.13.txt\n"); break;
	case 179: printf("cleopatra.3.2.txt\n"); break;
	case 180: printf("cleopatra.3.3.txt\n"); break;
	case 181: printf("cleopatra.3.4.txt\n"); break;
	case 182: printf("cleopatra.3.5.txt\n"); break;
	case 183: printf("cleopatra.3.6.txt\n"); break;
	case 184: printf("cleopatra.3.7.txt\n"); break;
	case 185: printf("cleopatra.3.8.txt\n"); break;
	case 186: printf("cleopatra.3.9.txt\n"); break;
	case 187: printf("cleopatra.4.1.txt\n"); break;
	case 188: printf("cleopatra.4.10.txt\n"); break;
	case 189: printf("cleopatra.4.11.txt\n"); break;
	case 190: printf("cleopatra.4.12.txt\n"); break;
	case 191: printf("cleopatra.4.13.txt\n"); break;
	case 192: printf("cleopatra.4.14.txt\n"); break;
	case 193: printf("cleopatra.4.15.txt\n"); break;
	case 194: printf("cleopatra.4.2.txt\n"); break;
	case 195: printf("cleopatra.4.3.txt\n"); break;
	case 196: printf("cleopatra.4.4.txt\n"); break;
	case 197: printf("cleopatra.4.5.txt\n"); break;
	case 198: printf("cleopatra.4.6.txt\n"); break;
	case 199: printf("cleopatra.4.7.txt\n"); break;
	case 200: printf("cleopatra.4.8.txt\n"); break;
	case 201: printf("cleopatra.4.9.txt\n"); break;
	case 202: printf("cleopatra.5.1.txt\n"); break;
	case 203: printf("cleopatra.5.2.txt\n"); break;
	case 204: printf("comedy_errors.1.1.txt\n"); break;
	case 205: printf("comedy_errors.1.2.txt\n"); break;
	case 206: printf("comedy_errors.2.1.txt\n"); break;
	case 207: printf("comedy_errors.2.2.txt\n"); break;
	case 208: printf("comedy_errors.3.1.txt\n"); break;
	case 209: printf("comedy_errors.3.2.txt\n"); break;
	case 210: printf("comedy_errors.4.1.txt\n"); break;
	case 211: printf("comedy_errors.4.2.txt\n"); break;
	case 212: printf("comedy_errors.4.3.txt\n"); break;
	case 213: printf("comedy_errors.4.4.txt\n"); break;
	case 214: printf("comedy_errors.5.1.txt\n"); break;
	case 215: printf("coriolanus.1.1.txt\n"); break;
	case 216: printf("coriolanus.1.10.txt\n"); break;
	case 217: printf("coriolanus.1.2.txt\n"); break;
	case 218: printf("coriolanus.1.3.txt\n"); break;
	case 219: printf("coriolanus.1.4.txt\n"); break;
	case 220: printf("coriolanus.1.5.txt\n"); break;
	case 221: printf("coriolanus.1.6.txt\n"); break;
	case 222: printf("coriolanus.1.7.txt\n"); break;
	case 223: printf("coriolanus.1.8.txt\n"); break;
	case 224: printf("coriolanus.1.9.txt\n"); break;
	case 225: printf("coriolanus.2.1.txt\n"); break;
	case 226: printf("coriolanus.2.2.txt\n"); break;
	case 227: printf("coriolanus.2.3.txt\n"); break;
	case 228: printf("coriolanus.3.1.txt\n"); break;
	case 229: printf("coriolanus.3.2.txt\n"); break;
	case 230: printf("coriolanus.3.3.txt\n"); break;
	case 231: printf("coriolanus.4.1.txt\n"); break;
	case 232: printf("coriolanus.4.2.txt\n"); break;
	case 233: printf("coriolanus.4.3.txt\n"); break;
	case 234: printf("coriolanus.4.4.txt\n"); break;
	case 235: printf("coriolanus.4.5.txt\n"); break;
	case 236: printf("coriolanus.4.6.txt\n"); break;
	case 237: printf("coriolanus.4.7.txt\n"); break;
	case 238: printf("coriolanus.5.1.txt\n"); break;
	case 239: printf("coriolanus.5.2.txt\n"); break;
	case 240: printf("coriolanus.5.3.txt\n"); break;
	case 241: printf("coriolanus.5.4.txt\n"); break;
	case 242: printf("coriolanus.5.5.txt\n"); break;
	case 243: printf("coriolanus.5.6.txt\n"); break;
	case 244: printf("cymbeline.1.1.txt\n"); break;
	case 245: printf("cymbeline.1.2.txt\n"); break;
	case 246: printf("cymbeline.1.3.txt\n"); break;
	case 247: printf("cymbeline.1.4.txt\n"); break;
	case 248: printf("cymbeline.1.5.txt\n"); break;
	case 249: printf("cymbeline.1.6.txt\n"); break;
	case 250: printf("cymbeline.2.1.txt\n"); break;
	case 251: printf("cymbeline.2.2.txt\n"); break;
	case 252: printf("cymbeline.2.4.txt\n"); break;
	case 253: printf("cymbeline.2.5.txt\n"); break;
	case 254: printf("cymbeline.3.1.txt\n"); break;
	case 255: printf("cymbeline.3.2.txt\n"); break;
	case 256: printf("cymbeline.3.3.txt\n"); break;
	case 257: printf("cymbeline.3.4.txt\n"); break;
	case 258: printf("cymbeline.3.5.txt\n"); break;
	case 259: printf("cymbeline.3.6.txt\n"); break;
	case 260: printf("cymbeline.3.7.txt\n"); break;
	case 261: printf("cymbeline.4.1.txt\n"); break;
	case 262: printf("cymbeline.4.2.txt\n"); break;
	case 263: printf("cymbeline.4.3.txt\n"); break;
	case 264: printf("cymbeline.4.4.txt\n"); break;
	case 265: printf("cymbeline.5.1.txt\n"); break;
	case 266: printf("cymbeline.5.2.txt\n"); break;
	case 267: printf("cymbeline.5.3.txt\n"); break;
	case 268: printf("cymbeline.5.4.txt\n"); break;
	case 269: printf("cymbeline.5.5.txt\n"); break;
	case 270: printf("elegy.txt\n"); break;
	case 271: printf("hamlet.1.1.txt\n"); break;
	case 272: printf("hamlet.1.2.txt\n"); break;
	case 273: printf("hamlet.1.3.txt\n"); break;
	case 274: printf("hamlet.1.4.txt\n"); break;
	case 275: printf("hamlet.1.5.txt\n"); break;
	case 276: printf("hamlet.2.1.txt\n"); break;
	case 277: printf("hamlet.2.2.txt\n"); break;
	case 278: printf("hamlet.3.1.txt\n"); break;
	case 279: printf("hamlet.3.2.txt\n"); break;
	case 280: printf("hamlet.3.3.txt\n"); break;
	case 281: printf("hamlet.3.4.txt\n"); break;
	case 282: printf("hamlet.4.1.txt\n"); break;
	case 283: printf("hamlet.4.2.txt\n"); break;
	case 284: printf("hamlet.4.3.txt\n"); break;
	case 285: printf("hamlet.4.4.txt\n"); break;
	case 286: printf("hamlet.4.5.txt\n"); break;
	case 287: printf("hamlet.4.6.txt\n"); break;
	case 288: printf("hamlet.4.7.txt\n"); break;
	case 289: printf("hamlet.5.1.txt\n"); break;
	case 290: printf("hamlet.5.2.txt\n"); break;
	case 291: printf("henryv.1.0.txt\n"); break;
	case 292: printf("henryv.1.1.txt\n"); break;
	case 293: printf("henryv.1.2.txt\n"); break;
	case 294: printf("henryv.2.0.txt\n"); break;
	case 295: printf("henryv.2.1.txt\n"); break;
	case 296: printf("henryv.2.2.txt\n"); break;
	case 297: printf("henryv.2.3.txt\n"); break;
	case 298: printf("henryv.2.4.txt\n"); break;
	case 299: printf("henryv.3.0.txt\n"); break;
	case 300: printf("henryv.3.1.txt\n"); break;
	case 301: printf("henryv.3.2.txt\n"); break;
	case 302: printf("henryv.3.3.txt\n"); break;
	case 303: printf("henryv.3.4.txt\n"); break;
	case 304: printf("henryv.3.5.txt\n"); break;
	case 305: printf("henryv.3.6.txt\n"); break;
	case 306: printf("henryv.3.7.txt\n"); break;
	case 307: printf("henryv.4.0.txt\n"); break;
	case 308: printf("henryv.4.1.txt\n"); break;
	case 309: printf("henryv.4.2.txt\n"); break;
	case 310: printf("henryv.4.3.txt\n"); break;
	case 311: printf("henryv.4.4.txt\n"); break;
	case 312: printf("henryv.4.5.txt\n"); break;
	case 313: printf("henryv.4.6.txt\n"); break;
	case 314: printf("henryv.4.7.txt\n"); break;
	case 315: printf("henryv.4.8.txt\n"); break;
	case 316: printf("henryv.5.0.txt\n"); break;
	case 317: printf("henryv.5.1.txt\n"); break;
	case 318: printf("henryv.5.2.txt\n"); break;
	case 319: printf("henryviii.1.0.txt\n"); break;
	case 320: printf("henryviii.1.1.txt\n"); break;
	case 321: printf("henryviii.1.2.txt\n"); break;
	case 322: printf("henryviii.1.3.txt\n"); break;
	case 323: printf("henryviii.1.4.txt\n"); break;
	case 324: printf("henryviii.2.1.txt\n"); break;
	case 325: printf("henryviii.2.2.txt\n"); break;
	case 326: printf("henryviii.2.3.txt\n"); break;
	case 327: printf("henryviii.2.4.txt\n"); break;
	case 328: printf("henryviii.3.1.txt\n"); break;
	case 329: printf("henryviii.3.2.txt\n"); break;
	case 330: printf("henryviii.4.1.txt\n"); break;
	case 331: printf("henryviii.4.2.txt\n"); break;
	case 332: printf("henryviii.5.1.txt\n"); break;
	case 333: printf("henryviii.5.2.txt\n"); break;
	case 334: printf("henryviii.5.3.txt\n"); break;
	case 335: printf("henryviii.5.4.txt\n"); break;
	case 336: printf("henryviii.5.5.txt\n"); break;
	case 337: printf("john.1.1.txt\n"); break;
	case 338: printf("john.2.1.txt\n"); break;
	case 339: printf("john.3.1.txt\n"); break;
	case 340: printf("john.3.2.txt\n"); break;
	case 341: printf("john.3.3.txt\n"); break;
	case 342: printf("john.3.4.txt\n"); break;
	case 343: printf("john.4.1.txt\n"); break;
	case 344: printf("john.4.2.txt\n"); break;
	case 345: printf("john.4.3.txt\n"); break;
	case 346: printf("john.5.1.txt\n"); break;
	case 347: printf("john.5.2.txt\n"); break;
	case 348: printf("john.5.3.txt\n"); break;
	case 349: printf("john.5.4.txt\n"); break;
	case 350: printf("john.5.5.txt\n"); break;
	case 351: printf("john.5.6.txt\n"); break;
	case 352: printf("john.5.7.txt\n"); break;
	case 353: printf("julius_caesar.1.1.txt\n"); break;
	case 354: printf("julius_caesar.1.2.txt\n"); break;
	case 355: printf("julius_caesar.1.3.txt\n"); break;
	case 356: printf("julius_caesar.2.1.txt\n"); break;
	case 357: printf("julius_caesar.2.2.txt\n"); break;
	case 358: printf("julius_caesar.2.3.txt\n"); break;
	case 359: printf("julius_caesar.2.4.txt\n"); break;
	case 360: printf("julius_caesar.3.1.txt\n"); break;
	case 361: printf("julius_caesar.3.2.txt\n"); break;
	case 362: printf("julius_caesar.3.3.txt\n"); break;
	case 363: printf("julius_caesar.4.1.txt\n"); break;
	case 364: printf("julius_caesar.4.2.txt\n"); break;
	case 365: printf("julius_caesar.4.3.txt\n"); break;
	case 366: printf("julius_caesar.5.1.txt\n"); break;
	case 367: printf("julius_caesar.5.2.txt\n"); break;
	case 368: printf("julius_caesar.5.3.txt\n"); break;
	case 369: printf("julius_caesar.5.4.txt\n"); break;
	case 370: printf("julius_caesar.5.5.txt\n"); break;
	case 371: printf("lear.1.1.txt\n"); break;
	case 372: printf("lear.1.2.txt\n"); break;
	case 373: printf("lear.1.3.txt\n"); break;
	case 374: printf("lear.1.4.txt\n"); break;
	case 375: printf("lear.1.5.txt\n"); break;
	case 376: printf("lear.2.1.txt\n"); break;
	case 377: printf("lear.2.2.txt\n"); break;
	case 378: printf("lear.2.3.txt\n"); break;
	case 379: printf("lear.2.4.txt\n"); break;
	case 380: printf("lear.3.1.txt\n"); break;
	case 381: printf("lear.3.2.txt\n"); break;
	case 382: printf("lear.3.3.txt\n"); break;
	case 383: printf("lear.3.4.txt\n"); break;
	case 384: printf("lear.3.5.txt\n"); break;
	case 385: printf("lear.3.6.txt\n"); break;
	case 386: printf("lear.3.7.txt\n"); break;
	case 387: printf("lear.4.1.txt\n"); break;
	case 388: printf("lear.4.2.txt\n"); break;
	case 389: printf("lear.4.3.txt\n"); break;
	case 390: printf("lear.4.4.txt\n"); break;
	case 391: printf("lear.4.5.txt\n"); break;
	case 392: printf("lear.4.6.txt\n"); break;
	case 393: printf("lear.4.7.txt\n"); break;
	case 394: printf("lear.5.1.txt\n"); break;
	case 395: printf("lear.5.2.txt\n"); break;
	case 396: printf("lear.5.3.txt\n"); break;
	case 397: printf("lll.1.1.txt\n"); break;
	case 398: printf("lll.1.2.txt\n"); break;
	case 399: printf("lll.2.1.txt\n"); break;
	case 400: printf("lll.3.1.txt\n"); break;
	case 401: printf("lll.4.1.txt\n"); break;
	case 402: printf("lll.4.2.txt\n"); break;
	case 403: printf("lll.4.3.txt\n"); break;
	case 404: printf("lll.5.1.txt\n"); break;
	case 405: printf("lll.5.2.txt\n"); break;
	case 406: printf("LoversComplaint.txt\n"); break;
	case 407: printf("macbeth.1.1.txt\n"); break;
	case 408: printf("macbeth.1.2.txt\n"); break;
	case 409: printf("macbeth.1.3.txt\n"); break;
	case 410: printf("macbeth.1.4.txt\n"); break;
	case 411: printf("macbeth.1.5.txt\n"); break;
	case 412: printf("macbeth.1.6.txt\n"); break;
	case 413: printf("macbeth.1.7.txt\n"); break;
	case 414: printf("macbeth.2.1.txt\n"); break;
	case 415: printf("macbeth.2.2.txt\n"); break;
	case 416: printf("macbeth.2.3.txt\n"); break;
	case 417: printf("macbeth.2.4.txt\n"); break;
	case 418: printf("macbeth.3.1.txt\n"); break;
	case 419: printf("macbeth.3.2.txt\n"); break;
	case 420: printf("macbeth.3.3.txt\n"); break;
	case 421: printf("macbeth.3.4.txt\n"); break;
	case 422: printf("macbeth.3.5.txt\n"); break;
	case 423: printf("macbeth.3.6.txt\n"); break;
	case 424: printf("macbeth.4.1.txt\n"); break;
	case 425: printf("macbeth.4.2.txt\n"); break;
	case 426: printf("macbeth.4.3.txt\n"); break;
	case 427: printf("macbeth.5.1.txt\n"); break;
	case 428: printf("macbeth.5.2.txt\n"); break;
	case 429: printf("macbeth.5.3.txt\n"); break;
	case 430: printf("macbeth.5.4.txt\n"); break;
	case 431: printf("macbeth.5.5.txt\n"); break;
	case 432: printf("macbeth.5.6.txt\n"); break;
	case 433: printf("macbeth.5.7.txt\n"); break;
	case 434: printf("macbeth.5.8.txt\n"); break;
	case 435: printf("measure.1.1.txt\n"); break;
	case 436: printf("measure.1.2.txt\n"); break;
	case 437: printf("measure.1.3.txt\n"); break;
	case 438: printf("measure.1.4.txt\n"); break;
	case 439: printf("measure.2.1.txt\n"); break;
	case 440: printf("measure.2.2.txt\n"); break;
	case 441: printf("measure.2.3.txt\n"); break;
	case 442: printf("measure.2.4.txt\n"); break;
	case 443: printf("measure.3.1.txt\n"); break;
	case 444: printf("measure.3.2.txt\n"); break;
	case 445: printf("measure.4.1.txt\n"); break;
	case 446: printf("measure.4.2.txt\n"); break;
	case 447: printf("measure.4.3.txt\n"); break;
	case 448: printf("measure.4.4.txt\n"); break;
	case 449: printf("measure.4.5.txt\n"); break;
	case 450: printf("measure.4.6.txt\n"); break;
	case 451: printf("measure.5.1.txt\n"); break;
	case 452: printf("merchant.1.1.txt\n"); break;
	case 453: printf("merchant.1.2.txt\n"); break;
	case 454: printf("merchant.1.3.txt\n"); break;
	case 455: printf("merchant.2.1.txt\n"); break;
	case 456: printf("merchant.2.2.txt\n"); break;
	case 457: printf("merchant.2.3.txt\n"); break;
	case 458: printf("merchant.2.4.txt\n"); break;
	case 459: printf("merchant.2.5.txt\n"); break;
	case 460: printf("merchant.2.6.txt\n"); break;
	case 461: printf("merchant.2.7.txt\n"); break;
	case 462: printf("merchant.2.8.txt\n"); break;
	case 463: printf("merchant.2.9.txt\n"); break;
	case 464: printf("merchant.3.1.txt\n"); break;
	case 465: printf("merchant.3.2.txt\n"); break;
	case 466: printf("merchant.3.3.txt\n"); break;
	case 467: printf("merchant.3.4.txt\n"); break;
	case 468: printf("merchant.3.5.txt\n"); break;
	case 469: printf("merchant.4.1.txt\n"); break;
	case 470: printf("merchant.4.2.txt\n"); break;
	case 471: printf("merchant.5.1.txt\n"); break;
	case 472: printf("merry_wives.1.1.txt\n"); break;
	case 473: printf("merry_wives.1.2.txt\n"); break;
	case 474: printf("merry_wives.1.3.txt\n"); break;
	case 475: printf("merry_wives.1.4.txt\n"); break;
	case 476: printf("merry_wives.2.1.txt\n"); break;
	case 477: printf("merry_wives.2.2.txt\n"); break;
	case 478: printf("merry_wives.2.3.txt\n"); break;
	case 479: printf("merry_wives.3.1.txt\n"); break;
	case 480: printf("merry_wives.3.2.txt\n"); break;
	case 481: printf("merry_wives.3.3.txt\n"); break;
	case 482: printf("merry_wives.3.4.txt\n"); break;
	case 483: printf("merry_wives.3.5.txt\n"); break;
	case 484: printf("merry_wives.4.1.txt\n"); break;
	case 485: printf("merry_wives.4.2.txt\n"); break;
	case 486: printf("merry_wives.4.3.txt\n"); break;
	case 487: printf("merry_wives.4.4.txt\n"); break;
	case 488: printf("merry_wives.4.5.txt\n"); break;
	case 489: printf("merry_wives.4.6.txt\n"); break;
	case 490: printf("merry_wives.5.1.txt\n"); break;
	case 491: printf("merry_wives.5.2.txt\n"); break;
	case 492: printf("merry_wives.5.3.txt\n"); break;
	case 493: printf("merry_wives.5.4.txt\n"); break;
	case 494: printf("merry_wives.5.5.txt\n"); break;
	case 495: printf("midsummer.1.1.txt\n"); break;
	case 496: printf("midsummer.1.2.txt\n"); break;
	case 497: printf("midsummer.2.1.txt\n"); break;
	case 498: printf("midsummer.2.2.txt\n"); break;
	case 499: printf("midsummer.3.1.txt\n"); break;
	case 500: printf("midsummer.3.2.txt\n"); break;
	case 501: printf("midsummer.4.1.txt\n"); break;
	case 502: printf("midsummer.4.2.txt\n"); break;
	case 503: printf("midsummer.5.1.txt\n"); break;
	case 504: printf("much_ado.1.1.txt\n"); break;
	case 505: printf("much_ado.1.2.txt\n"); break;
	case 506: printf("much_ado.1.3.txt\n"); break;
	case 507: printf("much_ado.2.1.txt\n"); break;
	case 508: printf("much_ado.2.2.txt\n"); break;
	case 509: printf("much_ado.2.3.txt\n"); break;
	case 510: printf("much_ado.3.1.txt\n"); break;
	case 511: printf("much_ado.3.2.txt\n"); break;
	case 512: printf("much_ado.3.3.txt\n"); break;
	case 513: printf("much_ado.3.4.txt\n"); break;
	case 514: printf("much_ado.3.5.txt\n"); break;
	case 515: printf("much_ado.4.1.txt\n"); break;
	case 516: printf("much_ado.4.2.txt\n"); break;
	case 517: printf("much_ado.5.1.txt\n"); break;
	case 518: printf("much_ado.5.2.txt\n"); break;
	case 519: printf("much_ado.5.3.txt\n"); break;
	case 520: printf("much_ado.5.4.txt\n"); break;
	case 521: printf("othello.1.1.txt\n"); break;
	case 522: printf("othello.1.2.txt\n"); break;
	case 523: printf("othello.1.3.txt\n"); break;
	case 524: printf("othello.2.1.txt\n"); break;
	case 525: printf("othello.2.2.txt\n"); break;
	case 526: printf("othello.2.3.txt\n"); break;
	case 527: printf("othello.3.1.txt\n"); break;
	case 528: printf("othello.3.2.txt\n"); break;
	case 529: printf("othello.3.3.txt\n"); break;
	case 530: printf("othello.3.4.txt\n"); break;
	case 531: printf("othello.4.1.txt\n"); break;
	case 532: printf("othello.4.2.txt\n"); break;
	case 533: printf("othello.4.3.txt\n"); break;
	case 534: printf("othello.5.1.txt\n"); break;
	case 535: printf("othello.5.2.txt\n"); break;
	case 536: printf("pericles.1.0.txt\n"); break;
	case 537: printf("pericles.1.1.txt\n"); break;
	case 538: printf("pericles.1.2.txt\n"); break;
	case 539: printf("pericles.1.3.txt\n"); break;
	case 540: printf("pericles.1.4.txt\n"); break;
	case 541: printf("pericles.2.1.txt\n"); break;
	case 542: printf("pericles.2.2.txt\n"); break;
	case 543: printf("pericles.2.3.txt\n"); break;
	case 544: printf("pericles.2.4.txt\n"); break;
	case 545: printf("pericles.2.5.txt\n"); break;
	case 546: printf("pericles.3.2.txt\n"); break;
	case 547: printf("pericles.3.3.txt\n"); break;
	case 548: printf("pericles.3.4.txt\n"); break;
	case 549: printf("pericles.4.1.txt\n"); break;
	case 550: printf("pericles.4.2.txt\n"); break;
	case 551: printf("pericles.4.3.txt\n"); break;
	case 552: printf("pericles.4.5.txt\n"); break;
	case 553: printf("pericles.4.6.txt\n"); break;
	case 554: printf("pericles.5.1.txt\n"); break;
	case 555: printf("pericles.5.3.txt\n"); break;
	case 556: printf("RapeOfLucrece.txt\n"); break;
	case 557: printf("richardii.1.1.txt\n"); break;
	case 558: printf("richardii.1.2.txt\n"); break;
	case 559: printf("richardii.1.3.txt\n"); break;
	case 560: printf("richardii.1.4.txt\n"); break;
	case 561: printf("richardii.2.1.txt\n"); break;
	case 562: printf("richardii.2.2.txt\n"); break;
	case 563: printf("richardii.2.3.txt\n"); break;
	case 564: printf("richardii.2.4.txt\n"); break;
	case 565: printf("richardii.3.1.txt\n"); break;
	case 566: printf("richardii.3.2.txt\n"); break;
	case 567: printf("richardii.3.3.txt\n"); break;
	case 568: printf("richardii.3.4.txt\n"); break;
	case 569: printf("richardii.4.1.txt\n"); break;
	case 570: printf("richardii.5.1.txt\n"); break;
	case 571: printf("richardii.5.2.txt\n"); break;
	case 572: printf("richardii.5.3.txt\n"); break;
	case 573: printf("richardii.5.4.txt\n"); break;
	case 574: printf("richardii.5.5.txt\n"); break;
	case 575: printf("richardii.5.6.txt\n"); break;
	case 576: printf("richardiii.1.1.txt\n"); break;
	case 577: printf("richardiii.1.2.txt\n"); break;
	case 578: printf("richardiii.1.3.txt\n"); break;
	case 579: printf("richardiii.1.4.txt\n"); break;
	case 580: printf("richardiii.2.1.txt\n"); break;
	case 581: printf("richardiii.2.2.txt\n"); break;
	case 582: printf("richardiii.2.3.txt\n"); break;
	case 583: printf("richardiii.2.4.txt\n"); break;
	case 584: printf("richardiii.3.1.txt\n"); break;
	case 585: printf("richardiii.3.2.txt\n"); break;
	case 586: printf("richardiii.3.3.txt\n"); break;
	case 587: printf("richardiii.3.4.txt\n"); break;
	case 588: printf("richardiii.3.5.txt\n"); break;
	case 589: printf("richardiii.3.6.txt\n"); break;
	case 590: printf("richardiii.3.7.txt\n"); break;
	case 591: printf("richardiii.4.1.txt\n"); break;
	case 592: printf("richardiii.4.2.txt\n"); break;
	case 593: printf("richardiii.4.3.txt\n"); break;
	case 594: printf("richardiii.4.4.txt\n"); break;
	case 595: printf("richardiii.4.5.txt\n"); break;
	case 596: printf("richardiii.5.1.txt\n"); break;
	case 597: printf("richardiii.5.2.txt\n"); break;
	case 598: printf("richardiii.5.3.txt\n"); break;
	case 599: printf("richardiii.5.4.txt\n"); break;
	case 600: printf("richardiii.5.5.txt\n"); break;
	case 601: printf("romeo_juliet.1.0.txt\n"); break;
	case 602: printf("romeo_juliet.1.1.txt\n"); break;
	case 603: printf("romeo_juliet.1.2.txt\n"); break;
	case 604: printf("romeo_juliet.1.3.txt\n"); break;
	case 605: printf("romeo_juliet.1.4.txt\n"); break;
	case 606: printf("romeo_juliet.1.5.txt\n"); break;
	case 607: printf("romeo_juliet.2.0.txt\n"); break;
	case 608: printf("romeo_juliet.2.1.txt\n"); break;
	case 609: printf("romeo_juliet.2.2.txt\n"); break;
	case 610: printf("romeo_juliet.2.3.txt\n"); break;
	case 611: printf("romeo_juliet.2.4.txt\n"); break;
	case 612: printf("romeo_juliet.2.5.txt\n"); break;
	case 613: printf("romeo_juliet.2.6.txt\n"); break;
	case 614: printf("romeo_juliet.3.1.txt\n"); break;
	case 615: printf("romeo_juliet.3.2.txt\n"); break;
	case 616: printf("romeo_juliet.3.3.txt\n"); break;
	case 617: printf("romeo_juliet.3.4.txt\n"); break;
	case 618: printf("romeo_juliet.3.5.txt\n"); break;
	case 619: printf("romeo_juliet.4.1.txt\n"); break;
	case 620: printf("romeo_juliet.4.2.txt\n"); break;
	case 621: printf("romeo_juliet.4.3.txt\n"); break;
	case 622: printf("romeo_juliet.4.4.txt\n"); break;
	case 623: printf("romeo_juliet.4.5.txt\n"); break;
	case 624: printf("romeo_juliet.5.1.txt\n"); break;
	case 625: printf("romeo_juliet.5.2.txt\n"); break;
	case 626: printf("romeo_juliet.5.3.txt\n"); break;
	case 627: printf("sonnet.C.txt\n"); break;
	case 628: printf("sonnet.CI.txt\n"); break;
	case 629: printf("sonnet.CII.txt\n"); break;
	case 630: printf("sonnet.CIII.txt\n"); break;
	case 631: printf("sonnet.CIV.txt\n"); break;
	case 632: printf("sonnet.CIX.txt\n"); break;
	case 633: printf("sonnet.CL.txt\n"); break;
	case 634: printf("sonnet.CLI.txt\n"); break;
	case 635: printf("sonnet.CLII.txt\n"); break;
	case 636: printf("sonnet.CLIII.txt\n"); break;
	case 637: printf("sonnet.CLIV.txt\n"); break;
	case 638: printf("sonnet.CV.txt\n"); break;
	case 639: printf("sonnet.CVI.txt\n"); break;
	case 640: printf("sonnet.CVII.txt\n"); break;
	case 641: printf("sonnet.CVIII.txt\n"); break;
	case 642: printf("sonnet.CX.txt\n"); break;
	case 643: printf("sonnet.CXI.txt\n"); break;
	case 644: printf("sonnet.CXII.txt\n"); break;
	case 645: printf("sonnet.CXIII.txt\n"); break;
	case 646: printf("sonnet.CXIV.txt\n"); break;
	case 647: printf("sonnet.CXIX.txt\n"); break;
	case 648: printf("sonnet.CXL.txt\n"); break;
	case 649: printf("sonnet.CXLI.txt\n"); break;
	case 650: printf("sonnet.CXLII.txt\n"); break;
	case 651: printf("sonnet.CXLIII.txt\n"); break;
	case 652: printf("sonnet.CXLIV.txt\n"); break;
	case 653: printf("sonnet.CXLIX.txt\n"); break;
	case 654: printf("sonnet.CXLV.txt\n"); break;
	case 655: printf("sonnet.CXLVI.txt\n"); break;
	case 656: printf("sonnet.CXLVII.txt\n"); break;
	case 657: printf("sonnet.CXLVIII.txt\n"); break;
	case 658: printf("sonnet.CXV.txt\n"); break;
	case 659: printf("sonnet.CXVI.txt\n"); break;
	case 660: printf("sonnet.CXVII.txt\n"); break;
	case 661: printf("sonnet.CXVIII.txt\n"); break;
	case 662: printf("sonnet.CXX.txt\n"); break;
	case 663: printf("sonnet.CXXI.txt\n"); break;
	case 664: printf("sonnet.CXXII.txt\n"); break;
	case 665: printf("sonnet.CXXIII.txt\n"); break;
	case 666: printf("sonnet.CXXIV.txt\n"); break;
	case 667: printf("sonnet.CXXIX.txt\n"); break;
	case 668: printf("sonnet.CXXV.txt\n"); break;
	case 669: printf("sonnet.CXXVI.txt\n"); break;
	case 670: printf("sonnet.CXXVII.txt\n"); break;
	case 671: printf("sonnet.CXXVIII.txt\n"); break;
	case 672: printf("sonnet.CXXX.txt\n"); break;
	case 673: printf("sonnet.CXXXI.txt\n"); break;
	case 674: printf("sonnet.CXXXII.txt\n"); break;
	case 675: printf("sonnet.CXXXIII.txt\n"); break;
	case 676: printf("sonnet.CXXXIV.txt\n"); break;
	case 677: printf("sonnet.CXXXIX.txt\n"); break;
	case 678: printf("sonnet.CXXXV.txt\n"); break;
	case 679: printf("sonnet.CXXXVI.txt\n"); break;
	case 680: printf("sonnet.CXXXVII.txt\n"); break;
	case 681: printf("sonnet.CXXXVIII.txt\n"); break;
	case 682: printf("sonnet.I.txt\n"); break;
	case 683: printf("sonnet.II.txt\n"); break;
	case 684: printf("sonnet.III.txt\n"); break;
	case 685: printf("sonnet.IV.txt\n"); break;
	case 686: printf("sonnet.IX.txt\n"); break;
	case 687: printf("sonnet.L.txt\n"); break;
	case 688: printf("sonnet.LI.txt\n"); break;
	case 689: printf("sonnet.LII.txt\n"); break;
	case 690: printf("sonnet.LIII.txt\n"); break;
	case 691: printf("sonnet.LIV.txt\n"); break;
	case 692: printf("sonnet.LIX.txt\n"); break;
	case 693: printf("sonnet.LV.txt\n"); break;
	case 694: printf("sonnet.LVI.txt\n"); break;
	case 695: printf("sonnet.LVII.txt\n"); break;
	case 696: printf("sonnet.LVIII.txt\n"); break;
	case 697: printf("sonnet.LX.txt\n"); break;
	case 698: printf("sonnet.LXI.txt\n"); break;
	case 699: printf("sonnet.LXII.txt\n"); break;
	case 700: printf("sonnet.LXIII.txt\n"); break;
	case 701: printf("sonnet.LXIV.txt\n"); break;
	case 702: printf("sonnet.LXIX.txt\n"); break;
	case 703: printf("sonnet.LXV.txt\n"); break;
	case 704: printf("sonnet.LXVI.txt\n"); break;
	case 705: printf("sonnet.LXVII.txt\n"); break;
	case 706: printf("sonnet.LXVIII.txt\n"); break;
	case 707: printf("sonnet.LXX.txt\n"); break;
	case 708: printf("sonnet.LXXI.txt\n"); break;
	case 709: printf("sonnet.LXXII.txt\n"); break;
	case 710: printf("sonnet.LXXIII.txt\n"); break;
	case 711: printf("sonnet.LXXIV.txt\n"); break;
	case 712: printf("sonnet.LXXIX.txt\n"); break;
	case 713: printf("sonnet.LXXV.txt\n"); break;
	case 714: printf("sonnet.LXXVI.txt\n"); break;
	case 715: printf("sonnet.LXXVII.txt\n"); break;
	case 716: printf("sonnet.LXXVIII.txt\n"); break;
	case 717: printf("sonnet.LXXX.txt\n"); break;
	case 718: printf("sonnet.LXXXI.txt\n"); break;
	case 719: printf("sonnet.LXXXII.txt\n"); break;
	case 720: printf("sonnet.LXXXIII.txt\n"); break;
	case 721: printf("sonnet.LXXXIV.txt\n"); break;
	case 722: printf("sonnet.LXXXIX.txt\n"); break;
	case 723: printf("sonnet.LXXXV.txt\n"); break;
	case 724: printf("sonnet.LXXXVI.txt\n"); break;
	case 725: printf("sonnet.LXXXVII.txt\n"); break;
	case 726: printf("sonnet.LXXXVIII.txt\n"); break;
	case 727: printf("sonnet.V.txt\n"); break;
	case 728: printf("sonnet.VI.txt\n"); break;
	case 729: printf("sonnet.VII.txt\n"); break;
	case 730: printf("sonnet.VIII.txt\n"); break;
	case 731: printf("sonnet.X.txt\n"); break;
	case 732: printf("sonnet.XC.txt\n"); break;
	case 733: printf("sonnet.XCI.txt\n"); break;
	case 734: printf("sonnet.XCII.txt\n"); break;
	case 735: printf("sonnet.XCIII.txt\n"); break;
	case 736: printf("sonnet.XCIV.txt\n"); break;
	case 737: printf("sonnet.XCIX.txt\n"); break;
	case 738: printf("sonnet.XCV.txt\n"); break;
	case 739: printf("sonnet.XCVI.txt\n"); break;
	case 740: printf("sonnet.XCVII.txt\n"); break;
	case 741: printf("sonnet.XCVIII.txt\n"); break;
	case 742: printf("sonnet.XI.txt\n"); break;
	case 743: printf("sonnet.XII.txt\n"); break;
	case 744: printf("sonnet.XIII.txt\n"); break;
	case 745: printf("sonnet.XIV.txt\n"); break;
	case 746: printf("sonnet.XIX.txt\n"); break;
	case 747: printf("sonnet.XL.txt\n"); break;
	case 748: printf("sonnet.XLI.txt\n"); break;
	case 749: printf("sonnet.XLII.txt\n"); break;
	case 750: printf("sonnet.XLIII.txt\n"); break;
	case 751: printf("sonnet.XLIV.txt\n"); break;
	case 752: printf("sonnet.XLIX.txt\n"); break;
	case 753: printf("sonnet.XLV.txt\n"); break;
	case 754: printf("sonnet.XLVI.txt\n"); break;
	case 755: printf("sonnet.XLVII.txt\n"); break;
	case 756: printf("sonnet.XLVIII.txt\n"); break;
	case 757: printf("sonnet.XV.txt\n"); break;
	case 758: printf("sonnet.XVI.txt\n"); break;
	case 759: printf("sonnet.XVII.txt\n"); break;
	case 760: printf("sonnet.XVIII.txt\n"); break;
	case 761: printf("sonnet.XX.txt\n"); break;
	case 762: printf("sonnet.XXI.txt\n"); break;
	case 763: printf("sonnet.XXII.txt\n"); break;
	case 764: printf("sonnet.XXIII.txt\n"); break;
	case 765: printf("sonnet.XXIV.txt\n"); break;
	case 766: printf("sonnet.XXIX.txt\n"); break;
	case 767: printf("sonnet.XXV.txt\n"); break;
	case 768: printf("sonnet.XXVI.txt\n"); break;
	case 769: printf("sonnet.XXVII.txt\n"); break;
	case 770: printf("sonnet.XXVIII.txt\n"); break;
	case 771: printf("sonnet.XXX.txt\n"); break;
	case 772: printf("sonnet.XXXI.txt\n"); break;
	case 773: printf("sonnet.XXXII.txt\n"); break;
	case 774: printf("sonnet.XXXIII.txt\n"); break;
	case 775: printf("sonnet.XXXIV.txt\n"); break;
	case 776: printf("sonnet.XXXIX.txt\n"); break;
	case 777: printf("sonnet.XXXV.txt\n"); break;
	case 778: printf("sonnet.XXXVI.txt\n"); break;
	case 779: printf("sonnet.XXXVII.txt\n"); break;
	case 780: printf("sonnet.XXXVIII.txt\n"); break;
	case 781: printf("sonnets.txt\n"); break;
	case 782: printf("taming_shrew.0.1.txt\n"); break;
	case 783: printf("taming_shrew.0.2.txt\n"); break;
	case 784: printf("taming_shrew.1.1.txt\n"); break;
	case 785: printf("taming_shrew.1.2.txt\n"); break;
	case 786: printf("taming_shrew.2.1.txt\n"); break;
	case 787: printf("taming_shrew.3.1.txt\n"); break;
	case 788: printf("taming_shrew.3.2.txt\n"); break;
	case 789: printf("taming_shrew.4.1.txt\n"); break;
	case 790: printf("taming_shrew.4.2.txt\n"); break;
	case 791: printf("taming_shrew.4.3.txt\n"); break;
	case 792: printf("taming_shrew.4.4.txt\n"); break;
	case 793: printf("taming_shrew.4.5.txt\n"); break;
	case 794: printf("taming_shrew.5.1.txt\n"); break;
	case 795: printf("taming_shrew.5.2.txt\n"); break;
	case 796: printf("tempest.1.1.txt\n"); break;
	case 797: printf("tempest.1.2.txt\n"); break;
	case 798: printf("tempest.2.1.txt\n"); break;
	case 799: printf("tempest.2.2.txt\n"); break;
	case 800: printf("tempest.3.1.txt\n"); break;
	case 801: printf("tempest.3.2.txt\n"); break;
	case 802: printf("tempest.3.3.txt\n"); break;
	case 803: printf("tempest.4.1.txt\n"); break;
	case 804: printf("tempest.5.1.txt\n"); break;
	case 805: printf("timon.1.1.txt\n"); break;
	case 806: printf("timon.1.2.txt\n"); break;
	case 807: printf("timon.2.1.txt\n"); break;
	case 808: printf("timon.2.2.txt\n"); break;
	case 809: printf("timon.3.1.txt\n"); break;
	case 810: printf("timon.3.2.txt\n"); break;
	case 811: printf("timon.3.3.txt\n"); break;
	case 812: printf("timon.3.4.txt\n"); break;
	case 813: printf("timon.3.5.txt\n"); break;
	case 814: printf("timon.3.6.txt\n"); break;
	case 815: printf("timon.4.1.txt\n"); break;
	case 816: printf("timon.4.2.txt\n"); break;
	case 817: printf("timon.4.3.txt\n"); break;
	case 818: printf("timon.5.1.txt\n"); break;
	case 819: printf("timon.5.2.txt\n"); break;
	case 820: printf("timon.5.3.txt\n"); break;
	case 821: printf("timon.5.4.txt\n"); break;
	case 822: printf("titus.1.1.txt\n"); break;
	case 823: printf("titus.2.1.txt\n"); break;
	case 824: printf("titus.2.2.txt\n"); break;
	case 825: printf("titus.2.3.txt\n"); break;
	case 826: printf("titus.2.4.txt\n"); break;
	case 827: printf("titus.3.1.txt\n"); break;
	case 828: printf("titus.3.2.txt\n"); break;
	case 829: printf("titus.4.1.txt\n"); break;
	case 830: printf("titus.4.2.txt\n"); break;
	case 831: printf("titus.4.3.txt\n"); break;
	case 832: printf("titus.4.4.txt\n"); break;
	case 833: printf("titus.5.1.txt\n"); break;
	case 834: printf("titus.5.2.txt\n"); break;
	case 835: printf("titus.5.3.txt\n"); break;
	case 836: printf("troilus_cressida.1.0.txt\n"); break;
	case 837: printf("troilus_cressida.1.1.txt\n"); break;
	case 838: printf("troilus_cressida.1.2.txt\n"); break;
	case 839: printf("troilus_cressida.1.3.txt\n"); break;
	case 840: printf("troilus_cressida.2.1.txt\n"); break;
	case 841: printf("troilus_cressida.2.2.txt\n"); break;
	case 842: printf("troilus_cressida.2.3.txt\n"); break;
	case 843: printf("troilus_cressida.3.1.txt\n"); break;
	case 844: printf("troilus_cressida.3.2.txt\n"); break;
	case 845: printf("troilus_cressida.3.3.txt\n"); break;
	case 846: printf("troilus_cressida.4.1.txt\n"); break;
	case 847: printf("troilus_cressida.4.2.txt\n"); break;
	case 848: printf("troilus_cressida.4.3.txt\n"); break;
	case 849: printf("troilus_cressida.4.4.txt\n"); break;
	case 850: printf("troilus_cressida.4.5.txt\n"); break;
	case 851: printf("troilus_cressida.5.1.txt\n"); break;
	case 852: printf("troilus_cressida.5.10.txt\n"); break;
	case 853: printf("troilus_cressida.5.2.txt\n"); break;
	case 854: printf("troilus_cressida.5.3.txt\n"); break;
	case 855: printf("troilus_cressida.5.4.txt\n"); break;
	case 856: printf("troilus_cressida.5.5.txt\n"); break;
	case 857: printf("troilus_cressida.5.6.txt\n"); break;
	case 858: printf("troilus_cressida.5.7.txt\n"); break;
	case 859: printf("troilus_cressida.5.8.txt\n"); break;
	case 860: printf("troilus_cressida.5.9.txt\n"); break;
	case 861: printf("twelfth_night.1.1.txt\n"); break;
	case 862: printf("twelfth_night.1.2.txt\n"); break;
	case 863: printf("twelfth_night.1.3.txt\n"); break;
	case 864: printf("twelfth_night.1.4.txt\n"); break;
	case 865: printf("twelfth_night.1.5.txt\n"); break;
	case 866: printf("twelfth_night.2.1.txt\n"); break;
	case 867: printf("twelfth_night.2.2.txt\n"); break;
	case 868: printf("twelfth_night.2.3.txt\n"); break;
	case 869: printf("twelfth_night.2.4.txt\n"); break;
	case 870: printf("twelfth_night.2.5.txt\n"); break;
	case 871: printf("twelfth_night.3.1.txt\n"); break;
	case 872: printf("twelfth_night.3.2.txt\n"); break;
	case 873: printf("twelfth_night.3.3.txt\n"); break;
	case 874: printf("twelfth_night.3.4.txt\n"); break;
	case 875: printf("twelfth_night.4.1.txt\n"); break;
	case 876: printf("twelfth_night.4.2.txt\n"); break;
	case 877: printf("twelfth_night.4.3.txt\n"); break;
	case 878: printf("twelfth_night.5.1.txt\n"); break;
	case 879: printf("two_gentlemen.1.1.txt\n"); break;
	case 880: printf("two_gentlemen.1.2.txt\n"); break;
	case 881: printf("two_gentlemen.1.3.txt\n"); break;
	case 882: printf("two_gentlemen.2.1.txt\n"); break;
	case 883: printf("two_gentlemen.2.2.txt\n"); break;
	case 884: printf("two_gentlemen.2.3.txt\n"); break;
	case 885: printf("two_gentlemen.2.4.txt\n"); break;
	case 886: printf("two_gentlemen.2.5.txt\n"); break;
	case 887: printf("two_gentlemen.2.6.txt\n"); break;
	case 888: printf("two_gentlemen.2.7.txt\n"); break;
	case 889: printf("two_gentlemen.3.1.txt\n"); break;
	case 890: printf("two_gentlemen.3.2.txt\n"); break;
	case 891: printf("two_gentlemen.4.1.txt\n"); break;
	case 892: printf("two_gentlemen.4.2.txt\n"); break;
	case 893: printf("two_gentlemen.4.3.txt\n"); break;
	case 894: printf("two_gentlemen.4.4.txt\n"); break;
	case 895: printf("two_gentlemen.5.1.txt\n"); break;
	case 896: printf("two_gentlemen.5.2.txt\n"); break;
	case 897: printf("two_gentlemen.5.3.txt\n"); break;
	case 898: printf("two_gentlemen.5.4.txt\n"); break;
	case 899: printf("VenusAndAdonis.txt\n"); break;
	case 900: printf("winters_tale.1.1.txt\n"); break;
	case 901: printf("winters_tale.1.2.txt\n"); break;
	case 902: printf("winters_tale.2.1.txt\n"); break;
	case 903: printf("winters_tale.2.2.txt\n"); break;
	case 904: printf("winters_tale.2.3.txt\n"); break;
	case 905: printf("winters_tale.3.1.txt\n"); break;
	case 906: printf("winters_tale.3.2.txt\n"); break;
	case 907: printf("winters_tale.3.3.txt\n"); break;
	case 908: printf("winters_tale.4.2.txt\n"); break;
	case 909: printf("winters_tale.4.3.txt\n"); break;
	case 910: printf("winters_tale.4.4.txt\n"); break;
	case 911: printf("winters_tale.5.1.txt\n"); break;
	case 912: printf("winters_tale.5.2.txt\n"); break;*/
	}

}