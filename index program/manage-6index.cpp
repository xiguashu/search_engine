/*生成总的index文件*/ 
#include<io.h>
#include <fstream>
#include <iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<string>

using namespace std;

struct oldIndex//单个文件的term-postinglist的条目 
{
	unsigned long oData;//term 
	unsigned short int oDocID;//docID 
	unsigned short int t1f;//频率 
};

list<oldIndex> L1, L2;//链表，L1用于 存储总的list，L2用于存储每个文件新增的list 

bool Comp(const oldIndex& a, const oldIndex& b)//用于merge的比较函数 
{
	return (a.oData < b.oData);
}

int main(int argc, char** argv) 
{
	char * filePath = "";
	  //~按需要修改路径名
	vector<string> files;//存储文件的绝对地址和名字的“数组”。 
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
			files.push_back(p.assign(filePath).append("\\").append(fileinfo.name));// 将绝对地址存入数组中 。 
		} while (_findnext(hFile, &fileinfo) == 0);//_findnext（）将定位到下一个文件，若没有下一个则返回0. 
		_findclose(hFile);//关闭 
	}

	oldIndex tem;//临时存储 

	int size = files.size();//文件的总数
	fstream out,in;

	for (int j = 0;j < size;j++)//从每篇文章的单独的index文件中读取term-posting 
	{
		string s = files[j];
		in.open(s,ios_base::in);if(in.is_open() == false)cout << "fail to open" << endl;
		while (in.eof() == false)
		{
			char help;//c文件读写时在long型后面会加一个l。。。我想哭
			in >> tem.oData;
			//cout << in.fail() << 'x';
			in >> help;
			//cout << in.fail() << 'h';
			in >> tem.oDocID;
			//cout << in.fail() << 'y';
			in>> tem.t1f;
			//cout << in.fail() <<'z'<< endl;
			L2.push_back(tem);//存入链表中 
		}
		in.close();
		
		L1.merge(L2, Comp);//归并排序 
		L2.empty();
		cout << "文件" << j << "已被归并" << endl;

	}

	out.open("Index.txt", ios::out);//生成总的index文件 
	list<oldIndex>::iterator it;//迭代器 
	list<oldIndex>::iterator oriIt, newBegin;
	unsigned int time = 0;
	int num = 0;
	for (it = L1.begin(); /*it != L1.end()*/;)
	{
		bool isEnd = false;
		oriIt = it;//需要将链表中重复的term(不同的docID、term frequence)合并 ,
		time = (*it).t1f;
		while (1)
		{
			it++;
			if (it == L1.end())//读到最后一个了，那么将剩下的全输出 
			{
				it = oriIt;
				//out << endl << '#' << '#' << (*it).oData << '$' << time << ' ' << (*it).oDocID << '@' << (*it).t1f;//为了方便看 
				out << '#' << '#' << (*it).oData << " " << (num + 1) << " " << (*it).oDocID << " " << (*it).t1f;
				for (int i = 0; i < num; i++)
				{
					it++;
					//out << ' ' << (*it).oDocID << '@' << (*it).t1f;
					out << " " << (*it).oDocID << " " << (*it).t1f;
				}
				isEnd = true;
				break;
			}
			if ((*it).oData == (*oriIt).oData)//oriIt存储原始位置 
			{
				num++;
				time += (*it).t1f;
			}
			else//读到了不同的term 
			{
				break;
			}
		}
		if (isEnd == true)break;
		newBegin = it;
		it = oriIt;
		//out << endl << '#' << '#' << (*it).oData << '$' << time << ' ' << (*it).oDocID << '@' << (*it).t1f;
		out << '#' << '#' << (*it).oData << " " << (num + 1) << " " << (*it).oDocID << " " << (*it).t1f;
		  //读到了不同的term ，生成一个新的term-posting list，以 ##号开头，隔以空格,
		  //格式为“termID-出现的文档总数-文档1的ID-文档1中出现的频率-文档2的ID-文档2 中出现的 频率 
		for (int i = 0; i < num; i++)
		{
			it++;
			//out << ' ' << (*it).oDocID << '@' << (*it).t1f;
			out << " " << (*it).oDocID << " " << (*it).t1f;
		}
		num = 0;
		time = 0;
		it = newBegin;//前往下一块 
	}
	cout << endl;
	cout << "ok" << endl;//结束 
	system("pause");
	return 0;
}

