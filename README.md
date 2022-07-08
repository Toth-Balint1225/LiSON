# LiSON - LiSp Object Notation version 1.1
Tóth Bálint, University of Pannonia, 2022,

<toth.balint1225@gmail.com>

LiSON is a simple markup language that (at this point) can interpret single-quoted
strings and list of strings and/or other strings. This makes it kind of similar to
the LISP programming language.

LiSON.h is the one-file version of the LiSON library.
## About LiSON:
A LiSON object can be a string literal, integer literal, float literal,
or a list of other objects. The list syntax is similar to
the lisp language's parenthesis with whitespaces as object delimiters, and a string
is between single quotes. An integer is a number of any length and a float requires the presense
of the decimal point. Inside a list, the order of contents is fixed.

Some example objects:
- () = an empty list
- '' = an empty string
- 123 = an integer
- 3.14 = a float
- ('') = a list containing an empty string
- 'Hello' = a string containing the word Hello
- ('Hello' '' ) = a list containing a literal of Hello and an empty string
- ('Hello' ('World')) = a list containing the literal Hello and another list containing the literal World.

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
its type, that is a standard variant of the Tkn_Literal, Tkn_Object and Tkn_Error structures.
The Object class has some functions to easily access the inside data of the Object. These
are the expectLiteralData, expectObjectData and foreachObjectData. The functions starting with
expect return an optional of the literal's or the object's contents and the foreach can take a
function of type const Object& -> void. Adding an Object to another instance of object can be
done with the add function. The Object class also provides factory methods to create itself
from strings, LiSON implementations and even arbitrary objects with a conversion functon provided.
Other way of accessing the inner data is the overload pattern and std::visit() functions, seen
in the to_string() method. This is similar to the Rust programming language's match operator, and
works with type specific lambda functions that get matched to the actual value of the std::variant.
The LiSON interface does the conversion between Object and custom class.

The serialization process can be done with the Serializer class, and its pre-implemented
convenience operators.

## Examples:
### 1. parsing lison: the MyObj class contains one string, that can be represented as a single
   literal in lison. The following code implements the conversion between the 'data' literal
   and an instance of MyObj

```
#define LISON_IMPLEMENTATION
#include <iostream>
#include "LiSON.h"

class MyObj : public lison::LiSON
{
private:
	std::string data;
protected:
	virtual void interpret(const lison::Object& obj) override
	{
		data = obj.expectLiteralData().value_or("Error");
	}

	virtual lison::Object revert() const override
	{
		lison::Object o_data = lison::Object::fromString(data);
		return o_data;
	}
public:
	void print() const
	{
		std::cout << "value: " << data << std::endl;
	}
};

int main()
{
	MyObj myObj;
	"\'hello\'" >> myObj;
	myObj.print();
	std::string after;
	after << myObj;
	std::cout << "after: " << after << std::endl;
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

## Documentation
### Token
### LiSON
### Serializer
### Obect