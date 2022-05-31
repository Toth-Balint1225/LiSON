# LiSON - LiSp Object Notation version 1.0
Tóth Bálint, University of Pannonia, 2022

LiSON is a simple markup language that (at this point) can interpret single-quotet
strings and list of strings and/or other strings. This makes it kind of similar to
the LISP programming language.

LiSON.h is the one-file version of the LiSON library.

## Contents:
The LiSON.h contains the header part and the implementation part of the LiSON library,
so it compiles into the main executable.
Everything is in the lison namespace, the most important ones being the Object, the
LiSON interface and the Serializer classes.

## How to use?
To use the "library" user must #define LISON_IMPLEMENTATION before the include in any
translation unit.

To create a mapping of a class and its LiSON representation, user must implement the
LiSON interface for aforementioned class. The Object class has a token representing
its type (can be Tkn_Literal or Tkn_Object) and the data in the respective member
(str or obj). The LiSON interface does the conversion between Object and custom class.

The serialization process can be done with the Serializer class, and its pre-implemented
convenience operators.

## Examples:
### 1. parsing lison: the MyObj class contains one string, that can be represented as a single
   literal in lison. The following code implements the conversion between the 'data' literal
   and an instance of MyObj

```
// the include so that it actually works
#define LISON_IMPLEMENTATION
#include <LiSON.h>

class MyObj : public lison::LiSON
{
private:
    std::string data;
protected:
    virtual void interpret(const lison::Object& obj) override
    {
        if (obj.token != lison::Object::Tkn_Literal)
        return;
        data = obj.str;
    }

    virtual Object revert() const override
    {
        lison::Object o_data;
        o_data.token = lison::Object::Tkn_Literal;
        o_data.str = data;
        return o_data;
    }
};
```

### 2. using the operators: the MyObj class inherits default implementations for the >> operator,

   that can be used to invoke the interpreter.
#### 2.a: convert a lison string to MyObj:

```
MyObj my_obj;
"\'hello\'" >> my_obj;
```

#### 2.b: convert MyObj to lison string:

```
std::string lison;
lison << my_obj;
```

### 3. using the serializer: the serializer has a constructor with the filename associated
   with it, but the >> operator can set it on the fly.

```
lison::Serializer serializer("myfile.lison"); // create a serializer
serializer >> my_obj; // get the file contents and parse into the my_obj
"other.lison" >> serializer << my_obj;  // change the file to other.lison and serialize my_obj into it
```