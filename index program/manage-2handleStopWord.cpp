/* 去除文档中的停用词 */ 
#include<io.h>
#include <fstream>
#include<io.h>
#include <fstream>
#include <iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<string>

using namespace std;

int main(int argc, char** argv) {
	char * filePath = "";
	  //~按需要修改路径名
	vector<string> files;//存储文件的绝对地址的“数组”。 
	  //文件句柄:
	long   hFile = 0;
	  //文件信息:
	struct _finddata_t fileinfo;//_finddata_t是一个用于读取磁盘文件信息的结构。 
	string p;
	if ((hFile = _findfirst(p.assign(filePath).append("\\*.txt").c_str(), &fileinfo)) != -1)
	  //读取全部.txt文件， _findfirst接受一个字符串和 _finddata_t指针，定位到找到文件中的第一个，若没有找到则返回-1 
	{
		do
		{
			files.push_back(p.assign(filePath).append("\\").append(fileinfo.name));// 将绝对地址存入数组中 。 
		} while (_findnext(hFile, &fileinfo) == 0);//_findnext（）将定位到下一个文件，若没有下一个则返回0. 
		_findclose(hFile);//关闭 
	}


	fstream fsw;//读入停用词表：
	fsw.open("stop word.txt", ios_base::in);
      if(fsw.is_open()==false)cout<<"fail to open stop word list"<<endl; 
      
	string tem;
	list<string> sw[26];
	while (fsw.eof() == false) {
		fsw >> tem;
		transform(tem.begin(), tem.end(), tem.begin(), ::tolower);/*全部转为小写*/ 
		int i = tem[0] - 97;//根据首字母存入不同的链表 
		sw[i].push_back(tem);
	}
	fsw.close();//~ 

	int size = files.size();//获得待处理文档的数目 
	for (int i = 0;i < size;i++)
	{
	//:去除停用词： 
		string name,old;
		old = files[i].c_str();
	fstream rawFile, newFile;
	rawFile.open(old, ios_base::out | ios_base::in);
	  if (rawFile.is_open() == false)cerr << "fail to create the new txt" << endl;
	name =old.replace(0,sizeof(""), "") ;// 更改新文件的路径与名字 
	newFile.open(name, ios_base::out);
	  if (newFile.is_open() == false)cerr << "fail to create the new txt" << endl;
	  
	list<string>::iterator strli2;//字符串链表迭代器 

	while (rawFile.eof() == false) {//对每一篇文档,读取文档至末尾。 
		string nextWord;
		char ch;
		rawFile.get(ch);
		if ((ch > 64 && ch < 91) || (ch>96 && ch < 123)) {//是字母 
			rawFile.unget();
			rawFile >> nextWord;//获得一个原文档单词。 
			transform(nextWord.begin(), nextWord.end(), nextWord.begin(), ::tolower);//转为小写 
			int i = nextWord[0] - 97;//查看这个单词的首字母。 
			int flag;
			for (strli2 = sw[i].begin();strli2 != sw[i].end();strli2++) {//对每个单词,检验是不是停用词 
				if (nextWord == *(strli2))//是一个停用词
				{
					flag = 0;
					break;
				}
				else {
					flag = 1;
				}
			}
			if (flag == 1) newFile << nextWord;//不是停用词,输出到新文档 
		}
		else {
			newFile << ch;//控制字符\标点等原样输出 
		}
	}
	rawFile.close();
	newFile.close();
	printf("File%d ok", i);
	}
	cout << "ok" << endl;
	system("pause");
	return 0;
}




void deal(string& old)
{

}
