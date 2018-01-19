#include<io.h>
#include <fstream>
#include <iostream>
#include<vector>
#include<string>

using namespace std;


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

	int size = files.size();//得到数组中即目录中文件个数 
	fstream rawFile;
	fstream newFile;
	string old;
	string ne;
	for (int i = 0;i < size;i++)
	{
		old = files[i];
		rawFile.open(old, ios_base::in);
		  if (rawFile.is_open() == false)cout << "fail to open raw file" << endl;
		ne = "";
		ne.append(names[i]);
		newFile.open(ne, ios_base::out);
		  if (newFile.is_open() == false)cout << "fail to open  new file" << endl;
		  /*~打开与新建文件*/ 
		char ch2;
		while (rawFile.eof() == false) {
			rawFile.read(&ch2, sizeof(ch2));/*一个字符一个字符读取*/  
			if (ch2 > 0) newFile.put(ch2);/*如果不是\0，就输出——去除\0*/ 
		}
		rawFile.close();
		newFile.close();/*关闭文件*/  
		cout << "File" << i << "ok" << endl;/*告知这个文件被处理了，程序在正常运行*/ 
	}

	return 0;
}

