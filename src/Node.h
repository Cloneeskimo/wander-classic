
#ifndef Node_h
#define Node_h

//Error Codes Used: 0-2

//Includes
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <Clfram.h>

//Node Struct
struct Node {
public:

	//Static Data
	static const char INDENT_CHAR = '\t';

	//Constructors
	Node() { //default constructor
		this->name = this->value = "";
	}
	Node(std::string name, std::string value = "") { //full constructor
		this->name = clUtil::uppercase(name);
		this->value = value;
	}
	Node(std::string name, int value = 0) { //constructor with int
		this->name = clUtil::uppercase(name);
		this->value = std::to_string(value);
	}
	Node(std::string name, char value = ' ') { //constructor with char
		this->name = clUtil::uppercase(name);
		this->value = std::string(1, value);
	}

	//Equals Operator
	friend bool operator==(const Node& lhs, const std::string& rhs) {
		if (const_cast<Node&>(lhs).name == const_cast<std::string&>(clUtil::uppercase(rhs))) return true; //check name
		else return false;
	}

	//Children Manipulation
	void addChildWithValue(std::string title, std::string value) { this->children.push_back(Node(title, value)); }
	void addChild(Node child) { this->children.push_back(child); }
	int getChildCount() { return this->children.size(); }
	std::string getChildValue(int index) { return this->children[index].getValue(); }

	//Value Accessors/Mutators
	int getValueAsInt() { return stoi(this->value); }
	double getValueAsDouble() { return stod(this->value); }
	float getValueAsFloat() { return stof(this->value); }
	char getValueAsChar() { return this->value[0]; }
	std::string getValue() { return this->value; }
	void setValue(std::string value) { this->value = value; }

	//Children Accessors
	Node* getChild(int index) { return &(this->children[index]); }
	std::vector<Node>* getChildren() { return &(this->children); }
	Node* getChildByName(std::string name) {
		std::string uppercaseName = clUtil::uppercase(name);
		for (int i = 0; i < this->children.size(); i++) {
			if (this->children[i].getName() == uppercaseName) return &(this->children[i]);
		}
		clUtil::error("No child with name '" + uppercaseName + "'", "NODE_H", 0, "data\\error\\", "ErrorLog", "DO_DISPLAY DO_LOG FATAL_ERROR");
	}

	//Name Accessor/Mutator
	std::string getName() { return this->name; }
	std::string setName(std::string name) { this->name = clUtil::uppercase(name); }
	
	//Writing Function
	static void writeNode(std::string filePath, Node* node) { writeNode(&filePath, node); }
	static void writeNode(std::string* filePath, Node* node) {

		//open file and being recursive writing process
		std::ofstream write;
		write.open(*filePath);
		if (write.fail()) clUtil::error("unable to open file '" + *filePath + "'", "NODE_H", 1, "data\\error\\", "ErrorLog", "DO_DISPLAY DO_LOG FATAL_ERROR");
		writeNodeR(&write, node, "");
	}

	//Reading Functions
	static Node readNode(std::string filePath) { return readNode(&filePath); }
	static Node readNode(std::string* filePath) {

		//open file
		std::ifstream read;
		read.open(*filePath); //try to open file
		if (read.fail()) clUtil::error("unable to open file '" + *filePath + "'", "NODE_H", 2, "data\\error\\", "ErrorLog", "DO_DISPLAY DO_LOG FATAL_ERROR");
    
		//read entire file into vector of strings
		std::vector<std::string> file;
		std::string nextLine;
		while (!read.eof()) {
			getline(read, nextLine);
			file.push_back(nextLine);
		}
    
		//recursively loop through vector of strings to build the node
		int i = 0;
		Node n = readNodeR(&file, &i, 0);
		return n;
	}

private:

	//Data
	std::string name;
	std::string value;
	std::vector<Node> children;

	//Recursive Writing Function
	static void writeNodeR(std::ofstream* write, Node* data, std::string indent) {
		*write << indent << data->getName() << ": "; //indent and write node name
		if (data->getValue() != "") //write node value if there is one
			*write << data->getValue();
		*write << std::endl;
		if (data->getChildCount() > 0) { //if node has children
			*write << indent << "{" << std::endl;
			for (Node c : *(data->getChildren())) //save all children
				writeNodeR(write, &c, indent + std::string(1, Node::INDENT_CHAR));
			*write << indent <<  "}" << std::endl;
		}
	}

	//Recursive Reading Function
	static Node readNodeR(std::vector<std::string>* file, int* i, int indent) {

		//format next line and find dividing point
		std::string nextLine = file->at(*i); //nextLine
		nextLine = nextLine.substr(indent, nextLine.length() - indent); //remove indent
		int dividerLoc = -1; //location of divider in line
		for (int j = 0; j < nextLine.length() && dividerLoc == -1; j++) //find location of divider in line
			if (nextLine[j] == ':')
				dividerLoc = j;
    
		//throw error if there is no colon in line
		if (dividerLoc == -1) std::cout << "NO COLON IN LINE" << std::endl;
    
		//create node represented by next line
		Node node = Node(nextLine.substr(0, dividerLoc), ""); //create node with name
		std::string posVal = nextLine.substr(dividerLoc + 1, nextLine.length() - dividerLoc - 1); //load value
		if (posVal != " " && posVal != "") //if there is a value
			node.setValue(posVal.substr(1, posVal.length() - 1)); //assign it to the new node (remove space)
    
		//check for more file
		if (*i + 1 <= file->size()) { //if not eof
        
			//check for child nodes
			if (file->at(*i + 1).find("{") != std::string::npos) { //if the node has children
				*i += 2; //iterate twice
				indent++; //iterate indent
				while (file->at(*i).find("}") == std::string::npos) { //if the children are over
					node.addChild(readNodeR(file, i, indent));
                
					//throw error if file suddenly stop
					if ((*i + 1) > file->size()) std::cout << "error reading file: sudden file stop after line '" << nextLine << std::endl;
                
					*i += 1; //iterate
				}
			}
		}
		return node; //return newly-loaded node
	}
};

#endif