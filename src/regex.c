#include "regex.h"
/************************************************************************/
/* ������ʽ״̬����ʵ��                                                 */
/************************************************************************/

static char empty[128]; //�ձ�

static bool StateGraph_getValidNode(Node node, Closure* closure);
static int nodeSetCompare(const void* a, const void* b);
static unsigned int nodeSetHash(const void* a1);

Edge Edge_new(char charRecv[128], Node next)
{ //���캯��
	Edge edge;
	NEW(edge);

	memcpy(edge->m_charRecv, charRecv, sizeof(edge->m_charRecv));
	edge->m_next = next;

	return edge;
}

Edge Edge_copy(Edge edge)
{ //���ƹ��캯��
	Edge e;
	NEW(e);
	memcpy(e->m_charRecv, edge->m_charRecv, sizeof(e->m_charRecv));
	e->m_next = edge->m_next;
	return e;
}

bool Edge_equals(Edge a, Edge b)
{ //operator==����
	return memcmp(a->m_charRecv, b->m_charRecv, sizeof(a->m_charRecv)) == 0
		&& a->m_next == b->m_next;
}

void Edge_free(Edge* edge)
{ //��������
	free(*edge);
	*edge = NULL;
}

Node Node_new(Node node, mint label)
{ //���캯��
	Node n;
	NEW(n);
	assert(n);
	if (node)
	{
		n->m_edges = Vector_copy(node->m_edges);
		n->m_label = node->m_label;
	}
	else
	{
		n->m_edges = Vector_new(0);
		n->m_label = label;
	}
	return n;
}

void Node_free(Node* node)
{ //��������
	Vector_free(&((*node)->m_edges));
	free(*node);
	*node = NULL;
}
void Node_addNewEdge(Node node, Node next, char ch)
{ //��Ա��������һ����
	Vector_foreach(Edge, edge, node->m_edges)
	{
		if (edge->m_next == next)
		{
			edge->m_charRecv[ch] = 1;
			return;
		}
	}
	Vector_loopend;
	{
		char charset[128] = {0};
		charset[ch] = 1;
		Vector_push_back(node->m_edges, Edge_new(charset, next));
	}
}
void Node_addEdgeCharset(Node node, Node next, char charset[128])
{ //��һ���ߵ����أ�C����ֻ������һ������
	Vector_push_back(node->m_edges, Edge_new(charset, next));
}
void Node_addEdge(Node node, Edge edge)
{ //�ӱߵ���һ������
	Vector_foreach(Edge, e, node->m_edges)
	{
		if (Edge_equals(e, edge))
		{
			break;
		}
	}
	if (_i == Vector_getLength(node->m_edges))
	{
		Vector_push_back(node->m_edges, edge);
	}
	Vector_loopend;
}

void Node_removeEpsilonEdge(Node node)
{ //�Ƴ����пձ�
	Vector_foreach(Edge, edge, node->m_edges)
	{
		if (memcmp(edge->m_charRecv, empty, sizeof(empty)) == 0)
		{
			Vector_erase(node->m_edges, _i);
			_i--;
			Edge_free(&edge);
		}
	}
	Vector_loopend;
}
Node Node_copy(Node node, Table oldtoNew)
{ //�������캯��
	Node r_node = Node_new(NULL, node->m_label);
	Table_put(oldtoNew, node, r_node);
	Vector_foreach(Edge, edge, node->m_edges)
	{
		Node newNode = Table_get(oldtoNew, edge->m_next);
		if (newNode)
		{
			Node_addEdgeCharset(r_node, newNode, edge->m_charRecv);
		}
		else
		{
			Node_addEdgeCharset(r_node, Node_copy(edge->m_next, oldtoNew), edge->m_charRecv);
		}
	}
	Vector_loopend;
	return r_node;
}

StateGraph StateGraph_new(char chr)
{ //״̬ͼ���캯��
	StateGraph sg;
	sg.m_Start = Node_new(NULL, 0);
	sg.m_End = Node_new(NULL, 0);
	Node_addNewEdge(sg.m_Start, sg.m_End, chr);
	return sg;
}

void StateGraph_free(StateGraph sg)
{ //״̬ͼ��������
	Vector allnodes = StateGraph_getAllNode(sg);
	Vector_foreach(Node, node, allnodes)
	{
		Vector_foreach(Edge, edge, node->m_edges)
		{
			Edge_free(&edge);
		}
		Vector_loopend;
		Node_free(&node);
	}
	Vector_loopend;
	Vector_free(&allnodes);
}

Vector StateGraph_epsilonClosure(Node node)
{ //��ȡ��ǰ�ڵ��epsilon�հ�
	Vector nodeList = Vector_new(Vector_getLength(node->m_edges));
	Vector_push_back(nodeList, node);
	Vector_foreach(Node, n, nodeList)
	{
		Vector_foreach(Edge, e, n->m_edges)
		{
			if (memcmp(e->m_charRecv, empty, 128) == 0	//�����ַ�Ϊ��
				&& !Vector_member(nodeList, e->m_next, NULL)) //��û�����Ѿ�������Ľڵ��г���
			{
				Vector_push_back(nodeList, e->m_next);
			}
		}
		Vector_loopend;
	}
	Vector_loopend;
	Vector_erase(nodeList, 0);
	return nodeList;
}

Vector StateGraph_validNode(StateGraph sg)
{ //��ȡ������Ч�ڵ㣺start�ڵ�������з�epsilon�߽���ĵ�
	Vector all = StateGraph_getAllNode(sg);
	Vector valid = Vector_new(all->m_head);
	Vector_foreach(Node, node, all)
	{
		Vector_foreach(Edge, edge, node->m_edges)
		{
			if (memcmp(edge->m_charRecv, empty, 128) != 0)
				if (!Vector_member(valid, edge->m_next, NULL))
				{
					Vector_push_back(valid, edge->m_next);
				}
		}
		Vector_loopend;
	}
	Vector_loopend;
	if (!Vector_member(valid, sg.m_Start, NULL))
	{
		Vector_push_back(valid, sg.m_Start);
	}
	Vector_free(&all);
	return valid;
}

void StateGraph_foreach(Node node, bool(*apply)(Node node, void* closure), void* closure)
{ //���ѱ�����apply����ֵΪtrue�Ľڵ�����������ѣ�����return��closure���������ⲿ����
	if (apply(node, closure))
	{
		Vector_foreach(Edge, edge, node->m_edges)
		{
			StateGraph_foreach(edge->m_next, apply, (void*)closure);
		}
		Vector_loopend;
	}
}

StateGraph StateGraph_copy(StateGraph stategraph)
{ //�������캯��
	StateGraph sg;
	Table oldtoNew = Table_new(0, NULL, NULL);
	sg.m_Start = Node_copy(stategraph.m_Start, oldtoNew);
	sg.m_End = Table_get(oldtoNew, stategraph.m_End);
	Table_free(&oldtoNew);
	return sg;
}

StateGraph StateGraph_parallel(StateGraph lsg, StateGraph rsg)
{ //�ϲ�״̬ͼ��������ƽ��
	Node newHead = Node_new(NULL, 0);
	Node newTail = Node_new(NULL, 0);

	Node_addEdgeCharset(newHead, lsg.m_Start, empty);
	Node_addEdgeCharset(newHead, rsg.m_Start, empty);
	Node_addEdgeCharset(lsg.m_End, newTail, empty);
	Node_addEdgeCharset(rsg.m_End, newTail, empty);
	lsg.m_Start = newHead;
	lsg.m_End = newTail;

	return lsg;
}

StateGraph StateGraph_connect(StateGraph lsg, StateGraph rsg)
{ //�ϲ�״̬ͼ������������
	Node_addEdgeCharset(lsg.m_End, rsg.m_Start, empty);
	lsg.m_End = rsg.m_End;
	return lsg;
}

StateGraph StateGraph_repeat(StateGraph sg)
{ //�ϲ�״̬ͼ��������>=0���ظ�
	Node newNode = Node_new(NULL, 0);
	Node_addEdgeCharset(newNode, sg.m_Start, empty);
	Node_addEdgeCharset(sg.m_End, newNode, empty);
	sg.m_Start = newNode;
	sg.m_End = newNode;
	return sg;
}

StateGraph StateGraph_repeat1(StateGraph sg)
{ //�ϲ�״̬ͼ��������>0���ظ�
	Table oldtonew = Table_new(0, NULL, NULL);
	Node copyHead = Node_copy(sg.m_Start, oldtonew);
	Node copyTail = Table_get(oldtonew, sg.m_End);
	Node_addEdgeCharset(sg.m_End, copyHead, empty);
	Node_addEdgeCharset(copyTail, sg.m_End, empty);
	return sg;
}

StateGraph StateGraph_optional(StateGraph sg)
{ //�ϲ�״̬ͼ����������ѡ
	Node_addEdgeCharset(sg.m_Start, sg.m_End, empty);
	return sg;
}

StateGraph StateGraph_removeEpsilon(StateGraph sg)
{ //ȥepsilon���㷨
	Vector allNode = StateGraph_getAllNode(sg);
	Vector validNode = StateGraph_validNode(sg);

	Vector_foreach(Node, currnode, validNode)
	{ //�������е���Ч�ڵ�
		Vector epsilonClosure = StateGraph_epsilonClosure(currnode); //��ȡ���ǵ�epsilon�հ�
		Vector_foreach(Node, node, epsilonClosure)
		{ //�հ��е�ÿһ����
			Vector_foreach(Edge, edge, node->m_edges)
			{ //��ÿһ����
				if (memcmp(edge->m_charRecv, empty, 128) != 0)
				{ //����Ƿ�epsilon�ߣ��͸���һ�ݼӵ���ǰ����
					Node_addEdge(currnode, Edge_copy(edge));
				}
			}
			Vector_loopend;
			if (node->m_label == REGEX_LABELEND)	 //�����ս�״̬
			{ //���epsilon�հ��еĵ����ս�㣬��ô��ǰ��Ҳ�����ս��
				currnode->m_label = REGEX_LABELEND;
			}
		}
		Vector_loopend;
		Vector_free(&epsilonClosure);
	}
	Vector_loopend;
	Vector_foreach(Node, node, allNode)
	{ //�ҳ�����Ҫfree���Ľڵ�
		if (!Vector_member(validNode, node, NULL))
		{ //����Ч��
			Vector_foreach(Edge, edge, node->m_edges)
			{ //�ͷ����б�
				Edge_free(&edge);
			}
			Vector_loopend;
			Node_free(&node); //ɾ�������
		}
	}
	Vector_loopend;
	Vector_foreach(Node, node, validNode)
	{ //������Ч�㣬ȥepsilon��
		Node_removeEpsilonEdge(node);
	}
	Vector_loopend;
	Vector_free(&allNode);
	Vector_free(&validNode);
	return sg;
}

Vector StateGraph_getAllNode(StateGraph sg)
{ //��ȡ���нڵ�
	Vector list = Vector_new(0);
	Set done = Set_new(0, NULL, NULL);

	Vector_push_back(list, sg.m_Start);
	Set_put(done, sg.m_Start);

	Vector_foreach(Node, node, list)
	{
		Vector_foreach(Edge, edge, node->m_edges)
		{
			if (!Set_member(done, edge->m_next))
			{
				Vector_push_back(list, edge->m_next);
				Set_put(done, edge->m_next);
			}
		}
		Vector_loopend;
	}
	Vector_loopend;
	Set_free(&done);
	return list;
}

StateGraph StateGraph_NFAtoDFA(StateGraph sg)
{ //��ȷ������״̬�� ת ȷ������״̬���㷨��ԭ��ο�������ԭ������Ĵʷ������½�
	Vector queueNodeSet = Vector_new(0);
	Set setNodeSet = Set_new(0, nodeSetCompare, nodeSetHash);
	Table tableNodeSetToNode = Table_new(0, nodeSetCompare, nodeSetHash);

	//�㷨�ĳ�ʼ������
	Set headSet = Set_new(0, NULL, NULL);
	Set_put(headSet, sg.m_Start);
	Vector_push_back(queueNodeSet, headSet);
	Set_put(setNodeSet, headSet);
	Table_put(tableNodeSetToNode, headSet, Node_new(NULL, -5));

	Vector_foreach(Set, nodeSet, queueNodeSet)
	{ //���ڶ������ÿ���ڵ㼯��
		Set charToNodeSet[128] = {NULL};	//�ַ�-�ڵ���ӳ���
		Node* allNodeSet = (Node*)Set_toArray(nodeSet, NULL);
		mint i;
		for (i = 0; allNodeSet[i] != NULL; i++)
		{ //�ڵ㼯�����ÿһ���ڵ�
			Vector_foreach(Edge, edge, allNodeSet[i]->m_edges)
			{ //ÿ���ڵ��ÿ����
				mint i;
				for (i = 0; i < 128; i++)
				{ //ÿ���������ܵ�ÿ���ַ�
					//�ѽ�������ַ���ͨ��Ľڵ�Ž��ַ�-�ڵ���ӳ�����
					if (edge->m_charRecv[i])
					{
						if (charToNodeSet[(char)i] == NULL)
						{
							charToNodeSet[(char)i] = Set_new(0, NULL, NULL);
						}
						Set_put(charToNodeSet[(char)i], edge->m_next);
					}
				}
			}
			Vector_loopend;
		}
		free(allNodeSet);
		for (i = 0; i < 128; i++)
		{ //ÿ���ַ�-�ڵ���ӳ��
			if (charToNodeSet[i])
			{
				Node newNode;
				if (Set_member(setNodeSet, charToNodeSet[i]))
				{ //�ڵ����Ѵ���
					//�ҵ�����ڵ�����Ӧ��DFA�ڵ�
					newNode = Table_get(tableNodeSetToNode, charToNodeSet[i]);
					//ɾ������Ѵ��ڵ��ظ��ڵ㼯��
					Set_free(&charToNodeSet[i]);
				}
				else
				{
					mint newNodeLabel = 0;
					//�ڵ������
					Vector_push_back(queueNodeSet, charToNodeSet[i]);
					//�ڵ��������
					Set_put(setNodeSet, charToNodeSet[i]);
					{ //������ڵ������Ƿ������ֹ�ڵ�
						Node* allNodeSet = (Node*)Set_toArray(charToNodeSet[i], NULL);
						int i;
						for (i = 0; allNodeSet[i] != NULL; i++)
						{
							if (allNodeSet[i]->m_label == -3)
							{
								newNodeLabel = -3;
								break;
							}
						}
						free(allNodeSet);
					}
					//�½�����ڵ�
					newNode = Node_new(NULL, newNodeLabel);
					//�����ڽڵ���-DFA�ڵ�ӳ���
					Table_put(tableNodeSetToNode, charToNodeSet[i], newNode);
				}
				//��ǰ�ڵ�����Ӧ��DFA�ڵ�->�ӱ�ָ��ղŻ�õ�DFA�ڵ�
				Node_addNewEdge((Node)Table_get(tableNodeSetToNode, nodeSet), newNode, (char)i);
			}
		}
	}
	Vector_loopend;

	//ɾ��ԭ����NFA
	StateGraph_free(sg);
	//�����µ�DFA
	sg.m_Start = Table_get(tableNodeSetToNode, headSet);
	sg.m_End = NULL;
	//Set_free(&headSet);
	//Set_remove(setNodeSet, headSet);
	Vector_free(&queueNodeSet);
	{
		Set*allNodeSet = (Set*)Set_toArray(setNodeSet, NULL);
		mint i;
		for (i = 0; allNodeSet[i]; i++)
		{
			Set_free(&allNodeSet[i]);
		}
		free(allNodeSet);
	}
	Set_free(&setNodeSet);
	Table_free(&tableNodeSetToNode);
	
	return sg;
}

StateGraph StateGraph_mark(StateGraph sg)
{ //���״̬ͼ��ÿ���ڵ�һ��Ψһ�ı��
	Vector allNodes = StateGraph_getAllNode(sg);
	Vector_foreach(Node, node, allNodes)
	{
		node->m_label = _i + 1;
	}
	Vector_loopend;
	Vector_free(&allNodes);
	sg.m_Start->m_label = REGEX_LABELSTART;
	sg.m_End->m_label = REGEX_LABELEND;
	return sg;
}

FSM StateGraph_getFSM(StateGraph sg)
{ //״̬ͼתΪ״̬��
	Vector allNodes = StateGraph_getAllNode(sg);
	FSM fsm = calloc((Vector_getLength(allNodes) + 1), sizeof(fsm[0]));
	int stateindex = 1;
	Vector_foreach(Node, node, allNodes)
	{
		if (node->m_label == REGEX_LABELEND)
		{
			fsm[stateindex][0] = -1;
		}
		node->m_label = stateindex;
		stateindex++;
	}
	Vector_loopend;
	StateGraph_debugOutput(sg);
	Vector_foreach(Node, node, allNodes)
	{
		Vector_foreach(Edge, edge, node->m_edges)
		{
			int i;
			for (i = 0; i < 128; i++)
			{
				fsm[node->m_label][i] = edge->m_charRecv[i] ? edge->m_next->m_label : fsm[node->m_label][i];
			}
		}
		Vector_loopend;
	}
	Vector_loopend;
	Vector_free(&allNodes);
	assert(sg.m_Start->m_label == 1);
	return fsm;
}

void StateGraph_debugOutput(StateGraph sg)
{ //�������
#ifdef REGEX_DEBUG
	Vector allNode = StateGraph_getAllNode(sg);
	Vector_foreach(Node, node, allNode)
	{
		printf("Node #%d %#p\n", node->m_label, node);
		Vector_foreach(Edge, edge, node->m_edges)
		{
			int i;
			printf(" Edge");
			for (i = 0; i < 128; i++)
			{
				if (edge->m_charRecv[i] == 1)
				{
					printf(" %c ", i);
				}
			}
			printf(" %#p #%d\n", edge->m_next, edge->m_next->m_label);
		}
		Vector_loopend;
	}
	Vector_loopend;
	printf("\n");
	Vector_free(&allNode);
#endif //REGEX_DEBUG
}

static int nodeSetCompare(const void* a1, const void* b1)
{ //�������ڼ��ϣ���ϣ���ıȽϺ������Ƚ������ڵ㼯���Ƿ����
	Set a = (Set)a1;
	Set b = (Set)b1;
	if (Set_length(a) != Set_length(b))
	{
		return 1;
	}
	else
	{
		Node* allnodea = (Node*)Set_toArray(a, NULL);
		Node* allnodeb = (Node*)Set_toArray(b, NULL);
		int i;
		for (i = 0; allnodea[i]; i++)
		{
			if (allnodea[i] != allnodeb[i])
			{
				free(allnodea);
				free(allnodeb);
				return 1;
			}
		}
		free(allnodea);
		free(allnodeb);
		return 0;
	}
}
static unsigned int nodeSetHash(const void* a1)
{ //���ڽڵ㼯�ϵ�hash����
	Set a = (Set)a1;
	unsigned hash  = 0;
	Node* allnode = (Node*)Set_toArray(a, NULL);
	int i;
	for (i = 0; allnode[i]; i++)
	{
		hash += (unsigned)allnode[i];
	}
	free(allnode);
	return hash >> 2;
}
