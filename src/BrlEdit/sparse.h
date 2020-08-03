// Sparse array implementation by Glen Gordon
//Copyright C 2007 Freedom Scientific Inc.
// completed by Joseph K Stephen 10 April 2007.
#pragma once
#include <crtdbg.h>

template <typename _tpEntry,int _maxEntries,int _entriesPerBucket,BOOL _bInitUnused=TRUE>
class CSparseArray {
	static_assert(_maxEntries% _entriesPerBucket == 0, "_maxEntries must be a multiple of _entriesPerBucket");

public:
	enum {
		MaxEntries=_maxEntries,
		EntriesPerBucket=_entriesPerBucket,
		NumBuckets=MaxEntries/EntriesPerBucket
	};
	typedef _tpEntry tpEntry;

	// Constructors
	CSparseArray();
	~CSparseArray();
	tpEntry& operator[](int nIdx)
	{
		return EntryAt(nIdx);
	}
	tpEntry& EntryAt(int nIdx);
	BOOL SetAt(int nIdx,tpEntry e);
	void Clear();

 private:
	int BucketNum(int nIdx)
		{
	return nIdx/EntriesPerBucket;
}
	int SlotInBucket(int nIdx)
		{
	return nIdx%EntriesPerBucket;
}

	// member variables
	tpEntry *m_Buckets[NumBuckets];
};
template <typename _tpEntry,int _maxEntries,int _entriesPerBucket,BOOL _bInitUnused>
CSparseArray<_tpEntry,_maxEntries,_entriesPerBucket,_bInitUnused>::CSparseArray()
{
	memset(m_Buckets,0,sizeof(m_Buckets));
}

template <typename _tpEntry,int _maxEntries,int _entriesPerBucket,BOOL _bInitUnused>
inline CSparseArray<_tpEntry,_maxEntries,_entriesPerBucket,_bInitUnused>::~CSparseArray()
{
	Clear();
}
template <typename _tpEntry,int _maxEntries,int _entriesPerBucket,BOOL _bInitUnused>
inline typename CSparseArray<_tpEntry,_maxEntries,_entriesPerBucket,_bInitUnused>::tpEntry& CSparseArray<_tpEntry,_maxEntries,_entriesPerBucket,_bInitUnused>::EntryAt(int nIdx)
{
	_ASSERTE(nIdx < MaxEntries);
	int nBucket = BucketNum(nIdx);
	_ASSERTE(nBucket < NumBuckets);
	int nSlot = SlotInBucket(nIdx);
	_ASSERTE(nSlot < EntriesPerBucket);

	if (m_Buckets[nBucket] == 0)
	{
		m_Buckets[nBucket] = new  tpEntry[EntriesPerBucket];
		if (_bInitUnused)
			memset(m_Buckets[nBucket],0,sizeof(tpEntry)*EntriesPerBucket);
	}
	return m_Buckets[nBucket][nSlot] ;
}
template <typename _tpEntry,int _maxEntries,int _entriesPerBucket,BOOL _bInitUnused>
inline void CSparseArray<_tpEntry,_maxEntries,_entriesPerBucket,_bInitUnused>::Clear()
{
	for (int nIdx = 0;nIdx < NumBuckets;nIdx++)
	{
		if (m_Buckets[nIdx])
		{
			delete m_Buckets[nIdx];
			m_Buckets[nIdx]=0;
		}
	}
}
template <typename _tpEntry,int _maxEntries,int _entriesPerBucket,BOOL _bInitUnused>
inline BOOL CSparseArray<_tpEntry,_maxEntries,_entriesPerBucket,_bInitUnused>::SetAt(int nIdx,tpEntry e)
{
	_ASSERTE(nIdx < MaxEntries);
	int nBucket = BucketNum(nIdx);
	_ASSERTE(nBucket < NumBuckets);
	int nSlot = SlotInBucket(nIdx);
	_ASSERTE(nSlot < EntriesPerBucket);

	if (m_Buckets[nBucket] == 0)
	{
		m_Buckets[nBucket] = new  tpEntry[EntriesPerBucket];
		if (!m_Buckets[nBucket])
			return FALSE;
		if (_bInitUnused)
			memset(m_Buckets[nBucket],0,sizeof(tpEntry)*EntriesPerBucket);
	}
	m_Buckets[nBucket][nSlot]=e;
	return TRUE;
}
