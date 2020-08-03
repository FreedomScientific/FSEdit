#ifndef _MY_BLOCK_ALLOC_HXX_YYY_
#define _MY_BLOCK_ALLOC_HXX_YYY_


//=======================================================================================
class MyMTLock
{
public:
	MyMTLock(){ InitializeCriticalSection(&cs); }
	~MyMTLock(){ DeleteCriticalSection(&cs);     }
	void Lock()    { EnterCriticalSection(&cs);      }
	void Unlock()  { LeaveCriticalSection(&cs);      }

	CRITICAL_SECTION cs;
};

//=======================================================================================
class MySTLock
{
public:
	void Lock()    { }
	void Unlock()  { }
};

//=======================================================================================
template<class Lock>
class MyAutoLock
{
public:
	MyAutoLock(Lock *pLk) : pLK(pLk) { pLK->Lock(); }
	~MyAutoLock() { pLK->Unlock(); }

	Lock *pLK;
};

//=======================================================================================
template<class Lock>
class GenBlockAllocator
{
public:
	GenBlockAllocator(int SizeOfOneElement,int NumOfElements = 64);
	~GenBlockAllocator();

	void *New(size_t Size);
	void  Delete(void *pElem);

protected:
	struct Block
	{
		Block *pNext;
		unsigned long  Align; // Align to 8 byte boundary.
		unsigned char *Data() { return (unsigned char*)(this+1); }
	};
	void NewBlock();

	Block *pFirstBlock;
	Block *pFree;

	int    nUsed;
	int    nElems;
	int    ElemSize;
	Lock   TheLock;
};

//=======================================================================================
#ifdef _MT
# define MY_DEF_MT_LOCK  MyMTLock
#else 
# define MY_DEF_MT_LOCK  MySTLock
#endif

//=======================================================================================
#define IMPLEMENT_FIXED_BL_ALLOC(theClassName,nElements) \
  GenBlockAllocator<MY_DEF_MT_LOCK> theClassName::theBlockAllocator(sizeof(theClassName),nElements);

#define DECLARE_FIXED_BL_ALLOC(theClassName)\
  static GenBlockAllocator<MY_DEF_MT_LOCK>  theBlockAllocator;\
  void* operator new(size_t size)              \
  {                                            \
    return theBlockAllocator.New(size);        \
  }                                            \
  void operator delete(void* p)                \
  {                                            \
    theBlockAllocator.Delete(p);               \
  }

//=======================================================================================
#define IMPLEMENT_FIXED_BL_ALLOC_NOLOCK(theClassName,nElements) \
  GenBlockAllocator<MySTLock> theClassName::theBlockAllocator(sizeof(theClassName),nElements);

#define DECLARE_FIXED_BL_ALLOC_NOLOCK(theClassName)\
  static GenBlockAllocator<MySTLock>  theBlockAllocator;\
  void* operator new(size_t size)              \
  {                                            \
    return theBlockAllocator.New(size);        \
  }                                            \
  void operator delete(void* p)                \
  {                                            \
    theBlockAllocator.Delete(p);               \
  }

//=======================================================================================
template<class Lock>
GenBlockAllocator<Lock>::GenBlockAllocator(int _ElemSize,int _nElems)
		: nElems(_nElems)
		, ElemSize(_ElemSize)
		, pFirstBlock(0)
		, pFree(0)
{
	ElemSize = (ElemSize + 3) & ~3;
	MyAutoLock<Lock> lk(&TheLock);
	NewBlock();
}

//=======================================================================================
template<class Lock>
GenBlockAllocator<Lock>::~GenBlockAllocator()
{
	MyAutoLock<Lock> lk(&TheLock);
	/*
	  if (nUsed)
		dprintf("Memory leaks in Block Allocator\nleak size: %d elements of size %d\n",nUsed,ElemSize);
	 */
	Block *pCur = pFirstBlock;
	Block *pNext;
	while(pCur)
	{
		pNext = pCur->pNext;
		free(pCur);
		pCur  = pNext;
	}
}

//=======================================================================================
template<class Lock>
void GenBlockAllocator<Lock>::NewBlock()
{
	Block *pBlock;
	pBlock       = (Block*) malloc(sizeof(Block) + (nElems * ElemSize));
	if (pBlock)
	{
		pBlock->pNext = pFirstBlock;
		pFirstBlock = pBlock;

		pBlock = pBlock + 1;
		for (int i = 0; i < nElems; i++)
		{
			if (!pBlock)
			{
				break;
			}
			pBlock->pNext = pFree;
			pFree = pBlock;
			pBlock = (Block*)(((char *)pBlock) + ElemSize);
		}
	}
}

//=======================================================================================
template<class Lock>
void *GenBlockAllocator<Lock>::New(size_t /*Size*/)
{
	// If you bailed out here:
	// you probably declared a class AAA with the
	// DECLARE_FIXED_BL_ALLOC macro and derived a BBB from AAA
	// (class BBB : public AAA).
	// That's wrong. The class declared with DECLARE_FIXED_BL_ALLOC must
	// be the topmost class in the hirarchy.

	MyAutoLock<Lock> lk(&TheLock);
	if (!pFree)
		NewBlock();

	Block *pTmp = pFree;
	if (pFree)
	{
		pFree = pFree->pNext;
		nUsed++;
	}
	return pTmp;
}

//=======================================================================================
template<class Lock>
void  GenBlockAllocator<Lock>::Delete(void *pElem)
{
	MyAutoLock<Lock> lk(&TheLock);
	Block *pTmp = (Block*) pElem;

#ifdef _DEBUG
	memset(pElem,0xCC,ElemSize);
	{
		// rpyron 2003-02-20 -- Fill the memory with a unique pattern.
		DWORD *p = (DWORD*)pElem;
		for (DWORD j = 0; j < ElemSize/sizeof(DWORD); ++j)
			*p++ = 0x00;//0xDEADBEEF;
	}
#endif

	pTmp->pNext = pFree;
	pFree       = pTmp;
	nUsed--;
}

#endif
