#include<io.h>
#include <fstream>
#include <iostream>
#include<vector>
#include<string>

using namespace std;


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

	int size = files.size();//�õ������м�Ŀ¼���ļ����� 
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
		  /*~�����½��ļ�*/ 
		char ch2;
		while (rawFile.eof() == false) {
			rawFile.read(&ch2, sizeof(ch2));/*һ���ַ�һ���ַ���ȡ*/  
			if (ch2 > 0) newFile.put(ch2);/*�������\0�����������ȥ��\0*/ 
		}
		rawFile.close();
		newFile.close();/*�ر��ļ�*/  
		cout << "File" << i << "ok" << endl;/*��֪����ļ��������ˣ���������������*/ 
	}

	return 0;
}

