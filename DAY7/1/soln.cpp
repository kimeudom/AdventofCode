#include <bits/stdc++.h>
#include <fstream>
#include <iostream>

using namespace std;

// Tree nodes
class TreeNode {
private:
  string name;
  int size;
  TreeNode *parent;
  TreeNode *nextSibling;
  TreeNode *firstChild;

public:
  TreeNode(string DirName);
  TreeNode(string fileName, TreeNode *prt, int value);
  TreeNode *getLastChild();
  void addSibling(TreeNode *sib) { nextSibling = sib; };
  void addFirstChild(TreeNode *chld) { firstChild = chld; };
  void addParent(TreeNode *prt) { parent = prt; };
  TreeNode *getParent() { return parent; };
  TreeNode *getNextSibling() { return nextSibling; };
  TreeNode *getFirstChild() { return firstChild; };
  string getName() { return name; };
  friend int getSize(TreeNode *self);
};

// Class TreeNode memeber functions

// Create a null tree node, helpful for creating directories
TreeNode::TreeNode(string DirName) {
  size = 0;
  parent = NULL;
  nextSibling = NULL;
  firstChild = NULL;
  name = DirName;
}

// Constructor for files (not directories)
TreeNode::TreeNode(string fileName, TreeNode *prt, int value = 0) {
  size = value;
  name = fileName;
  parent = prt;
  nextSibling = NULL;
  firstChild = NULL;
}

// Returns NULL if the node has no children
// or returns the pointer to the last child in the linked list
TreeNode *TreeNode::getLastChild() {
  TreeNode *temp = firstChild;
  if (temp == NULL) {
    // Has no children
    return temp;
  }

  while (temp->nextSibling != NULL) {
    temp = temp->nextSibling;
  }

  // Return last child
  return temp;
}

// Returns the size of the file or the directory beneath it (if present)
int getSize(TreeNode *self) {
  if (self->firstChild == NULL) {
    // Return the file size since it has no children
    if (self->size > 100000) {
      return self->size;
    }
    return self->size;
  }

  // Traverse the entire tree beneath the directory and return its size
  // Level order traversal
  int sum = 0;
  TreeNode *temp = self->getFirstChild();
  while (temp != NULL) {
    sum += getSize(temp);
    temp = temp->getNextSibling();
  }
  delete (temp);

  return sum;
}

void testing() {
  TreeNode *root = new TreeNode("/");
  // 4 kids , 2 dir , 2 files for /
  TreeNode *a = new TreeNode("a", root);
  TreeNode *b = new TreeNode("b", root, 10);
  TreeNode *c = new TreeNode("c", root);
  TreeNode *d = new TreeNode("d", root, 50);
  // a has 3 files
  TreeNode *e = new TreeNode("e", a, 37);
  TreeNode *f = new TreeNode("f", a, 44);
  TreeNode *g = new TreeNode("g", a, 68);
  a->addFirstChild(e);
  e->addSibling(f);
  f->addSibling(g);
  // c has 4 files and 1 dir
  TreeNode *h = new TreeNode("h", c, 512);
  TreeNode *i = new TreeNode("i", c, 648);
  TreeNode *j = new TreeNode("j", c, 488);
  TreeNode *k = new TreeNode("k", c, 9);
  TreeNode *l = new TreeNode("l", c);
  c->addFirstChild(h);
  h->addSibling(i);
  i->addSibling(j);
  j->addSibling(k);
  k->addSibling(l);

  // Add a level 3 on l
  TreeNode *m = new TreeNode("m", l, 889);
  TreeNode *n = new TreeNode("n", l, 89);
  TreeNode *o = new TreeNode("o", l, 356);
  TreeNode *p = new TreeNode("p", l, 157);
  TreeNode *q = new TreeNode("q", l, 3);
  l->addFirstChild(m);
  m->addSibling(n);
  n->addSibling(o);
  o->addSibling(p);
  p->addSibling(q);

  cout << "a : " << getSize(a) << endl;
  cout << "b : " << getSize(b) << endl;
  cout << "c : " << getSize(c) << endl;
  cout << "d : " << getSize(d) << endl;

  cout << "l : " << getSize(l) << endl;
}

void addChild(TreeNode *prt, TreeNode *chld) {
  if (prt->getFirstChild() == NULL) {
    prt->addFirstChild(chld);
    chld->addParent(prt);
    return;
  }

  TreeNode *last = prt->getLastChild();
  last->addSibling(chld);
  chld->addParent(prt);
  return;
}

// Executes the commands passed from the line inT the current dir
void execCommand_cd(string line, TreeNode *dir, TreeNode **temp) {
  // Available Commands
  // cd [d] [/] [..]
  string command = line.substr(0, 3);

  // Get command argument
  string commandArg = line.substr(5, line.length() - 1);

  if (commandArg == "/") {
    // Go to parent
    while (dir->getParent() != NULL) {
      dir = dir->getParent();
    }
    *temp = dir;
  } else if (commandArg == "..") {
    // Go to above dir
    dir = dir->getParent();
    *temp = dir;
  } else {
    // Change directory to the supplied argument
    TreeNode *dirTemp = dir->getFirstChild();
    while (dirTemp->getName() != commandArg) {
      dirTemp = dirTemp->getNextSibling();
    }
    *temp = dirTemp;
  }
}

void execCommand_ls(string line, string isDir, TreeNode *dir) {
  if (isDir == "dir") {
    string dirName = line.substr(4, line.length() - 1);
    TreeNode *tempDir = new TreeNode(dirName);
    addChild(dir, tempDir);
  } else {
    // is a file
    // File format
    // [filesize[int]] newFileName
    int fileSize = stoi(line);
    string newFileName = line.substr(line.find(' ') + 1, line.length() - 1);
    TreeNode *file = new TreeNode(newFileName, dir, fileSize);
    addChild(dir, file);
  }
}
int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "Usage :: ./soln <<puzzle input>>" << endl;
    return -1;
  }

  string filename = argv[1];
  ifstream input;
  input.open(filename);

  if (input.fail()) {
    cerr << "Failed to open file" << endl;
    return -2;
  }

  string line;
  // The parent tree Node
  TreeNode *root = new TreeNode("/");
  TreeNode *temp = root;

  // 1 if last executed command was ls
  // 0 if not
  int lsExitCode = 0;

  while (input.peek() != EOF) {
    if (lsExitCode != 0) {
      lsExitCode = 0;
    } else {
      getline(input, line, '\n');
    }

    // Differenciate between commands and files
    // Commands start with the (Dollar)$ sign
    // ls and cd commands
    string command;
    command = line.substr(0, 4);

    // Perform command operations
    if (command == "$ cd") {
      // Perform cd operations
      execCommand_cd(line, temp, &temp);
    } else {
      // Perform  ls functionality
      // Command is "$ ls"
      // Perform ls operations
      // Read until line[0] is $ i.e. the next command
      do {
        string isDir;
        getline(input, line, '\n');
        if (line[0] == '$') {
          break;
        }
        isDir = line.substr(0, 3);
        // Move to the line below "ls"
        // Check dir content type
        // dir
        // 7798 [file size]
        if (isDir == "dir") {
          string dirName = line.substr(4, line.length() - 1);
          TreeNode *tempDir = new TreeNode(dirName);
          addChild(temp, tempDir);
        } else {
          // is a file
          // File format
          // [filesize[int]] newFileName
          int fileSize = stoi(line);
          string newFileName =
              line.substr(line.find(' ') + 1, line.length() - 1);
          TreeNode *file = new TreeNode(newFileName, temp, fileSize);
          addChild(temp, file);
        }
      } while (input.peek() != EOF);
      lsExitCode = 1;
    }
  }

  cout << getSize(root) << endl;
  return 0;
}