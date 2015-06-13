#include "regex.h"
/************************************************************************/
/* 正则表达式状态机的实现                                                 */
/************************************************************************/

static char empty[128]; //空边

static bool StateGraph_getValidNode(Node node, Closure* closure);
static int nodeSetCompare(const void* a, const void* b);
static unsigned int nodeSetHash(const void* a1);

Edge Edge_new(char charRecv[128], Node next)
{ //构造函数
	Edge edge;
	NEW(edge);

	memcpy(edge->m_charRecv, charRecv, sizeof(edge->m_charRecv));
	edge->m_next = next;

	return edge;
}

Edge Edge_copy(Edge edge)
{ //复制构造函数
	Edge e;
	NEW(e);
	memcpy(e->m_charRecv, edge->m_charRecv, sizeof(e->m_charRecv));
	e->m_next = edge->m_next;
	return e;
}

bool Edge_equals(Edge a, Edge b)
{ //operator==重载
	return memcmp(a->m_charRecv, b->m_charRecv, sizeof(a->m_charRecv)) == 0
		&& a->m_next == b->m_next;
}

void Edge_free(Edge* edge)
{ //析构函数
	free(*edge);
	*edge = NULL;
}

Node Node_new(Node node, mint label)
{ //构造函数
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
{ //析构函数
	Vector_free(&((*node)->m_edges));
	free(*node);
	*node = NULL;
}
void Node_addNewEdge(Node node, Node next, char ch)
{ //成员方法，加一条边
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
{ //加一条边的重载，C里面只好起另一个名字
	Vector_push_back(node->m_edges, Edge_new(charset, next));
}
void Node_addEdge(Node node, Edge edge)
{ //加边的另一个重载
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
{ //移除所有空边
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
{ //拷贝构造函数
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
{ //状态图构造函数
	StateGraph sg;
	sg.m_Start = Node_new(NULL, 0);
	sg.m_End = Node_new(NULL, 0);
	Node_addNewEdge(sg.m_Start, sg.m_End, chr);
	return sg;
}

void StateGraph_free(StateGraph sg)
{ //状态图析构函数
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
{ //获取当前节点的epsilon闭包
	Vector nodeList = Vector_new(Vector_getLength(node->m_edges));
	Vector_push_back(nodeList, node);
	Vector_foreach(Node, n, nodeList)
	{
		Vector_foreach(Edge, e, n->m_edges)
		{
			if (memcmp(e->m_charRecv, empty, 128) == 0	//接受字符为空
				&& !Vector_member(nodeList, e->m_next, NULL)) //并没有在已经处理过的节点中出现
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
{ //获取所有有效节点：start节点和所有有非epsilon边进入的点
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
{ //深搜遍历，apply返回值为true的节点继续进行深搜，否则return，closure用来传递外部变量
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
{ //拷贝构造函数
	StateGraph sg;
	Table oldtoNew = Table_new(0, NULL, NULL);
	sg.m_Start = Node_copy(stategraph.m_Start, oldtoNew);
	sg.m_End = Table_get(oldtoNew, stategraph.m_End);
	Table_free(&oldtoNew);
	return sg;
}

StateGraph StateGraph_parallel(StateGraph lsg, StateGraph rsg)
{ //合并状态图————平行
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
{ //合并状态图————连接
	Node_addEdgeCharset(lsg.m_End, rsg.m_Start, empty);
	lsg.m_End = rsg.m_End;
	return lsg;
}

StateGraph StateGraph_repeat(StateGraph sg)
{ //合并状态图————>=0次重复
	Node newNode = Node_new(NULL, 0);
	Node_addEdgeCharset(newNode, sg.m_Start, empty);
	Node_addEdgeCharset(sg.m_End, newNode, empty);
	sg.m_Start = newNode;
	sg.m_End = newNode;
	return sg;
}

StateGraph StateGraph_repeat1(StateGraph sg)
{ //合并状态图————>0次重复
	Table oldtonew = Table_new(0, NULL, NULL);
	Node copyHead = Node_copy(sg.m_Start, oldtonew);
	Node copyTail = Table_get(oldtonew, sg.m_End);
	Node_addEdgeCharset(sg.m_End, copyHead, empty);
	Node_addEdgeCharset(copyTail, sg.m_End, empty);
	return sg;
}

StateGraph StateGraph_optional(StateGraph sg)
{ //合并状态图————可选
	Node_addEdgeCharset(sg.m_Start, sg.m_End, empty);
	return sg;
}

StateGraph StateGraph_removeEpsilon(StateGraph sg)
{ //去epsilon边算法
	Vector allNode = StateGraph_getAllNode(sg);
	Vector validNode = StateGraph_validNode(sg);

	Vector_foreach(Node, currnode, validNode)
	{ //对于所有的有效节点
		Vector epsilonClosure = StateGraph_epsilonClosure(currnode); //获取他们的epsilon闭包
		Vector_foreach(Node, node, epsilonClosure)
		{ //闭包中的每一个点
			Vector_foreach(Edge, edge, node->m_edges)
			{ //的每一条边
				if (memcmp(edge->m_charRecv, empty, 128) != 0)
				{ //如果是非epsilon边，就复制一份加到当前点上
					Node_addEdge(currnode, Edge_copy(edge));
				}
			}
			Vector_loopend;
			if (node->m_label == REGEX_LABELEND)	 //代表终结状态
			{ //如果epsilon闭包中的店是终结点，那么当前点也属于终结点
				currnode->m_label = REGEX_LABELEND;
			}
		}
		Vector_loopend;
		Vector_free(&epsilonClosure);
	}
	Vector_loopend;
	Vector_foreach(Node, node, allNode)
	{ //找出所有要free掉的节点
		if (!Vector_member(validNode, node, NULL))
		{ //是无效点
			Vector_foreach(Edge, edge, node->m_edges)
			{ //释放所有边
				Edge_free(&edge);
			}
			Vector_loopend;
			Node_free(&node); //删除这个点
		}
	}
	Vector_loopend;
	Vector_foreach(Node, node, validNode)
	{ //所有有效点，去epsilon边
		Node_removeEpsilonEdge(node);
	}
	Vector_loopend;
	Vector_free(&allNode);
	Vector_free(&validNode);
	return sg;
}

Vector StateGraph_getAllNode(StateGraph sg)
{ //获取所有节点
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
{ //非确定有限状态机 转 确定有限状态机算法，原理参考《编译原理》等书的词法分析章节
	Vector queueNodeSet = Vector_new(0);
	Set setNodeSet = Set_new(0, nodeSetCompare, nodeSetHash);
	Table tableNodeSetToNode = Table_new(0, nodeSetCompare, nodeSetHash);

	//算法的初始化部分
	Set headSet = Set_new(0, NULL, NULL);
	Set_put(headSet, sg.m_Start);
	Vector_push_back(queueNodeSet, headSet);
	Set_put(setNodeSet, headSet);
	Table_put(tableNodeSetToNode, headSet, Node_new(NULL, -5));

	Vector_foreach(Set, nodeSet, queueNodeSet)
	{ //对于队列里的每个节点集合
		Set charToNodeSet[128] = {NULL};	//字符-节点组映射表
		Node* allNodeSet = (Node*)Set_toArray(nodeSet, NULL);
		mint i;
		for (i = 0; allNodeSet[i] != NULL; i++)
		{ //节点集合里的每一个节点
			Vector_foreach(Edge, edge, allNodeSet[i]->m_edges)
			{ //每个节点的每条边
				mint i;
				for (i = 0; i < 128; i++)
				{ //每条边所接受的每个字符
					//把接受这个字符所通向的节点放进字符-节点组映射表里
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
		{ //每个字符-节点组映射
			if (charToNodeSet[i])
			{
				Node newNode;
				if (Set_member(setNodeSet, charToNodeSet[i]))
				{ //节点组已存在
					//找到这个节点所对应的DFA节点
					newNode = Table_get(tableNodeSetToNode, charToNodeSet[i]);
					//删掉这个已存在的重复节点集合
					Set_free(&charToNodeSet[i]);
				}
				else
				{
					mint newNodeLabel = 0;
					//节点组进队
					Vector_push_back(queueNodeSet, charToNodeSet[i]);
					//节点组进集合
					Set_put(setNodeSet, charToNodeSet[i]);
					{ //这里检查节点组里是否存在终止节点
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
					//新建这个节点
					newNode = Node_new(NULL, newNodeLabel);
					//保存在节点组-DFA节点映射表
					Table_put(tableNodeSetToNode, charToNodeSet[i], newNode);
				}
				//当前节点所对应的DFA节点->加边指向刚才获得的DFA节点
				Node_addNewEdge((Node)Table_get(tableNodeSetToNode, nodeSet), newNode, (char)i);
			}
		}
	}
	Vector_loopend;

	//删掉原来的NFA
	StateGraph_free(sg);
	//换上新的DFA
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
{ //标记状态图的每个节点一个唯一的标号
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
{ //状态图转为状态机
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
{ //调试输出
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
{ //两个用于集合（哈希表）的比较函数，比较两个节点集合是否相等
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
{ //用于节点集合的hash函数
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
