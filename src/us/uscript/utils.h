////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  UndeadScript Game Scripting Engine                                        //
//  by Kostas "Bad Sector" Michalopoulos of Slashstone                        //
//                                                                            //
//  File name:    vm.h                                                        //
//  Description:  UScript utilities (not intended to be used outside the lib) //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __USCRIPT_UTILS_H_INCLUDED__
#define __USCRIPT_UTILS_H_INCLUDED__

namespace UScriptUtils
{
	template <class T> struct StackItem
	{
		T		item;
		StackItem<T>	*next;
	};
	
	template <class T> struct Stack
	{
		StackItem<T>	*items;
		uint		len;
		
		inline Stack()
		{
			items = NULL;
			len = 0;
		}
		
		inline ~Stack()
		{
			removeAll();
		}
		
		inline void push(T item)
		{
			StackItem<T>	*it = new StackItem<T>;
			it->item = item;
			it->next = items;
			items = it;
			len++;
		}
		
		inline bool hasMore()
		{
			return len != 0;
		}
		
		inline T pop()
		{
			StackItem<T>	*next;
			T		r;
			
			if (!items)
				return (T)NULL;
			next = items->next;
			r = items->it;
			delete items;
			items = next;
			len--;
			return r;
		}
		
		inline void removeAll()
		{
			StackItem<T>	*next;
			while (items)
			{
				next = items->next;
				delete items;
				items = next;
			}
		}
		
		inline int length()
		{
			return len;
		}
	};
};

#endif

