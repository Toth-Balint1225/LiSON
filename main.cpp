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

class MyObj;
class KeyValuePair : public LiSON
{
	friend class MyObj;
public:
	std::string key;
	std::string data;

	KeyValuePair()
		: key("default")
		, data("default")
	{}

	void debug_print()
	{
		std::cout << key << " -> " << data << std::endl;
	}

protected:
    virtual void interpret(const Object& obj) override
	{
		auto list = obj.expectObjectData().value_or(std::list<Object>());
		key = list.front().expectKeywordData().value_or("error");
		data = list.back().expectLiteralData().value_or("error");
	}

    virtual Object revert() const
	{
		Object o_root = Object::empty();
		o_root.add(Object::fromKeyword(key));
		o_root.add(Object::fromString(data));
		return o_root;
	}
	
};

class MyObj : public LiSON
{
public:
	std::list<KeyValuePair> data;
protected:
    /**
     * Implementation of the interpret function.
     * The interpret function turns the Object into the desired
     * class.
     */
    virtual void interpret(const Object& obj) override
    {
		std::cout << "Interpretation happens" << std::endl;
		// the shiny new api
		obj.foreachObjectData([this](const Object& o)
		{
			KeyValuePair pair;
			pair.interpret(o);
			data.push_back(pair);
		});
    }

    /**
     * Implementation of the revert function.
     * This should create an object with a literal in it.
     */
    virtual Object revert() const 
    {
		Object o_root = Object::empty();
		for (auto it : data)
			o_root.add(Object::fromLiSON(it));

		return o_root;
    }
public:
    // some conveinence and debug functions
    MyObj()
    {}


    void debug_print()
    {
		for (auto it : data)
			it.debug_print();
		// std::cout << "data: " << data << std::endl;
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
