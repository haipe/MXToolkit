#pragma once

#include <list>
#include <mutex>

#include "mxkit.h"

_BEGIN_MX_KIT_NAME_SPACE_



#define INSERT_TAIL

template <class T>
class ElementAllocator
{
    class AutoLock
    {
    public:
        AutoLock(ElementAllocator* ea)
            : m_auto(ea->m_mutex) {}
    private:
        std::lock_guard<std::recursive_mutex> m_auto;
    };

    struct it
    {
        T		element;
        it* nextElement;
    };
    typedef std::list<it*> ElementList;
public:
    ElementAllocator(uint32 increCount = 100) :
        m_increCount(increCount),
        m_totalCount(0),
        m_headIt(NULL),
        m_tailIt(NULL)
    {
    };
    virtual ~ElementAllocator()
    {
        Clear();
    };
public:
    int32		GetTotalAllocCount() { return m_totalCount; };

    T* Alloc()
    {
        T* pElement = NULL;
        AutoLock aut_lock(this);

        if (m_headIt)
        {
            pElement = &(m_headIt->element);
            m_headIt = m_headIt->nextElement;
        }
        else
        {
            if (BatchAlloc(m_increCount))
            {
                pElement = &(m_headIt->element);
                m_headIt = m_headIt->nextElement;
            }
        }
        m_tailIt = m_headIt ? m_tailIt : NULL;

        return pElement;
    };
    VOID	Free(T* pElement)
    {
        it* i = (it*)pElement;

        AutoLock aut_lock(this);
#ifdef INSERT_TAIL
        i->nextElement = NULL;
        if (m_headIt)
        {
            m_tailIt->nextElement = i;
            m_tailIt = i;
        }
        else
        {
            m_headIt = m_tailIt = i;
        }
#else
        i->nextElement = m_headIt;
        m_headIt = i;
        if (NULL == m_tailIt)
            m_tailIt = m_headIt;
#endif
    };
    VOID	Clear()
    {
        while (m_totalElement.size() > 0)
        {
            it* pElement = m_totalElement.front();
            delete[] pElement;
            m_totalElement.pop_front();
        }

        m_headIt = NULL;
        m_tailIt = NULL;
    };
protected:
    BOOL	BatchAlloc(uint32 dwCount)
    {
        it* pElement = new  it[dwCount];
        if (NULL == pElement)
            return FALSE;


        m_tailIt = m_tailIt ? m_tailIt : pElement;
        for (uint32 dwIndex = 0; dwIndex < dwCount; dwIndex++)
        {
            pElement[dwIndex].nextElement = m_headIt;
            m_headIt = pElement + dwIndex;
        }
        m_totalElement.push_back(pElement);
        m_totalCount += dwCount;

        //uint32 dwTotalSize = sizeof(T)*m_lTotalCount;

        return TRUE;
    };

    ElementList		        m_totalElement;
    uint32			        m_increCount;
    volatile int32	        m_totalCount;

    std::recursive_mutex	m_mutex;
    it* m_headIt;
    it* m_tailIt;
};

_END_MX_KIT_NAME_SPACE_