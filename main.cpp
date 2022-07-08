/**
 * Functionallity test and example for LiSON.
 */
#include <iostream>
#include "LiSON_base.h"

// just to make life easier
using namespace lison;

/**
 * Implementation of the LiSON interface
 */


class MyObj : public LiSON
{
public:
	std::list<double> data;
protected:
    /**
     * Implementation of the interpret function.
     * The interpret function turns the Object into the desired
     * class.
     */
    virtual void interpret(const Object& obj) override
    {
		// the shiny new api
		obj.foreachObjectData([this](const Object& o)
		{
			data.push_back(o.expectFloatData().value_or(0.0));
		});
    }

    /**
     * Implementation of the revert function.
     * This should create an object with a literal in it.
     */
    virtual Object revert() const 
    {
		Object o_root(Token{Tkn_Object{}});

		for (auto it : data)
			o_root.add(Object::fromFloat(it));

		return o_root;
    }
public:
    // some conveinence and debug functions
    MyObj()
    {}


    void debug_print()
    {
		for (auto it : data)
			std::cout << "data: " << it << std::endl;
    }
};

int main()
{
    // the test objects
    MyObj obj;
    Serializer serializer;

    // read the object from a file
    "test.lison" >> serializer >> obj;

    // write the same object to another file
    "copy.lison" >> serializer << obj;

    // wtite the data into a string
    std::string after;
    after << obj;

    obj.debug_print();
    std::cout << after << std::endl;
    return 0;
}
