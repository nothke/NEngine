#pragma once
#include <vector>
#include <map>
#include "Model.h"

struct Node
{
	std::string name;
	Node* parent;
	Model* model;
};

class Scene
{
	std::vector <Node> nodes;
	std::vector <Model> objects;

	Node sceneRoot;

	std::multimap<Node*, Node*> childrenMap;
public:
	Node& Add(const std::string& name, Node* parent = nullptr, Model* model = nullptr)
	{
		Node tempn;
		nodes.push_back(tempn);
		Node& n = nodes[nodes.size() - 1];

		n.name = name;
		n.model = model;

		if (n.parent != nullptr)
		{
			n.parent = parent;
			childrenMap.insert({ n.parent, &n });
		}

		return n;
	}

	Model& AddObject(Model model) // copy
	{
		Node n;
		n.parent = &sceneRoot;

		objects.push_back(model);
		Model& out = objects[objects.size() - 1];
		n.model = &out;
		return out;
	}

	void Remove(Node* node)
	{
		// find all children
		// recursively call till all children are dead
		auto children = childrenMap.equal_range(node);

		for (auto pair = children.first; pair != children.second; ++pair)
		{
			std::cout << pair->first << ": " << pair->second << '\n';
		}
	}

	void LogChildren(Node* node)
	{
		// find all children
		// recursively call till all children are dead
		auto children = childrenMap.equal_range(node);

		for (auto pair = children.first; pair != children.second; ++pair)
		{
			std::cout << pair->first << ": " << pair->second << '\n';
		}
	}

	Scene() {}

	Scene(int objectsCapacity)
	{
		objects.reserve(objectsCapacity);
	}
};