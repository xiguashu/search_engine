/* 平方散列型hash函数的声明    */
#ifndef _HashQuad_H
#define _HashQuad_H

#ifdef __cplusplus             //告诉编译器，这部分代码按C语言的格式进行编译，而不是C++的

extern "C" {

#endif

	enum KindOfEntry { Legitimate, Empty, Deleted };//hash表单元的状态 

	struct hashElem;
	typedef struct hashElem ElementType;

	struct HashEntry;
	typedef struct HashEntry Cell;//条目定义为cell 

	struct HashTbl;
	typedef struct HashTbl *HashTable;

	typedef unsigned long KeyType;

	struct hashElem {
		unsigned int t1f;//在一篇文档中的频率 
		int docID;//文档的编号 
		char* term;//单词字符串 
	};// 在这里设计元素类型。 

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
		Cell *TheCells;//数组。     /* Cell *TheCells will be an array of  HashEntry cells, allocated later*/
	};

	typedef unsigned int Index;
	typedef Index Position;//在hash表中的索引或位置。

	HashTable InitializeTable(Index TableSize);//创建一个hash表并返回。 
	void DestroyTable(HashTable H);
	Position Find(KeyType Key, HashTable H);//输入关键字和hash表，返回在hash表中的索引，元素情况可查看.info 
	void Insert(KeyType Key, ElementType* pElem, HashTable H);//输入hash值和元素指针以及hash表，将元素插入其中。 
	ElementType* Retrieve(Position P, HashTable H);//获取hash表某个位置的元素的指针。 
	HashTable Rehash(HashTable H);//将旧表扩大为2倍以上，返回新表。 


#ifdef __cplusplus

}

#endif

#endif  /* _HashQuad_H */
