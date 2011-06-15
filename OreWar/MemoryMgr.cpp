#include "MemoryMgr.h"

// ========================================================================
// MemoryRecord Implementation
// ========================================================================
MemoryRecord::MemoryRecord(char * start, char * pageStart, int size)
	: mp_pageStart(start), mp_start(pageStart), m_size(size)
{
}

MemoryRecord::MemoryRecord(const MemoryRecord& copy)
	: mp_pageStart(copy.mp_pageStart), mp_start(copy.mp_start), m_size(copy.m_size)
{
}

char * MemoryRecord::page() const
{
	return mp_pageStart;
}

char * MemoryRecord::startAddress() const
{
	return mp_start;
}

int MemoryRecord::size() const
{
	return m_size;
}



// ========================================================================
// PagedMemoryPool Implementation
// ========================================================================
PagedMemoryPool::PagedMemoryPool(int pageSize, int initialPages)
	: mp_pages(), m_records(), m_nextPage(0), m_pageSize(pageSize)
{
	for(int i = 0; i < initialPages; i++) {
		addPage();
	}
}

void PagedMemoryPool::addPage()
{
	char * newPage = new char[m_pageSize];
	mp_pages.push_back(newPage);
	m_records.push_back(std::vector<MemoryRecord>());
}

void PagedMemoryPool::addMemoryRecord(int pageIndex, const MemoryRecord& record)
{
	int i = 0;
	for(std::vector<MemoryRecord>::iterator recordIter = m_records[pageIndex].begin(); 
			recordIter != m_records[pageIndex].end();
			recordIter++, i++)
	{
		if((*recordIter).startAddress() > record.startAddress()) {
			if(recordIter == m_records[pageIndex].begin()) {
				m_records[pageIndex].insert(m_records[pageIndex].begin(), MemoryRecord(record));
			}

			m_records[pageIndex].insert(recordIter, MemoryRecord(record));
			return;
		}
	}
	m_records[pageIndex].insert(m_records[pageIndex].end(), MemoryRecord(record));
}

int PagedMemoryPool::numPages() const {
	return mp_pages.size();
}

int PagedMemoryPool::currentPage() const {
	return m_nextPage;
}
