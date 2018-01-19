/* 处理每一篇文档,得到term-postinglist*/ 
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

int comp(const void* a, const void* b)//c语言版的比较函数 
{
	if (((Cell*)a)->key < ((Cell*)b)->key) return -1;
	else return 1;
}

int main(int argc, char** argv) {
	char * filePath = "";
	  //~按需要修改路径名
	vector<string> files,names;//存储文件的绝对地址和名字的“数组”。 
	  //文件句柄:
	long   hFile = 0;
	  //文件信息:
	struct _finddata_t fileinfo;//_finddata_t是一个用于读取磁盘文件信息的结构。 
	string p;
	if ((hFile = _findfirst(p.assign(filePath).append("\\*.txt").c_str(), &fileinfo)) != -1)
	  //找出目录下所有.txt文件， _findfirst接受一个字符串和 _finddata_t指针，定位到找到文件中的第一个，若没有找到则返回-1 
	{
		do
		{
			names.push_back(fileinfo.name);//将名字存入数组中，带有后缀 
			files.push_back(p.assign(filePath).append("\\").append(fileinfo.name));// 将绝对地址存入数组中 。 
		} while (_findnext(hFile, &fileinfo) == 0);//_findnext（）将定位到下一个文件，若没有下一个则返回0. 
		_findclose(hFile);//关闭 
	}

	FILE * pf, *pf2;
	int docID = 0;
	int size = files.size();

	map<string, int> Dmap;//声明映射 
	fstream docmap;
	docmap.open("newDocID.txt", ios::in);//读取ID-名称文档 
	while (docmap.eof() == false)
	{
		int x;
		string name;
		docmap >> x;
		docmap >> name;
		Dmap[name] = x;
	}//以后输入名字（带有txt）即可得到文件ID，如 Dmap【“hi.txt”】； 

	for (int j = 0;j < size;j++)//对每个文件 
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
		H = InitializeTable(4003);//建立hash表 
		char ch; int i;
		char str[40];//存储每个term 
		ElementType* ptem;//临时元素单元的指针 
		unsigned long hashVal;//存储一个单词的hash值，是从字符串到整数的映射。 
		unsigned int cellNum;//在hash表的第几个条目中。 

		string strName;
		strName = names[j].substr(8);
		
		while (feof(pf) == 0)//未到文件结尾。
		{
			do {
				ch = fgetc(pf);
			} while (ch != EOF && (ch<97 || ch>122));//当不是小写字母时，就一直读（预处理已经将大写变为小写）。

			for (i = 0;i < 40;i++)
			{
				str[i] = 0;
			}

			hashVal = ch - 96;//读到字母，退出do-while，同时给hashVal赋值 。
			str[0] = ch;i = 1;//存储term的字母 
			ch = fgetc(pf);
			while (1)//继续读该单词剩下的字母。 
			{
				if (ch != EOF && ch >= 97 && ch <= 122) {
					str[i] = ch;i++;
					hashVal = (hashVal << 5) + ch - 96;//字母位置加权（权重32）的方法映射到整数。 
					ch = fgetc(pf);
				}
				else {//一个term读完 
					str[i] = '\0';
					break;
				}
			}

			cellNum = Find(hashVal, H);
			if ((H->TheCells[cellNum]).Info == Legitimate) {//若该term已经在hash表中 
				ptem = (H->TheCells)[cellNum].pElement;// 
				(ptem->t1f)++;
			}
			else {//empty或deleted
				ptem = (ElementType*)malloc(sizeof(ElementType));//新条目 

				ptem->docID = Dmap[strName];
				/*ptem->term = (char*)malloc(sizeof(str) + 1);
				strcpy(ptem->term, str);*/
				ptem->t1f = 1;
				Insert(hashVal, ptem, H);
			}
		} //读到文件结尾。 
		  //排序： 

		qsort(H->TheCells, H->TableSize, sizeof(Cell), comp);//将整个hash表排序 
		for (i = 0;i < H->TableSize;i++)
		{
			if (H->TheCells[i].pElement != NULL)
			{
				ptem = H->TheCells[i].pElement;
				//printf("%s  ", ptem->term);
				fprintf(pf2, "%ul  %d  %u\n", H->TheCells[i].key, ptem->docID, ptem->t1f);
			}//将排完序的term-postinglist 输出 
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

