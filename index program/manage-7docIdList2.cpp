/*����docID����int-document��ӳ��*/ 
#include<io.h>
#include <fstream>
#include <iostream>
#include<vector>
#include<string>
#include<map>

using namespace std;

int main(int argc, char** argv) {
	char * filePath = "";
	  //~����Ҫ�޸�·����
	vector<string> names;//�洢�ļ��������ֵġ����顱�� 
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
		} while (_findnext(hFile, &fileinfo) == 0);//_findnext��������λ����һ���ļ�����û����һ���򷵻�0. 
		_findclose(hFile);//�ر� 
	}
	
	int size = files.size();
	fstream newFile;
		newFile.open("newDocID.txt", ios_base::out);//�½�docID�ļ� 
		if (newFile.is_open() == false)cout << "fail to open  new file" << endl;
	string ne;
	for (int i = 0;i < size;i++)
	{
		newFile<<i<<" "<< names[i]<<endl; //�򵥵�ӳ�� 
	}

	return 0;
}

