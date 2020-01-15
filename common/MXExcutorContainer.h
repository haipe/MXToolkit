#pragma once

#include <atomic>
#include <mutex>
#include <list>
#include <algorithm>

namespace mxtoolkit
{
    template<typename ExcutorInterface, typename ExcutorReturn = bool>
    class MXExcutorContainer
    {
    public:

        struct ExcutorObj
        {
            ExcutorObj(ExcutorInterface* ex)
                : excutor(ex)
                , effective(ex != nullptr) {}

            //std::atomic<bool>	effective;	//有效
            bool				effective;
            ExcutorReturn		ret;		//执行返回值
            ExcutorInterface*	excutor;	//执行接口

            bool operator == (const ExcutorObj& rhs) const
            {
                return excutor == rhs.excutor;
            }
        };
        typedef std::list<ExcutorObj>	ExcutorObjList;

    public:
        void Add(ExcutorInterface* ex)
        {
            if (ex == nullptr)
                return;

            AutoLock auto_lock(this);

            ExcutorObj obj(ex);
            auto find_it = std::find(m_container.begin(), m_container.end(), obj);
            if (find_it == m_container.end())
            {
                m_container.push_back(obj);
            }
            else
            {
                find_it->effective = true;
            }
        }

        void Remove(ExcutorInterface* ex)
        {
            if (ex == nullptr)
                return;

            AutoLock auto_lock(this);

            ExcutorObj obj(ex);
            auto find_it = std::find(m_container.begin(), m_container.end(), obj);
            if (find_it != m_container.end())
                find_it->effective = false;	//只是设置为无效，避免在使用的时候删除，出错
        }

        void RemoveAll()
        {
            AutoLock auto_lock(this);
            m_container.clear();
        }

        void Modify(ExcutorInterface* ex, const ExcutorReturn& rt)
        {
            AutoLock auto_lock(this);

            ExcutorObj obj(ex);
            auto i = std::find(m_container.begin(), m_container.end(), obj);
            if (i != m_container.end())
                i->ret = rt;
        }

        void ClearNotEffective()
        {
            AutoLock auto_lock(this);

            auto rmove_index = std::remove_if(m_container.begin(), m_container.end(), [](ExcutorObj& obj)
            {
                return !obj.effective || obj.excutor == nullptr;//删除无效的
            });

            if (rmove_index != m_container.end())
                m_container.erase(rmove_index, m_container.end());
        }

        ExcutorObjList& GetContainer()
        {
            return m_container;
        }

        void ForEach(std::function<void(const ExcutorObj&)> func)
        {
            if (!func)
                return;

            AutoLock auto_lock(m_mutex);

            for (auto& item : m_container)
            {
                if (!m_enable)
                    break;

                if (item.effective)
                    func(item);
            }
        }

        void SetEnable(bool enable) { m_enable = enable; }
        bool IsEnable() { return m_enable; }

        class AutoLock
        {
        public:
            AutoLock(MXExcutorContainer* ec)
                : m_auto(ec->m_mutex) {}
        private:
            std::lock_guard<std::recursive_mutex> m_auto;
        };

    private:
        std::atomic<bool>		m_enable = { true };
        std::recursive_mutex	m_mutex;
        ExcutorObjList			m_container;
    };


#define MX_EXCUTE_ONLY(Objects,Interface,...)					\
	do{															\
        if(!Objects.IsEnable()) break;							\
		Objects.ClearNotEffective();							\
		auto auto_lock(&Objects);								\
		for (auto& i : Objects.GetContainer())					\
		{														\
			if(i.effective)										\
				i.excutor->Interface(__VA_ARGS__);				\
		}														\
	} while (0);												\


#define MX_EXCUTE_AND_SAVE_RETURN(Objects,Interface,...)		\
	do{															\
        if(!Objects.IsEnable()) break;							\
		Objects.ClearNotEffective();							\
		auto lock(&Objects);									\
		for (auto& i : Objects.GetContainer())					\
		{														\
			if(i.effective)										\
				i.ret = i.excutor->Interface(__VA_ARGS__);		\
		}														\
	} while (0);												\

#define MX_CHECK_EXCUTE_SUCCESS_COUNT(Count,Objects,Interface)	\
	do{															\
		Count = 0;												\
        if(!Objects.IsEnable()) break;							\
		Objects.ClearNotEffective();							\
		auto lock( &Objects );									\
		for ( auto& i : Objects.GetContainer() )				\
		{														\
			if(i.effective)										\
				if(i.excutor->Interface())						\
					Count ++;									\
		}														\
	} while ( 0 );												\

#define MX_CHECK_EXCUTE_RETURN_COUNT(Count,Objects,Ret)			\
	do{															\
		Count = 0;												\
        if(!Objects.IsEnable()) break;							\
		Objects.ClearNotEffective();							\
		auto lock( &Objects );									\
		for ( auto& i : Objects.GetContainer() )				\
		{														\
			if(i.effective)										\
				if(i.ret == Ret)								\
					Count ++;									\
		}														\
	} while ( 0 );												\

#define MX_EXCUTE_AND_CHECH_TURE(Ret,Objects,Interface,...)		\
	do{															\
        if(!Objects.IsEnable()) break;							\
		Objects.ClearNotEffective();							\
		auto lock(&Objects);									\
		for (auto& i : Objects.GetContainer())					\
		{														\
			if(i.effective)										\
				if (!i.excutor->Interface(__VA_ARGS__))			\
					Ret = false;								\
		}														\
	} while (0);												\



#define MX_DECLARE_NOTIFY_CONTAINER_INTERFACE(Interface)		\
	virtual void AddNotify(Interface* it) = 0;					\
	virtual void RemoveNotify(Interface* it) = 0;				\
	virtual void RemoveAllNotify() = 0;							\


#define MX_DECLARE_NOTIFY_CONTAINER_INTERFACE_EX(Interface,AddNotify,RemoveNotify,RemoveAllNotify) \
	virtual void AddNotify(Interface* it) = 0;					\
	virtual void RemoveNotify( Interface* it ) = 0;				\
	virtual void RemoveAllNotify() = 0;							\


#define MX_IMP_NOTIFY_CONTAINER_INTERFACE(Interface)			\
	public:														\
	virtual void AddNotify(Interface* it)						\
	{															\
		m_notifys.Add(it);										\
	}															\
	virtual void RemoveNotify(Interface* it)					\
	{															\
		m_notifys.Remove(it);									\
	}															\
	virtual void RemoveAllNotify()								\
	{															\
		m_notifys.RemoveAll();									\
	}															\
	protected:													\
	mxtoolkit::MXExcutorContainer<Interface> m_notifys;			\


#define MX_IMP_NOTIFY_CONTAINER_INTERFACE_EX( Interface, memberName, AddNotify, RemoveNotify, RemoveAllNotify )  \
	public:														\
	virtual void AddNotify(Interface* it)						\
	{															\
		memberName.Add(it);									    \
	}															\
	virtual void RemoveNotify(Interface* it)					\
	{															\
		memberName.Remove(it);								    \
	}															\
	virtual void RemoveAllNotify()								\
	{															\
		memberName.RemoveAll();								    \
	}															\
	protected:													\
	mxtoolkit::MXExcutorContainer<Interface> memberName;		\


#define MX_NOTIFY_TO(Interface,...)             MX_EXCUTE_ONLY(m_notifys,Interface,__VA_ARGS__);
#define MX_NOTIFY_TO_EX(notifys,Interface,...)  MX_EXCUTE_ONLY(notifys,Interface,__VA_ARGS__);

    template<typename ExcutorInterface>
    class MXNotifyContainer
    {
    public:
        virtual ~MXNotifyContainer() {}

        void SetEnable(bool enable) { m_enable = enable; }
        bool IsEnable() { return m_enable; }

        bool m_enable = true;

        MX_IMP_NOTIFY_CONTAINER_INTERFACE(ExcutorInterface);
    };



}