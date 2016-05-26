struct Node{
	int tokenFlag;
	int line;
	struct Node *child;
	struct Node *sibling;
	struct Node *parent;
	char type[33];
	char text[33];
};

