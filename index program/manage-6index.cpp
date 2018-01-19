/*�����ܵ�index�ļ�*/ 
#include<io.h>
#include <fstream>
#include <iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<string>

using namespace std;

struct oldIndex//�����ļ���term-postinglist����Ŀ 
{
	unsigned long oData;//term 
	unsigned short int oDocID;//docID 
	unsigned short int t1f;//Ƶ�� 
};

list<oldIndex> L1, L2;//����L1���� �洢�ܵ�list��L2���ڴ洢ÿ���ļ�������list 

bool Comp(const oldIndex& a, const oldIndex& b)//����merge�ıȽϺ��� 
{
	return (a.oData < b.oData);
}

int main(int argc, char** argv) 
{
	char * filePath = "";
	  //~����Ҫ�޸�·����
	vector<string> files;//�洢�ļ��ľ��Ե�ַ�����ֵġ����顱�� 
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
			files.push_back(p.assign(filePath).append("\\").append(fileinfo.name));// �����Ե�ַ���������� �� 
		} while (_findnext(hFile, &fileinfo) == 0);//_findnext��������λ����һ���ļ�����û����һ���򷵻�0. 
		_findclose(hFile);//�ر� 
	}

	oldIndex tem;//��ʱ�洢 

	int size = files.size();//�ļ�������
	fstream out,in;

	for (int j = 0;j < size;j++)//��ÿƪ���µĵ�����index�ļ��ж�ȡterm-posting 
	{
		string s = files[j];
		in.open(s,ios_base::in);if(in.is_open() == false)cout << "fail to open" << endl;
		while (in.eof() == false)
		{
			char help;//c�ļ���дʱ��long�ͺ�����һ��l�����������
			in >> tem.oData;
			//cout << in.fail() << 'x';
			in >> help;
			//cout << in.fail() << 'h';
			in >> tem.oDocID;
			//cout << in.fail() << 'y';
			in>> tem.t1f;
			//cout << in.fail() <<'z'<< endl;
			L2.push_back(tem);//���������� 
		}
		in.close();
		
		L1.merge(L2, Comp);//�鲢���� 
		L2.empty();
		cout << "�ļ�" << j << "�ѱ��鲢" << endl;

	}

	out.open("Index.txt", ios::out);//�����ܵ�index�ļ� 
	list<oldIndex>::iterator it;//������ 
	list<oldIndex>::iterator oriIt, newBegin;
	unsigned int time = 0;
	int num = 0;
	for (it = L1.begin(); /*it != L1.end()*/;)
	{
		bool isEnd = false;
		oriIt = it;//��Ҫ���������ظ���term(��ͬ��docID��term frequence)�ϲ� ,
		time = (*it).t1f;
		while (1)
		{
			it++;
			if (it == L1.end())//�������һ���ˣ���ô��ʣ�µ�ȫ��� 
			{
				it = oriIt;
				//out << endl << '#' << '#' << (*it).oData << '$' << time << ' ' << (*it).oDocID << '@' << (*it).t1f;//Ϊ�˷��㿴 
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
			if ((*it).oData == (*oriIt).oData)//oriIt�洢ԭʼλ�� 
			{
				num++;
				time += (*it).t1f;
			}
			else//�����˲�ͬ��term 
			{
				break;
			}
		}
		if (isEnd == true)break;
		newBegin = it;
		it = oriIt;
		//out << endl << '#' << '#' << (*it).oData << '$' << time << ' ' << (*it).oDocID << '@' << (*it).t1f;
		out << '#' << '#' << (*it).oData << " " << (num + 1) << " " << (*it).oDocID << " " << (*it).t1f;
		  //�����˲�ͬ��term ������һ���µ�term-posting list���� ##�ſ�ͷ�����Կո�,
		  //��ʽΪ��termID-���ֵ��ĵ�����-�ĵ�1��ID-�ĵ�1�г��ֵ�Ƶ��-�ĵ�2��ID-�ĵ�2 �г��ֵ� Ƶ�� 
		for (int i = 0; i < num; i++)
		{
			it++;
			//out << ' ' << (*it).oDocID << '@' << (*it).t1f;
			out << " " << (*it).oDocID << " " << (*it).t1f;
		}
		num = 0;
		time = 0;
		it = newBegin;//ǰ����һ�� 
	}
	cout << endl;
	cout << "ok" << endl;//���� 
	system("pause");
	return 0;
}

