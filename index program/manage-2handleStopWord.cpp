/* ȥ���ĵ��е�ͣ�ô� */ 
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
	  //~����Ҫ�޸�·����
	vector<string> files;//�洢�ļ��ľ��Ե�ַ�ġ����顱�� 
	  //�ļ����:
	long   hFile = 0;
	  //�ļ���Ϣ:
	struct _finddata_t fileinfo;//_finddata_t��һ�����ڶ�ȡ�����ļ���Ϣ�Ľṹ�� 
	string p;
	if ((hFile = _findfirst(p.assign(filePath).append("\\*.txt").c_str(), &fileinfo)) != -1)
	  //��ȡȫ��.txt�ļ��� _findfirst����һ���ַ����� _finddata_tָ�룬��λ���ҵ��ļ��еĵ�һ������û���ҵ��򷵻�-1 
	{
		do
		{
			files.push_back(p.assign(filePath).append("\\").append(fileinfo.name));// �����Ե�ַ���������� �� 
		} while (_findnext(hFile, &fileinfo) == 0);//_findnext��������λ����һ���ļ�����û����һ���򷵻�0. 
		_findclose(hFile);//�ر� 
	}


	fstream fsw;//����ͣ�ôʱ�
	fsw.open("stop word.txt", ios_base::in);
      if(fsw.is_open()==false)cout<<"fail to open stop word list"<<endl; 
      
	string tem;
	list<string> sw[26];
	while (fsw.eof() == false) {
		fsw >> tem;
		transform(tem.begin(), tem.end(), tem.begin(), ::tolower);/*ȫ��תΪСд*/ 
		int i = tem[0] - 97;//��������ĸ���벻ͬ������ 
		sw[i].push_back(tem);
	}
	fsw.close();//~ 

	int size = files.size();//��ô������ĵ�����Ŀ 
	for (int i = 0;i < size;i++)
	{
	//:ȥ��ͣ�ôʣ� 
		string name,old;
		old = files[i].c_str();
	fstream rawFile, newFile;
	rawFile.open(old, ios_base::out | ios_base::in);
	  if (rawFile.is_open() == false)cerr << "fail to create the new txt" << endl;
	name =old.replace(0,sizeof(""), "") ;// �������ļ���·�������� 
	newFile.open(name, ios_base::out);
	  if (newFile.is_open() == false)cerr << "fail to create the new txt" << endl;
	  
	list<string>::iterator strli2;//�ַ������������ 

	while (rawFile.eof() == false) {//��ÿһƪ�ĵ�,��ȡ�ĵ���ĩβ�� 
		string nextWord;
		char ch;
		rawFile.get(ch);
		if ((ch > 64 && ch < 91) || (ch>96 && ch < 123)) {//����ĸ 
			rawFile.unget();
			rawFile >> nextWord;//���һ��ԭ�ĵ����ʡ� 
			transform(nextWord.begin(), nextWord.end(), nextWord.begin(), ::tolower);//תΪСд 
			int i = nextWord[0] - 97;//�鿴������ʵ�����ĸ�� 
			int flag;
			for (strli2 = sw[i].begin();strli2 != sw[i].end();strli2++) {//��ÿ������,�����ǲ���ͣ�ô� 
				if (nextWord == *(strli2))//��һ��ͣ�ô�
				{
					flag = 0;
					break;
				}
				else {
					flag = 1;
				}
			}
			if (flag == 1) newFile << nextWord;//����ͣ�ô�,��������ĵ� 
		}
		else {
			newFile << ch;//�����ַ�\����ԭ����� 
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
