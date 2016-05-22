struct Node{
	int tokenFlag;
	int line;
	char type[32];
	char text[32];
	struct Node *child;
	struct Node *sibling;
	struct Node *parent;
};

