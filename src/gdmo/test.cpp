
#include <iostream>

static const char mydescrip[] = "description";

class descrip {
    public:
	const char *const string;
	descrip(const char *d) : string(d) { };
	descrip() : string(NULL) { };
	virtual ~descrip() { };
};

class parent {
    public:
	static const int number;
	descrip desc;
	virtual int numberGet(void) { return number; }
	parent(const char *d) : desc(d) { };
	parent() : desc(NULL) { };
	virtual ~parent(void) { };
};

const int parent::number = 5;

class child : public parent {
    public:
	static const int number;
	virtual int numberGet(void) { return parent::number; }
	child(const char *d) : parent(d) { };
	child(void) : parent(mydescrip) { };
	virtual ~child(void) { };
};

const int child::number = 7;

int main(int argc, char *argv[])
{
    child c;
    cout << c.numberGet() << "\n";
    cout << c.number << "\n";
    parent *p = dynamic_cast<parent *>(&c);
    cout << p->number << "\n";
    cout << c.desc.string << "\n";
    return (0);
}
