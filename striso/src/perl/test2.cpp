
#include <iostream>

class GdmoObject {
    public:
	const char *const name;
	GdmoObject(const char *n);
	virtual const char *nameGet();
};

class Behaviour : public GdmoObject {
    public:
	const char *const definedas;
	Behaviour(const char *n, const char *d);
	virtual const char *definedasGet();
};

class XXXBehaviour : public Behaviour {
    public:
	static const char name[];
	static const char definedas[];
	XXXBehaviour();
};

GdmoObject::GdmoObject(const char *n) : name(n) { };
const char *GdmoObject::nameGet() { return name; }

Behaviour::Behaviour(const char *n, const char *d) : GdmoObject(n), definedas(d) { };
const char *Behaviour::definedasGet() { return definedas; }

XXXBehaviour::XXXBehaviour() : Behaviour(name,definedas) { };

const char XXXBehaviour::name[] = "XXXBehaviour";
const char XXXBehaviour::definedas[] = "this is the XXX Behaviour";

int main (int argc, char *argv[])
{
    XXXBehaviour b;

    cout << b.nameGet() << "\n";
    cout << b.definedasGet() << "\n";

    return (0);
}
