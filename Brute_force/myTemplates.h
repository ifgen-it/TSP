#pragma once

template <typename T> void Swap(T& x, T& y)
{
	T temp = move(x);
	x = move(y);
	y = move(temp);
}

template <typename T, size_t cap> class MyStack
{
private:
	T m_ar[cap];
	size_t m_size;

public:
	struct StackError
	{
		size_t Index;
		size_t Size;
		StackError(size_t i, size_t s) :Index(i), Size(s) {}
	};

	MyStack() : m_size(0) {}

	~MyStack() = default;

	void push(const T& other) //throw (const char*)	 Warn: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
	{
		if (m_size < cap)
		{
			m_ar[m_size] = other;
			m_size++;
		}
		else
		{
			throw "Stack is full";
		}
	}

	T& operator[](size_t n)// throw(size_t)
	{
		if (n >= 0 && n < m_size)
		{
			return m_ar[n];
		}
		else
		{
			throw StackError(n, m_size);
		}
	}

	void pop()
	{
		if (m_size)
		{
			m_size--;
		}
	}

	size_t size() { return m_size; }
};

template <typename T> class MyStack2
{

	template <typename T> class Node
	{
		friend class MyStack2<T>;
		T m_data;
		Node<T>* pNext;

		Node(Node<T>* n, const T& other) : m_data(other)
		{
			pNext = n;
		}

		~Node() = default;
	};

	size_t m_size;
	Node<T>* pHead;

public:
	struct StackErrMsg
	{
		const char* Err;
		StackErrMsg(const char* msg) :Err(msg){}
	};
	MyStack2()
	{
		pHead = nullptr;
		m_size = 0;
	}

	~MyStack2()
	{
		while (pHead)
		{
			Node<T>* pDel = pHead;
			pHead = pHead->pNext;
			delete pDel;
		}
	}

	void push(const T& other)
	{
		pHead = new Node<T>(pHead, other);
		m_size++;
	}

	T& top() 
	{
		if (pHead)
		{
			return pHead->m_data;
		}
		else
		{
			throw StackErrMsg("Stack is empty. There is no top");
		}
	}

	void pop()
	{
		if (pHead)
		{
			Node<T>* pDel = pHead;
			pHead = pHead->pNext;
			delete pDel;
			m_size--;
		}
		else
		{
			throw StackErrMsg("Stack is empty. You tried to pop nothing");
		}
	}

	void PrintBackward()
	{
		Node<T>* pTemp = pHead;
		while (pTemp)
		{
			cout << pTemp->m_data << endl;
			pTemp = pTemp->pNext;
		}
	}

	void PrintForward2() { PrintWithResursion(pHead); }

	void PrintWithResursion(Node<T>* pCurrent)
	{
		if (!pCurrent) return;
		PrintWithResursion(pCurrent->pNext);
		cout << pCurrent->m_data << endl;
	}

	void PrintForward1()
	{
		Invert();
		PrintBackward();
		Invert();
	}

	void Invert()
	{
		Node<T>* pFirst = pHead;
		Node<T>* pMiddle = pHead;
		Node<T>* pLast = nullptr;
		while (pFirst)
		{
			pFirst = pFirst->pNext;
			pMiddle->pNext = pLast;
			pLast = pMiddle;
			pMiddle = pFirst;
		}
		// pFirst = 0, pMiddle = 0, pLast = the pointer on the Last Node in the list
		pHead = pLast;
	}
};

template <typename T> class MyQueue
{
	T* m_p;
	size_t m_cap;
	size_t m_n;
	size_t m_first;
	size_t m_last;

public:
	MyQueue(size_t c) :m_cap(c), m_n(0), m_first(0), m_last(0)
	{

		m_p = new T[m_cap];
		for (size_t i = 0; i < m_cap; i++)
		{
			m_p[i] = T();
		}
	}

	MyQueue(const MyQueue& other) 	// надо копировать только где есть инфа
	{
		m_cap = other.m_n;
		m_n = other.m_n;
		m_first = 0;
		m_last = 0;

		m_p = new T[m_cap];
		for (size_t i = 0; i < m_cap; i++)
		{
			m_p[i] = other.m_p[(other.m_first + i) % other.m_cap];
		}
	}

	MyQueue& operator=(const MyQueue& other)
	{


		if (m_cap < other.m_n)
		{
			delete[] m_p;
			m_cap = other.m_n;
			m_p = new T[m_cap];
		}
		
			//m_cap the same 
			m_n = other.m_n;
			m_first = 0;
			m_last = m_n % m_cap;
		
			// m_p the same
			for (size_t i = 0; i < m_n; i++)
			{
				m_p[i] = other.m_p[(other.m_first + i) % other.m_cap];
			}
		

		return *this;
	}

	MyQueue& operator=(MyQueue&& temp)
	{
		delete[] m_p;

		m_cap = temp.m_cap;
		m_n = temp.m_n;
		m_first = temp.m_first;
		m_last = temp.m_last;
		m_p = temp.m_p;

		temp.m_p = nullptr;
		temp.m_cap = temp.m_n = temp.m_first = temp.m_last = 0;

		return *this;
	}

	~MyQueue()
	{
		delete[] m_p;
	}

	T operator[](size_t n)	// +  надо индекс юзера а не реального массива
	{
		return m_p[(m_first + n) % m_cap];
	}

	void Enqueue(const T& other)
	{
		if (m_n >= m_cap)
		{
			//throw "Queue if full. Nowhere to enqueue";
			int Delta = 1; // m_cap/2;
			Resize(Delta);
		}

		m_p[m_last] = other;
		m_last = (m_last + 1) % m_cap;
		m_n++;
	}

	T Dequeue()	// + надо по значению
	{
		if (m_n)
		{
			size_t ret = m_first;
			m_first = (m_first + 1) % m_cap;
			m_n--;
			return m_p[ret];
		}
		else
		{
			throw "Queue is empty. Nothing to dequeue";
		}
	}

	size_t Size() { return m_n; }
	size_t Capacity() { return m_cap; }

	void Resize(int Delta)
	{
		if (Delta > 0)
		{
			size_t m_capTemp = m_cap + Delta;
			T* m_pTemp = new T[m_capTemp];

			//default value in Temp array
			for (size_t i = 0; i < m_capTemp; i++)
			{
				m_pTemp[i] = T();
			}

			// copying Queue to Temp
			for (size_t i = 0; i < m_n; i++)
			{
				m_pTemp[i] = m_p[(m_first + i) % m_cap];
			}

			//swap
			delete[] m_p;
			m_p = m_pTemp;
			m_cap = m_capTemp;
			m_first = 0;
			m_last = m_n;
		}
		else
		{
			throw "Incorrect value to resize Queue. Need positive integer number";
		}
	}

};


// Template of Stack based on Dynamic array
/*template <typename T, size_t n> class MyStack
{
private:
	T* m_p;
	size_t m_size;
	size_t m_cap;

public:
	struct StackError
	{
		size_t Index;
		size_t Size;
		StackError(size_t i, size_t s) :Index(i), Size(s) {}
	};

	MyStack() :m_cap(n), m_size(0)
	{
		m_p = new T[m_cap];
	}

	~MyStack()
	{
		//for (size_t i = 0; i < m_size; i++)
		//{
		//	m_p[i].~T();
		//}
		delete[] m_p;
	}

	void push(const T& other)
	{
		if (m_cap == m_size)
		{
			m_cap++;
			T* pTemp = new T[m_cap];
			for (size_t i = 0; i < m_size; i++)
			{
				pTemp[i] = m_p[i];
			}
			
			delete[] m_p;
			m_p = pTemp;
		}
		
		//new(&m_p[m_size])T(other);	
		 m_p[m_size] = T(other);	// делает так: сначал конструктор, потом оператор=
		m_size++;
	}

	T& operator[](size_t n) throw(size_t)
	{
		if (n >= 0 && n < m_size)
		{
			return m_p[n];
		}
		else
		{
			throw StackError(n, m_size);
		}
	}

	void pop()
	{
		if (m_size)
		{
			m_size--;
			m_p[m_size].~T();
		}
	}

	size_t size() { return m_size; }
};*/

