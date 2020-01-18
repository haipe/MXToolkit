#pragma once

namespace mxtoolkit
{

	template<typename ObjType>
	class MXObjContainer
	{
	public:
		typedef ObjType* ObjPtr;
        MXObjContainer( ObjPtr obj = nullptr ) :obj_ptr( obj ){}

		void SetObj( ObjPtr obj )
		{
            obj_ptr = obj;
		}

		inline ObjPtr Obj()
		{
			return obj_ptr;
		}

	protected:
		ObjPtr	obj_ptr;
	};
}