#include <iostream>
#include <cstdlib>
#include <string>

#include "Node.h"

Node::Node()
{
    m_walkable = true;
    m_g = m_h = m_f = 0;

}

void Node::setParent(Node* n)
{
    m_parentCol = n->getCol();
    m_parentLine = n->getLine();
}

void Node::setWalkable(bool x)
{
    m_walkable = x;
}

void Node::setG(int g)
{
    m_g = g;
}

void Node::setH(int h)
{
    m_h = h;
}

void Node::setF(int f)
{
    m_f = f;
}

void Node::setCol(int col)
{
    m_col = col;
}

void Node::setLine(int line)
{
    m_line = line;
}

Node* Node::getParent(std::vector<std::vector<Node>> &graphe)
{
    return &(graphe[m_parentCol][m_parentLine]);
}

bool Node::getWalkable()
{
    return m_walkable;
}

int Node::getG()
{
    return m_g;
}

int Node::getH()
{
    return m_h;
}

int Node::getF()
{
    return m_f;
}

int Node::getCol()
{
    return m_col;
}

int Node::getLine()
{
    return m_line;
}


