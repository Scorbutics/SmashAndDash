#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <math.h>

#include "Path.h"
#include "Node.h"
#include "Utils/SkaConstants.h"

#define DOWN 0
#define RIGHT 1
#define UP 2
#define LEFT 3


void AddToList(std::vector<Node*> *toList, Node* n, std::vector<Node*> *fromList);
void RemoveFromList(std::vector<Node*> *list, Node* n);
float GetDistance(int col, int line, int nodeStartCol, int nodeStartLine);
std::vector<Node*> GetNeighbours(Node *n, std::vector<std::vector<Node>> &graphe);
Node* GetBestNode(std::vector<Node*> *openList);
Node SetDataToNodeAtPos(int col, int line);
bool IsOnList(std::vector<Node*> *list, Node *n);
bool AreNodesEquals(Node *a, Node *b);

ska::Path::Path()
{
    m_pos = 0;
	m_type = P_RANDOM;
}

int ska::Path::getPathDirection(unsigned int number)
{
    if(m_type != P_DEFINED)
        return -1;

    if(number < m_pathDirections.length())
    {
        int buf;
        if(m_pathDirections[number] == 'b')
            buf = 0;
        else if (m_pathDirections[number] == 'd')
            buf = 1;
        else if (m_pathDirections[number] == 'h')
            buf = 2;
        else if(m_pathDirections[number] == 'g')
            buf = 3;
        else
            buf = -1;

        return buf;
    }
    else
        return -1;
}

std::string ska::Path::getPathString()
{
    return m_pathDirections;
}

int ska::Path::getNextPathDirection()
{
    if(m_type != P_DEFINED)
        return -1;

    if(m_pos >= m_pathDirections.length())
        m_pos = 0;

    int buf;
    if(m_pathDirections[m_pos] == 'b')
        buf = 0;
    else if (m_pathDirections[m_pos] == 'd')
        buf = 1;
    else if (m_pathDirections[m_pos] == 'h')
        buf = 2;
    else if(m_pathDirections[m_pos] == 'g')
        buf = 3;
    else
	{
        buf = -1;
		m_type = P_FIXED;
	}

    m_pos++;

    return buf;

}

bool ska::Path::isMotionless()
{
	return (m_type == P_FIXED);
}

bool ska::Path::isRandom()
{
    if(m_type == P_RANDOM)
        return true;
    else
        return false;
}

bool ska::Path::isPredefined()
{
    if(m_type == P_DEFINED)
        return true;
    else
        return false;
}

void ska::Path::setPathString(std::string path)
{
    if(atoi(path.c_str()) == P_FIXED || path.empty())
	{
        m_type = P_FIXED;
		m_pathDirections = "";
	}
    else if(atoi(path.c_str()) == P_RANDOM)
        m_type = P_RANDOM;
    else
    {
        m_type = P_DEFINED;
        m_pathDirections = path;
    }

}



//pour l'explication détaillée de l'A*
//Code inspiré de celui page http://forums.mediabox.fr/wiki/tutoriaux/flashplatform/jeux/pathfinder_algorithme_astar_pratique
//Merci à eux

void ska::Path::findPath(Rectangle from, Rectangle to, const unsigned int blockSize) //Pathfinding A*
{

    Node nodeStart, nodeEnd, *curNode = NULL;

	std::string finalPath;

	nodeStart.setCol(from.x / blockSize);
	nodeStart.setLine(from.y / blockSize);

	nodeEnd.setCol(to.x / blockSize);
	nodeEnd.setLine(to.y / blockSize);

	std::vector<Node*> openList, closedList;
	std::vector<std::vector<Node>> graphe;

    createGraphe(&graphe);

    AddToList(&openList, &nodeStart, &closedList);

    while(!openList.empty())
    {
        curNode = GetBestNode(&openList);

        if(AreNodesEquals(curNode, &nodeEnd) ) //Si on arrive à la fin de la liste ouverte, on a trouvé
          break;

        AddToList(&closedList, curNode, &openList);

		std::vector<Node*> neighbours = GetNeighbours(curNode, graphe);

        for(unsigned int i = 0; i < neighbours.size(); i++)
        {
            //Node curNeighbour = *(neighbours[i]);

              //Si le node est un obstacle ou est dans la liste fermée ignorez-le et passer à l'analyse d'un autre node.
              if( IsOnList(&closedList, (neighbours[i])) || neighbours[i]->getWalkable() == false )
                continue;

              int newG = neighbours[i]->getParent(graphe)->getG() + 1;
              int newH = abs(neighbours[i]->getCol() - nodeEnd.getCol()) + abs(neighbours[i]->getLine() - nodeEnd.getLine());
              int newF = newH + newG;

               if( IsOnList(&openList, (neighbours[i]) ))
                  {
                    //Si le node est déjà dans la liste ouverte, recalculez son G, s'il est inférieur à l'ancien,
                    //faites de CURRENT  son parent(P) et recalculez et enregistrez ses propriétés F et H.

                    if(newG < neighbours[i]->getG())
                    {
                      neighbours[i]->setParent(curNode);
                      neighbours[i]->setG(newG);
                      neighbours[i]->setH(newH);
                      neighbours[i]->setF(newF);
                    }

                  }
                  else
                  {
                    //Si le node n'est pas dans la liste ouverte, ajoutez-le à la dite liste et faites de CURRENT son parent(P).
                    //Calculez et enregistrez ses propriétés F, G et H.
                      AddToList(&openList, (neighbours[i]), &closedList);
                      neighbours[i]->setParent(curNode);
                      neighbours[i]->setG(newG);
                      neighbours[i]->setH(newH);
                      neighbours[i]->setF(newF);
                  }

                    //if(newH == 0)
                      //  graphe[nodeEnd.getCol()][nodeEnd.getLine()].setParent(curNode);
                }



      }


      // on est sorti de la liste, on a deux solutions, soit la liste ouverte est vide dans ces cas là il
      // n'y a pas de solution et on retoure directement finalPath;

      if( openList.empty())
        return;


      Node *lastNode = &(graphe[nodeEnd.getCol()][nodeEnd.getLine()]);
      while(!AreNodesEquals(lastNode, &nodeStart))
      {
            //int col = lastNode->getCol(), line = lastNode->getLine();
            //int lastCol = lastNode->getParent(graphe)->getCol(), lastLine = lastNode->getParent(graphe)->getLine();

            if(lastNode->getCol() - lastNode->getParent(graphe)->getCol() == 1)
            {
                finalPath += "d";
            }
            else if(lastNode->getCol() - lastNode->getParent(graphe)->getCol() == -1)
            {
                finalPath += "g";
            }
            else if(lastNode->getLine() - lastNode->getParent(graphe)->getLine() == 1)
            {
                finalPath += "b";
            }
            else if(lastNode->getLine() - lastNode->getParent(graphe)->getLine() == -1)
            {
                finalPath += "h";
            }

            lastNode = lastNode->getParent(graphe);
      }

	  //m_pathDirections = finalPath;

		for(unsigned int i = 0; i < finalPath.size(); i++)
			m_pathDirections += finalPath[finalPath.size()-1-i];

		m_type = P_DEFINED;

}


bool AreNodesEquals(Node *a, Node *b)
{
    if(a->getCol() == b->getCol() && a->getLine() == b->getLine())
        return true;
    else
        return false;
}


std::vector<Node*> GetNeighbours(Node *, std::vector<std::vector<Node>> &) {
    //int currentPos[4];

	std::vector<Node*> neighbours;

    /*currentPos[DOWN] = n->getLine() + 1;
    currentPos[RIGHT] = n->getCol() + 1;
    currentPos[UP] = n->getLine() - 1;
    currentPos[LEFT] = n->getCol() - 1;*/

	/*
	if(currentPos[UP] >= 0)
        neighbours.push_back(&(graphe[n->getCol()][currentPos[UP]]));

    if(currentPos[DOWN] < w.getNbrBlocY())
        neighbours.push_back(&(graphe[n->getCol()][currentPos[DOWN]]));

    if(currentPos[RIGHT] < w.getNbrBlocX())
        neighbours.push_back(&(graphe[currentPos[RIGHT]][n->getLine()]));

    if(currentPos[LEFT] >= 0)
        neighbours.push_back(&(graphe[currentPos[LEFT]][n->getLine()]));
		*/
    return neighbours;

}

Node SetDataToNodeAtPos(int col, int line)
{
    Node nodeBuf;
    nodeBuf.setCol(col);
    nodeBuf.setLine(line);
    /*ska::Rectangle buf = {static_cast<Sint16>(nodeBuf.getCol()*TAILLEBLOC), static_cast<Sint16>(nodeBuf.getLine()*TAILLEBLOC), 1, 1};*/
    nodeBuf.setWalkable(/*w.canMoveToPos(buf, wScreen.getHero() )*/ true);
    nodeBuf.setParent(&nodeBuf);
    return nodeBuf;
}

float GetDistance(int col, int line, int nodeStartCol, int nodeStartLine)
{
    return sqrt((float)(col - nodeStartCol) * (col - nodeStartCol) + (line - nodeStartLine) * (line - nodeStartLine));
}


void AddToList(std::vector<Node*> *toList, Node* n, std::vector<Node*> *fromList)
 {
     RemoveFromList(fromList, n);
     toList->push_back(n);
 }

void RemoveFromList(std::vector<Node*> *list, Node* n)
{
	std::vector<Node*> tmp;

    for(unsigned int i = 0; i < list->size(); i++)
        if(!AreNodesEquals((*list)[i], n))
            tmp.push_back((*list)[i]);

    *list = tmp;
}

void ska::Path::createGraphe(std::vector<std::vector<Node>> *)
{
    /*for(int i = 0; i < w.getNbrBlocX(); i++)
    {
        vector<Node> tmp;
        for(int j = 0; j < w.getNbrBlocY(); j++)
        {
            Node n;
            n = SetDataToNodeAtPos(i, j);
            tmp.push_back(n);
        }

        graphe->push_back(tmp);
    }*/

}


//Cherche le node de la liste ouverte ayant le min de f
Node* GetBestNode(std::vector<Node*> *openList)
{
  int minF = 1000000; //valeur arbitraire très élevée
  Node* bestNode = NULL;

  for(unsigned int i = 0; i < openList->size(); i++ )
  {
    if( (*openList)[i]->getF() < minF )
    {
      minF = (*openList)[i]->getF();
      bestNode = (*openList)[i];
    }
  }

  return bestNode;
}

bool IsOnList(std::vector<Node*> *list, Node *n)
{

  for(unsigned int i = 0; i < list->size(); i++)
  {
    if(AreNodesEquals((*list)[i], n))
      return true;
  }

  return false;
}


