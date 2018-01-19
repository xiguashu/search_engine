/* ƽ��ɢ����hash����������    */
#ifndef _HashQuad_H
#define _HashQuad_H

#ifdef __cplusplus             //���߱��������ⲿ�ִ��밴C���Եĸ�ʽ���б��룬������C++��

extern "C" {

#endif

	enum KindOfEntry { Legitimate, Empty, Deleted };//hash��Ԫ��״̬ 

	struct hashElem;
	typedef struct hashElem ElementType;

	struct HashEntry;
	typedef struct HashEntry Cell;//��Ŀ����Ϊcell 

	struct HashTbl;
	typedef struct HashTbl *HashTable;

	typedef unsigned long KeyType;

	struct hashElem {
		unsigned int t1f;//��һƪ�ĵ��е�Ƶ�� 
		int docID;//�ĵ��ı�� 
		char* term;//�����ַ��� 
	};// ���������Ԫ�����͡� 

	struct HashEntry
	{
		KeyType key;
		ElementType*  pElement;
		enum KindOfEntry Info;
	};

	struct HashTbl
	{
		int TableSize;
		int num;
		Cell *TheCells;//���顣     /* Cell *TheCells will be an array of  HashEntry cells, allocated later*/
	};

	typedef unsigned int Index;
	typedef Index Position;//��hash���е�������λ�á�

	HashTable InitializeTable(Index TableSize);//����һ��hash�����ء� 
	void DestroyTable(HashTable H);
	Position Find(KeyType Key, HashTable H);//����ؼ��ֺ�hash��������hash���е�������Ԫ������ɲ鿴.info 
	void Insert(KeyType Key, ElementType* pElem, HashTable H);//����hashֵ��Ԫ��ָ���Լ�hash����Ԫ�ز������С� 
	ElementType* Retrieve(Position P, HashTable H);//��ȡhash��ĳ��λ�õ�Ԫ�ص�ָ�롣 
	HashTable Rehash(HashTable H);//���ɱ�����Ϊ2�����ϣ������±� 


#ifdef __cplusplus

}

#endif

#endif  /* _HashQuad_H */
