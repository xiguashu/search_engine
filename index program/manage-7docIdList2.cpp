/*生成docID，即int-document的映射*/ 
#include<io.h>
#include <fstream>
#include <iostream>
#include<vector>
#include<string>
#include<map>

using namespace std;

int main(int argc, char** argv) {
	char * filePath = "";
	  //~按需要修改路径名
	vector<string> names;//存储文件的名字字的“数组”。 
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
		} while (_findnext(hFile, &fileinfo) == 0);//_findnext（）将定位到下一个文件，若没有下一个则返回0. 
		_findclose(hFile);//关闭 
	}
	
	int size = files.size();
	fstream newFile;
		newFile.open("newDocID.txt", ios_base::out);//新建docID文件 
		if (newFile.is_open() == false)cout << "fail to open  new file" << endl;
	string ne;
	for (int i = 0;i < size;i++)
	{
		newFile<<i<<" "<< names[i]<<endl; //简单的映射 
	}

	return 0;
}

