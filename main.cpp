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
    // the class contains only one string, its LiSON equivalent is ('data')
    std::string data;
protected:
    /**
     * Implementation of the interpret function.
     * The interpret function turns the Object into the desired
     * class.
     */
    virtual void interpret(const Object& obj) override
    {
		auto interpreter = overload
		{
			[this] (const Tkn_Object& object)
			{
				auto innerInterpreter = overload
				{
					[] (const Tkn_Object& o) {},
					[this] (const Tkn_Literal& l)
					{
						data = l.value;
					},
					[this] (const Tkn_Error& e)
					{
						data = "ERROR";
					}
				};
				if (!object.value.empty())
					std::visit(innerInterpreter,object.value.front().token);
			},
			[] (const Tkn_Literal& l) {},
			[this] (const Tkn_Error& e)
			{
				data = "ERROR";
			}
		};
		std::visit(interpreter,obj.token);
    }

    /**
     * Implementation of the revert function.
     * This should create an object with a literal in it.
     */
    virtual Object revert() const 
    {
        // object for the ()
        Object o_root(Token{Tkn_Object{}});
        // object for the literal
        Object o_data(Token{Tkn_Literal{data}});
        // set the data of the appropriate object
		auto adder = overload
		{
			[&o_data](Tkn_Object& obj)
			{
				obj.value.push_back(o_data);
			},
			[](Tkn_Literal& l) {},
			[](Tkn_Error& e) {}
		};
		std::visit(adder,o_root.token);
        return o_root;
    }
public:
    // some conveinence and debug functions
    MyObj()
        : data("")
    {}

    MyObj(const std::string& _data)
        : data(_data)
    {}

    void debug_print()
    {
        std::cout << "data: " << data << std::endl;
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
