#pragma once

#include <string>
#include <vector>

class Node
{
    public:
    Node();
    void setParent(Node *n);
	void setWalkable(bool x);
	void setG(int g);
	void setH(int h);
	void setF(int f);
    void setCol(int col);
    void setLine(int line);

	Node* getParent(std::vector<std::vector<Node>> &graphe);
	bool getWalkable();
	int getG();
	int getH();
	int getF();
    int getCol();
    int getLine();

    private:
    int m_g, m_h, m_f, m_col, m_line, m_parentCol, m_parentLine;
    bool m_walkable;
    Node* m_parent;

};

