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
        // check if the object is valid
        if (obj.token != Object::Tkn_Object)
        {
            data = "ERROR";
            return;
        }
        // the first element of the object's list is that we're interested in
        Object o_data = obj.obj.front();
        if (o_data.token != Object::Tkn_Literal)
        {
            data = "ERROR";
            return;
        }

        // setting the object's data
        data = o_data.str;
    }

    /**
     * Implementation of the revert function.
     * This should create an object with a literal in it.
     */
    virtual Object revert() const 
    {
        // object for the ()
        Object o_root;
        // object for the literal
        Object o_data;
        // set the token type
        o_root.token = Object::Tkn_Object;
        o_data.token = Object::Tkn_Literal;
        // set the data of the appropriate object
        o_data.str = data;
        o_root.obj.push_back(o_data);
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