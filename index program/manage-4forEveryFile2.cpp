/* ����ÿһƪ�ĵ�,�õ�term-postinglist*/ 
#include<io.h>
#include <fstream>
#include <iostream>
#include<vector>
#include<list>
#include<string>
#include<map> 
#include<algorithm> 

#include <stdio.h>
#include <stdlib.h>
#include "quadHash.h"
using namespace std;

int comp(const void* a, const void* b)//c���԰�ıȽϺ��� 
{
	if (((Cell*)a)->key < ((Cell*)b)->key) return -1;
	else return 1;
}

int main(int argc, char** argv) {
	char * filePath = "";
	  //~����Ҫ�޸�·����
	vector<string> files,names;//�洢�ļ��ľ��Ե�ַ�����ֵġ����顱�� 
	  //�ļ����:
	long   hFile = 0;
	  //�ļ���Ϣ:
	struct _finddata_t fileinfo;//_finddata_t��һ�����ڶ�ȡ�����ļ���Ϣ�Ľṹ�� 
	string p;
	if ((hFile = _findfirst(p.assign(filePath).append("\\*.txt").c_str(), &fileinfo)) != -1)
	  //�ҳ�Ŀ¼������.txt�ļ��� _findfirst����һ���ַ����� _finddata_tָ�룬��λ���ҵ��ļ��еĵ�һ������û���ҵ��򷵻�-1 
	{
		do
		{
			names.push_back(fileinfo.name);//�����ִ��������У����к�׺ 
			files.push_back(p.assign(filePath).append("\\").append(fileinfo.name));// �����Ե�ַ���������� �� 
		} while (_findnext(hFile, &fileinfo) == 0);//_findnext��������λ����һ���ļ�����û����һ���򷵻�0. 
		_findclose(hFile);//�ر� 
	}

	FILE * pf, *pf2;
	int docID = 0;
	int size = files.size();

	map<string, int> Dmap;//����ӳ�� 
	fstream docmap;
	docmap.open("newDocID.txt", ios::in);//��ȡID-�����ĵ� 
	while (docmap.eof() == false)
	{
		int x;
		string name;
		docmap >> x;
		docmap >> name;
		Dmap[name] = x;
	}//�Ժ��������֣�����txt�����ɵõ��ļ�ID���� Dmap����hi.txt������ 

	for (int j = 0;j < size;j++)//��ÿ���ļ� 
	{
		string name, old;
		char*cname, *cold;
		old = files[j].c_str();
		cold = &old[0];
		pf = fopen(cold, "r");
		if (pf == NULL) { printf("File not found\n"); getchar();exit(12); }
		name = "" + old.substr(sizeof(""));
		cname = &name[0];
		pf2 = fopen(cname, "w");
		if (pf2 == NULL) { printf("File not found\n"); exit(11); }

		HashTable H;
		H = InitializeTable(4003);//����hash�� 
		char ch; int i;
		char str[40];//�洢ÿ��term 
		ElementType* ptem;//��ʱԪ�ص�Ԫ��ָ�� 
		unsigned long hashVal;//�洢һ�����ʵ�hashֵ���Ǵ��ַ�����������ӳ�䡣 
		unsigned int cellNum;//��hash��ĵڼ�����Ŀ�С� 

		string strName;
		strName = names[j].substr(8);
		
		while (feof(pf) == 0)//δ���ļ���β��
		{
			do {
				ch = fgetc(pf);
			} while (ch != EOF && (ch<97 || ch>122));//������Сд��ĸʱ����һֱ����Ԥ�����Ѿ�����д��ΪСд����

			for (i = 0;i < 40;i++)
			{
				str[i] = 0;
			}

			hashVal = ch - 96;//������ĸ���˳�do-while��ͬʱ��hashVal��ֵ ��
			str[0] = ch;i = 1;//�洢term����ĸ 
			ch = fgetc(pf);
			while (1)//�������õ���ʣ�µ���ĸ�� 
			{
				if (ch != EOF && ch >= 97 && ch <= 122) {
					str[i] = ch;i++;
					hashVal = (hashVal << 5) + ch - 96;//��ĸλ�ü�Ȩ��Ȩ��32���ķ���ӳ�䵽������ 
					ch = fgetc(pf);
				}
				else {//һ��term���� 
					str[i] = '\0';
					break;
				}
			}

			cellNum = Find(hashVal, H);
			if ((H->TheCells[cellNum]).Info == Legitimate) {//����term�Ѿ���hash���� 
				ptem = (H->TheCells)[cellNum].pElement;// 
				(ptem->t1f)++;
			}
			else {//empty��deleted
				ptem = (ElementType*)malloc(sizeof(ElementType));//����Ŀ 

				ptem->docID = Dmap[strName];
				/*ptem->term = (char*)malloc(sizeof(str) + 1);
				strcpy(ptem->term, str);*/
				ptem->t1f = 1;
				Insert(hashVal, ptem, H);
			}
		} //�����ļ���β�� 
		  //���� 

		qsort(H->TheCells, H->TableSize, sizeof(Cell), comp);//������hash������ 
		for (i = 0;i < H->TableSize;i++)
		{
			if (H->TheCells[i].pElement != NULL)
			{
				ptem = H->TheCells[i].pElement;
				//printf("%s  ", ptem->term);
				fprintf(pf2, "%ul  %d  %u\n", H->TheCells[i].key, ptem->docID, ptem->t1f);
			}//���������term-postinglist ��� 
		}
		DestroyTable(H);
		printf("file%d handled\n", j);
	}
	docmap.close();
	cout << endl;
	cout << "ok" << endl;
	system("pause");
	return 0;
}

