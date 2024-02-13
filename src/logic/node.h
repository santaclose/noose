#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "../nodeData.h"

class node
{
private:
#ifdef TEST
public:
#endif
	// pins
	const nodeData* m_nodeData;

	std::vector<void*> m_pinDataPointers;
	std::vector<void*> m_receivedDataPointers;

	// graph stuff
	std::vector<int> m_leftSideConnections;
	std::vector<int> m_rightSideConnections;

public:
	node(const nodeData* data);
	~node();
	
	void connect(int connectionId);
	void disconnect(int connectionId);
	void disconnectAll();

	void activate(); // executes node functionality propagating to nodes connected to the right side
	void run(); // executes node functionality

	int getPinType(int pinIndex);
	int getInputPinCount();
	int getOutputPinCount();
	int getPinCount();
	void setDefaultValue(int pin, const void* defaultValue);

	const std::vector<void*>& getDataPointers();
	const int* getPinTypes();

	void* getDataPointer(int pinIndex, bool acceptReceivedPointers=true);

	bool findNodeToTheRightRecursive(const node* toFind) const;
	inline bool isConnectedToTheLeft() const { return m_leftSideConnections.size() > 0; }

	template<typename T>
	static void removeFromList(const T item, std::vector<T>& list)
	{
		int j = 0;
		for (T c : list)
		{
			if (c == item)
			{
				list.erase(list.begin() + j);
				return;
			}
			j++;
		}
	}
};

