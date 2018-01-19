#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#define MAXTABLESIZE 100000
#define KEYLENGTH 15                   // �ؼ����ַ�������󳤶� 
#define TABLE_SIZE 5000


typedef char ElementType[KEYLENGTH + 1];  //�ؼ��ʵ�����Ϊ�ַ���
typedef unsigned long Index;                     //ɢ�еĵ�ַ����Ϊint

typedef struct LNode *PtrToLNode;
struct LNode {
	ElementType Data; //�ؼ���
	int times;  //�ܹ����ֵĴ���
	unsigned long  *docID; //�����ĵ���ID		
	int *docTimes;
	PtrToLNode Next;
};
typedef PtrToLNode Position;
typedef PtrToLNode List;


typedef struct TblNode *HashTable;
struct TblNode {
	int TableSize; //�����󳤶�
	List Heads;   //�������ͷָ�������
};

int NextPrime(int N);
unsigned long Hash(char* key, int tablesize);
HashTable CreateTable(int TableSize);
Position Find(HashTable H, ElementType Key);
void Insert(HashTable H);
void DestroyTable(HashTable H);
void transfer(char string[], char result[]); //������ĵ���ת��Ϊ�ַ�����ʽ����ֵ
void FindMost(unsigned long a[], int number, unsigned long index[]);


int main()
{
	FILE *fp1;
	char query[160]; //��ѯ���
	ElementType words[10]; //��ֺ�Ĺؼ���
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
	printf("������ϣ���ʱ��%dms\n", (finish - start));
	printf("Input words to start your query\n");
	//�Ѳ�ѯ���ֽ��һ��������
	gets(query); //�����ѯ
	start = clock();

	len = strlen(query); //���㳤��
	for (i = 0; i <= len; i++)
	{
		if (query[i] != ' ') //�����ո���ʼ��һ������
		{
			words[k1][k2] = query[i]; //���ַ�����ɵ���
			k2++;
		}
		else {
			words[k1][k2] = '\0';//�ϸ��ʽ���
			transfer(words[k1], valueOfquery[k1]);//�Ѹս����Ĵ�ת������ֵ
			k2 = 0;   //�¸��ʿ�ʼ
			k1++;
		}

	}
	words[k1][k2] = '\0';//�ϸ��ʽ���
	transfer(words[k1], valueOfquery[k1]);//�����һ����ת������ֵ

	int numOfwords = k1+1; //��ѯ���ܵ�����
	for (i = 0; i <numOfwords; i++)
		p[i] = Find(H, valueOfquery[i]);  //�ڹ�ϣ���в���ÿ���ؼ���

	for (i = 0; i <numOfwords; i++)
	{
		if (p[i])
			numOfDoc = numOfDoc + p[i]->times;//�������ĵ���
	}
	if (numOfDoc == 0) //û�ҵ�
	{
		printf("Please Inpur More Information!\n");//�����ʾ
		system("pause");
		return 0;
	}
	sortedID = (unsigned long*)malloc(sizeof(unsigned long)*numOfDoc);//�����ڴ�

	//���²�����ÿ�����ʵĲ�ѯ�������ı���������sortedID��
	for (j = 0; j < numOfDoc; j++) //һ����numOfDoc ����
	{

		while(1) { //��min��ʼ��
			if (p[k] && (indexOfid[k]<p[k]->times)) //���p[k]�鵽�Ľ����Ϊ��,�Ҵ�ʱp[k]->docID�л���Ԫ��
			{
				min = p[k]->docID[indexOfid[k]];//��min��ֵΪ p[k]->docID�����е�ǰ����Сֵ��Ҳ�����±�ΪindexOfid[k]��Ӧ��ֵ
				k = 0;//Ϊ��һ�θ�ֵ����ʼ��
				break;
			}
			k++;//��һ����ѯ���
		}
		for (i = 0; i < numOfwords; i++)//���ҵ�ǰp[0]��p[n]�е���Сֵ
		{
			if (p[i]&&(indexOfid[i]<p[i]->times)&&(p[i]->docID[indexOfid[i]] <= min))//��minС
			{
				min = p[i]->docID[indexOfid[i]]; //����min
				minP = i; //��¼�ǵ�i����ѯ���
			}
		}
		indexOfid[minP]++; //��i����ѯ������±��һ
		sortedID[j] = min; //����Сֵ��������
    }
	

	FindMost(sortedID, numOfDoc, index);//�ҵ�sortedID�г��ִ��������ȶ�id���±�
	i = 0;
	printf("Here are the results:\n");//������
	while (index[i] != -1) //�±겻����-1
	{
		printf("Doc %lu  ", sortedID[index[i]]);//����±��Ӧ��ֵ
		i++;
		if (!(i%5))
			printf("\n");//ÿ��������һ����
	}

	finish = clock();
	printf(" \n");

	printf("��ѯ��ʱ��%dms", (finish - start) );

	system("pause");
}

int NextPrime(int N)
{ /* ���ش���N�Ҳ�����MAXTABLESIZE����С���� */
	int i, p = (N % 2) ? N + 2 : N + 1; /*�Ӵ���N����һ��������ʼ */

	while (p <= MAXTABLESIZE) {
		for (i = (int)sqrt(p); i>2; i--)
			if (!(p%i)) break; /* p�������� */
		if (i == 2) break; /* for����������˵��p������ */
		else  p += 2; /* ������̽��һ������ */
	}
	return p;
}

unsigned long Hash(char* key, int tablesize) //��ϣ����
{
	unsigned long h = 0;
	while (*key != 0)  //λ��ӳ��
	{
		h = (h << 5) + *key++;
	}
	return h%tablesize;

}


HashTable CreateTable(int TableSize) //������ϣ��
{
	HashTable H;
	int i;

	H = (HashTable)malloc(sizeof(struct TblNode));//�����ڴ�
	H->TableSize = NextPrime(TableSize);
	H->Heads = (List)malloc(H->TableSize*sizeof(struct LNode)); //Ϊָ�������ڴ�
	for (i = 0; i<H->TableSize; i++) { //��ʼ��
		H->Heads[i].Data[0] = '\0';
		H->Heads[i].Next = NULL;
	}

	return H;
}

Position Find(HashTable H, ElementType Key) //���ҹؼ���
{
	Position P; //��¼λ��
	Index Pos; //��ϣֵ����head������±�
	Pos = Hash(Key, H->TableSize);  //�����ϣֵ
	P = H->Heads[Pos].Next;
	while (P && strcmp(P->Data, Key)) //���αȽ�������ÿ���ؼ���
		P = P->Next;

	return P; //���ز��ҽ�����ؼ�����Ȼ��ָ��
}

void Insert(HashTable H) //���뺯��
{
	Position P, NewCell; //���Ҫ������¹ؼ���
	Index Pos; //�±�
	FILE *fp1;   //�ļ�ָ��
	ElementType Key; //�ؼ���
	int i, j;  //ѭ������

	 fopen_s(&fp1,"d:\\Index.txt", "r"); //ֻ����ʽ���ļ�

	while (1)
	{

		if (fscanf_s(fp1, "%s", Key,16) == EOF) //��ȡ�ؼ���
			break;
		if ((Key[0] == '#') && (Key[1] == '#')) //�Ա�ʶ����ͷ
		{
			P = Find(H, Key + 2);
			if (!P) //δ�ҵ��ؼ��ʣ����Բ���
			{
				NewCell = (Position)malloc(sizeof(struct LNode)); //����ռ�
				strcpy_s(NewCell->Data, Key + 2); //�ѹؼ��ʴ���

				if (fscanf_s(fp1, "%d", &(NewCell->times)) == EOF) //��ȡ���ֵ�Ƶ��
					break;
				NewCell->docID = (unsigned long*)malloc(sizeof(unsigned long)*NewCell->times);
				NewCell->docTimes = (int*)malloc(sizeof(int)*NewCell->times);

				for (i = 0; i < NewCell->times; i++) //��ȡ�����ļ�id
				{
					if (fscanf_s(fp1, "%lu", &(NewCell->docID[i])) == EOF)
						break;
					if (fscanf_s(fp1, "%d", &(NewCell->docTimes)) == EOF) //��ȡ��ĳ���ļ��г��ֵĴ���
						break;
				}
				Pos = Hash(Key + 2, H->TableSize); /* ��ʼɢ��λ�� */
												   /* ��NewCell����ΪH->Heads[Pos]����ĵ�1����� */
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

	/* �ͷ�ÿ������Ľ�� */
	for (i = 0; i<H->TableSize; i++) {
		P = H->Heads[i].Next;
		while (P) {
			Tmp = P->Next;
			free(P);
			P = Tmp;
		}
	}
	free(H->Heads); /* �ͷ�ͷ������� */
	free(H);        /* �ͷ�ɢ�б��� */
}
void transfer(char string[],char result[])//���ַ���ת���ɹ�ϣֵ
{
	unsigned long value; //��ϣֵ
	int i = 0;
	int lenth = 0; //��ϣֵ�ĳ���
	unsigned long value0; 
	value = 0; //��ʼ��
	while (string[i]) //û��ĩβ
	{
		if (string[i] >= 'A'&&string[i] <= 'Z') //���Ǵ�д��ĸ���������Сд
			string[i] = string[i] - 'A' + 'a';
		if (string[i]>='a'&&string[i]<='z') {
			
			value = (value << 5) + string[i] - 'a'+1;//��λ
		}
		i++;
	}
	value0 = value; //����
	do
	{
		lenth++;
		value0 = value0 / 10;//�������ĳ���

	} while (value0);
	result[lenth] = '\0'; //�ַ���ĩβ
	for (i = lenth - 1; i >= 0; i--)//����ת�����ַ���
	{
		result[i] = value % 10+'0';
		value = value / 10;
	}
}

void FindMost(unsigned long sortedID[], int number, unsigned long index[])//�ҵ�˳������sortedID�������ظ���������Ԫ�ص��±�index
{
	int i,j;//ѭ������
	int count=1;//����
	index[0] = 0;
	int max = 1; //��ʼ���ظ�������
	j = 0; //index���±�
	for (i = 1; i < number; i++) //����ÿһ��Ԫ��
	{
		if (sortedID[i] == sortedID[i - 1])//�����������һ��Ԫ��
		{
			count++; //��������һ
			if (count == max) //�����Ԫ���ظ��Ĵ�����������ظ�����
			{
				index[++j] = i; //��¼���±�
				index[j + 1] = -1; //��������β���
			}
			if (count > max)//�����Ԫ���ظ�������������ظ�����
			{
				j = 0; //����j
				index[j] = i; //��ʱindex��ֻ�и�Ԫ�ض�Ӧ�±�
				index[j + 1] = -1;//��β���
				max = count; //����max
			}
		}
		else { //������һԪ�ز�һ��
			count = 1;//���¼���
			if (count == max) //ͬ��
			{
				index[++j] = i;
				index[j + 1] = -1;
			}
		}
	}


}
