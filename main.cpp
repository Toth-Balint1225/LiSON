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
	std::list<std::string> data;
protected:
    /**
     * Implementation of the interpret function.
     * The interpret function turns the Object into the desired
     * class.
     */
    virtual void interpret(const Object& obj) override
    {
		// this one still works
		// auto interpreter = overload
		// {
		// 	[this] (const Tkn_Object& object)
		// 	{
		// 		auto innerInterpreter = overload
		// 		{
		// 			[] (const Tkn_Object& o) {},
		// 			[this] (const Tkn_Literal& l)
		// 			{
		// 				data = l.value;
		// 			},
		// 			[this] (const Tkn_Error& e)
		// 			{
		// 				data = "ERROR";
		// 			}
		// 		};
		// 		if (!object.value.empty())
		// 			std::visit(innerInterpreter,object.value.front().token);
		// 	},
		// 	[] (const Tkn_Literal& l) {},
		// 	[this] (const Tkn_Error& e)
		// 	{
		// 		data = "ERROR";
		// 	}
		// };
		// std::visit(interpreter,obj.token);

		// this is just another way of parsing the thing
		// if (std::holds_alternative<Tkn_Object>(obj.token))
		// {
		// 	auto object = std::get<Tkn_Object>(obj.token);
		// 	for (auto it : object.value)
		// 		if (std::holds_alternative<Tkn_Literal>(it.token))
		// 		{
		// 			auto value = std::get<Tkn_Literal>(it.token);
		// 			data.push_back(value.value);
		// 		}
		// }


		// the shiny new api
		obj.foreachObjectData([this](const Object& o)
		{
			data.push_back(o.expectLiteralData().value_or("ERROR"));
		});
		
    }

	/*
	std::optional<std::list<lison::Object>> object = obj.expectObjectData();

	obj.foreachObjectData([&data](const lison::Object& obj) {
		data.push_back(obj.expectLiteralData().value_or("ERROR"));
	});

	for (std::string it : data)
		o_root.add(Object::fromString(it));

	factory functions
	static Object =>
	fromString(std::string)
	fromStringList(std::list<std::string>)
	fromLiSON(const Lison&)
	fromObjectList<T>(std::list<T>,std::function<Object(T)>)

	std::list<Coord> cs;
	Object::fromObjectList<Coord>(cs,[] (auto& c) {std::stringstream ss(); ss << "("<< c.x << "," << c.y << ")";
		return Object::fromString(ss.str());}
	*/

    /**
     * Implementation of the revert function.
     * This should create an object with a literal in it.
     */
    virtual Object revert() const 
    {
        // object for the ()
        // Object o_root(Token{Tkn_Object{}});
        // object for the literal
        // set the data of the appropriate object


		// auto& token = std::get<Tkn_Object>(o_root.token);
		// for (auto it : data)
		// {
		// 	Object o_data(Token{Tkn_Literal{it}});
		// 	token.value.push_back(o_data);
		// }
		
		// new api candidate

		Object o_root(Token{Tkn_Object{}});
		for (auto it : data)
			o_root.add(Object::fromString(it));
        return o_root;
    }
public:
    // some conveinence and debug functions
    MyObj()
    {}

    MyObj(const std::string& _data)
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
