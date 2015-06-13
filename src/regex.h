#ifndef REGEX_H
#define REGEX_H

#include "basic.h"
#include "vector.h"
#include "set.h"
#include "table.h"

struct tag_closure {
	Set set;
	Vector vector;
};
typedef struct tag_closure Closure;

struct Node {
	mint m_label;
	Vector m_edges;
};
typedef struct Node* Node;

struct Edge {
	char m_charRecv[128];
	struct Node* m_next;
};
typedef struct Edge* Edge;

struct StateGraph {
	Node m_Start;
	Node m_End;
	Vector m_endList;
};
typedef struct StateGraph StateGraph;

typedef int(*FSM)[128];

#define REGEX_LABELSTART -5
#define REGEX_LABELEND -3

Edge		Edge_new					(char charRecv[128], Node next);
Edge		Edge_copy					(Edge edge);
bool		Edge_equals					(Edge a, Edge b);
void		Edge_free					(Edge* edge);

Node		Node_new					(Node node, mint label);
void		Node_free					(Node* node);
void		Node_addNewEdge				(Node node, Node next, char ch);
void		Node_addEdgeCharset			(Node node, Node next, char charset[128]);
void		Node_addEdge				(Node node, Edge edge);
void		Node_removeEpsilonEdge		(Node node);
Node		Node_copy					(Node node, Table oldtoNew);
Node		Node_traceTail				(Node node);

StateGraph	StateGraph_new				(char chr);
void		StateGraph_free				(StateGraph sg);
Vector		StateGraph_epsilonClosure	(Node node);
Vector		StateGraph_validNode		(StateGraph sg);
void		StateGraph_foreach			(Node node, bool(*apply)(Node node, void* closure), void* closure);
Vector		StateGraph_getAllNode		(StateGraph sg);
StateGraph	StateGraph_removeEpsilon	(StateGraph sg);
StateGraph	StateGraph_NFAtoDFA			(StateGraph sg);
StateGraph	StateGraph_mark				(StateGraph sg);

StateGraph	StateGraph_optional			(StateGraph sg);
StateGraph	StateGraph_repeat1			(StateGraph sg);
StateGraph	StateGraph_repeat			(StateGraph sg);
StateGraph	StateGraph_parallel			(StateGraph lsg, StateGraph rsg);
StateGraph	StateGraph_connect			(StateGraph lsg, StateGraph rsg);

FSM			StateGraph_getFSM			(StateGraph sg);
void		StateGraph_debugOutput		(StateGraph sg);

#endif
