
#ifndef NODE_H
#define NODE_H

template<typename T>
class Node
{
private:
	T item;
	Node<T>* next;
public:

	Node()
	{
		next = nullptr;
	}

	Node(T newItem)
	{
		item = newItem;
		next = nullptr;

	}

	void setItem(T newItem)
	{
		item = newItem;
	}

	void setNext(Node<T>* nextNodePtr)
	{
		next = nextNodePtr;
	}

	T getItem() const
	{
		return item;
	}

	Node<T>* getNext() const
	{
		return next;
	}
	virtual ~Node() {}
};


template < typename T>
class priNode
{
private:
	T item;
	double pri;
	priNode<T>* next;
public:
	priNode(const T& r_Item, double PRI)
	{
		setItem(r_Item, PRI);
		next = nullptr;
	}
	void setItem(const T& r_Item, double PRI)
	{
		item = r_Item;
		pri = PRI;
	}
	void setNext(priNode<T>* nextNodePtr)
	{
		next = nextNodePtr;
	}

	T getItem(double& PRI) const
	{
		PRI = pri;
		return item;
	}

	priNode<T>* getNext() const
	{
		return next;
	}

	double getPri() const
	{
		return pri;
	}
};



#endif