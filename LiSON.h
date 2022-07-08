/**
 *     Copyright (C) 2022  Tóth Bálint
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
// interface guard
#ifndef LISON_H
#define LISON_H

#include <string>
#include <sstream>
#include <list>
#include <fstream>
#include <variant>
#include <functional>
#include <optional>

/**
 * LiSON - LiSp Object Notation
 * Tóth Bálint, University of Pannonia
 *
 * LiSON is a simple markup language that (at this point) can interpret single-quotet
 * strings and list of strings and/or other strings. This makes it kind of similar to
 * the LISP programming language.
 * 
 * This is the one-file version of the LiSON library.
 *
 * About LiSON:
 * A LiSON object can be a string literal or a list of other objects. The list syntax is
 * the lisp language's parenthesis with whitespaces as object delimiters, and a string
 * is between single quotes. Inside a list, the order of contents is fix.
 *
 * Some example objects:
 * () = an empty list
 * '' = an empty string
 * ('') = a list containing an empty string
 * 'Hello' = a string containing the word Hello
 * ('Hello' '' ) = a list containing a literal of Hello and an empty string
 * ('Hello' ('World')) = a list containing the literal Hello and another list containing the literal World.
 *
 * Contents:
 * This file contains the header part and the implementation part of the LiSON library,
 * so it compiles into the main executable.
 * Everything is in the lison namespace, the most important ones being the Object, the
 * LiSON interface and the Serializer classes.
 *
 * How to use?
 * To use the "library" user must #define LISON_IMPLEMENTATION before the include in any
 * translation unit.
 *
 * To create a mapping of a class and its LiSON representation, user must implement the
 * LiSON interface for aforementioned class. The Object class has a token representing
 * its type, that is a standard variant of the Tkn_Literal, Tkn_Object and Tkn_Error structures.
 * The Object class has some functions to easily access the inside data of the Object. These
 * are the expectLiteralData, expectObjectData and foreachObjectData. The functions starting with
 * expect return an optional of the literal's or the object's contents and the foreach can take a
 * function of type const Object& -> void. Adding an Object to another instance of object can be
 * done with the add function. The Object class also provides factory methods to create itself
 * from strings, LiSON implementations and even arbitrary objects with a conversion functon provided.
 * Other way of accessing the inner data is the overload pattern and std::visit() functions, seen
 * in the to_string() method. This is similar to the Rust programming language's match operator, and
 * works with type specific lambda functions that get matched to the actual value of the std::variant.
 * The LiSON interface does the conversion between Object and custom class.
 *
 * The serialization process can be done with the Serializer class, and its pre-implemented
 * convenience operators.
 *
 * Examples:
 * 1. parsing lison: the MyObj class contains one string, that can be represented as a single
 *    literal in lison. The following code implements the conversion between the 'data' literal
 *    and an instance of MyObj
 *
 * // the include so that it actually works
 * #define LISON_IMPLEMENTATION
 * #include <iostream>
 * #include "LiSON.h"
 * 
 * class MyObj : public lison::LiSON
 * {
 * private:
 * 	std::string data;
 * protected:
 * 	virtual void interpret(const lison::Object& obj) override
 * 	{
 * 		data = obj.expectLiteralData().value_or("Error");
 * 	}
 * 
 * 	virtual lison::Object revert() const override
 * 	{
 * 		lison::Object o_data = lison::Object::fromString(data);
 * 		return o_data;
 * 	}
 * public:
 * 	void print() const
 * 	{
 * 		std::cout << "value: " << data << std::endl;
 * 	}
 * };
 * 
 * int main()
 * {
 * 	MyObj myObj;
 * 	"\'hello\'" >> myObj;
 * 	myObj.print();
 * 	std::string after;
 * 	after << myObj;
 * 	std::cout << "after: " << after << std::endl;
 * };
 *
 * 2. using the operators: the MyObj class inherits default implementations for the >> operator,
 *    that can be used to invoke the interpreter.
 * 2.a: convert a lison string to MyObj:
 * MyObj my_obj;
 * "\'hello\'" >> my_obj;
 *
 * 2.b: convert MyObj to lison string:
 * std::string lison;
 * lison << my_obj;
 *
 * 3. using the serializer: the serializer has a constructor with the filename associated
 *    with it, but the >> operator can set it on the fly.
 * 3.a deserializing a file into a LiSON object
 * lison::Serializer serializer("myfile.lison"); // create a serializer
 * serializer >> my_obj; // get the file contents and parse into the my_obj
 * "other.lison" >> serializer << my_obj;  // change the file to other.lison and serialize my_obj into it
 */

/**
 * Namespace lison contains all types and default implementations.
 */
namespace lison
{
    /**
     * Structure to hold the actual data and token
     */

	struct Tkn_Literal
	{
		std::string value;
	};

	struct Tkn_Integer
	{
		long int value;
	};

	struct Tkn_Float
	{
		double value;
	};

	struct Object;
	struct Tkn_Object
	{
		std::list<Object> value;
	};

	struct Tkn_Error
	{};

	using Token = std::variant<Tkn_Literal,Tkn_Integer,Tkn_Float,Tkn_Object,Tkn_Error>;

	template <class... Ts>
	struct overload : Ts...
	{
		using Ts::operator ()...;
	};

	template <class... Ts>
	overload(Ts...) -> overload<Ts...>;

	class LiSON;
	struct Object
	{
		Token token;
		Object(const Token& t); 
		Object(const Object& other);
		~Object() = default;
		std::string to_string() const;

		// the factory API
		static Object fromString(const std::string& str);
		static Object fromLiSON(const LiSON& lison);
		static Object fromInt(long int i);
		static Object fromFloat(double f);

		template <class T>
		static Object fromObject(
			T t,
			std::function<Object(const T&)> f);

		// injection
		void foreachObjectData(
			std::function<void(const Object&)> f) const;

		// adding
		void add(const Object& obj);

		// maybe getting
		std::optional<std::string> expectLiteralData() const;
		std::optional<std::list<Object>> expectObjectData() const;
		std::optional<long int> expectIntData() const;
		std::optional<double> expectFloatData() const;
	};

    /**
     * string -> set of symbols
     * conversion class
     */
    class Tokenizer
    {
    friend class Parser;
    friend class LiSON;
    private:
        enum Symbol
        {
            Sym_NIL,
            Sym_Quote,
            Sym_LeftParen,
            Sym_RightParen,
            Sym_Character,
            Sym_Whitespace,
			Sym_Numeric,
        };

        struct SymbolObject
        {
            Symbol sym;
            char character;
        };

        SymbolObject actual;
    public:
        Tokenizer() = default;
        std::list<SymbolObject> tokenize(const std::string& src);
    };

    /**
     * set of symbols -> Object (abstract syntax tree)
     * conversion class
     */
    class Parser
    {
    private:
        std::list<Tokenizer::SymbolObject>::iterator iter;
        std::list<Tokenizer::SymbolObject>::iterator endIter;

        bool accept(Tokenizer::Symbol req, char c = 0);
		bool expect(Tokenizer::Symbol req, char c = 0);
        char character();
        Object literal();
        Object object();
		Object integer();
		Object floating();
    public:
        Object parse(std::list<Tokenizer::SymbolObject> symbolStream);
    };

    /**
     * Abstract class, that also serves as the interface for the lison objects.
     * The serialize and deserialize methods have default implementation to work
     * with the serializer class.
     */
    class LiSON
    {
		friend class Object;
    protected:
        /**
         * Interface methods.
         */
        virtual void interpret(const Object& obj) = 0;
        virtual Object revert() const = 0;
    public:
        virtual ~LiSON() = default;
        /**
         * Default implemented methods, shouldn't be overriden
         */
        void deserialize(const std::string& src);
        std::string serialize() const;
    };

    /**
     * file -> object
     */ 
    class Serializer
    {
    private:
        std::string filename = "";
    public:
        Serializer() = default;
        Serializer(const std::string& _filename);
        void set_file(const std::string& _filename);
        std::string read() const;
        void write(const std::string& source) const;
    };

    /**
     * Epic clean-code features
     */
    // wtring <-> lison
    LiSON& operator >>(const std::string& src, LiSON& lison);
    std::string& operator <<(std::string& res, const LiSON& lison);

    // serializer <-> lison
    LiSON& operator >>(const Serializer& serializer, LiSON& lison);
    const Serializer& operator <<(const Serializer& serializer, const LiSON& lison);

    // string -> serializer
    Serializer& operator >>(const std::string& filename, Serializer& serializer);
}

#endif // LISON_H

/**
 * LiSON default implementations.
 * To use this, you must #define LISON_IMPLEMENTATION
 */
#ifdef LISON_IMPLEMENTATION
#ifndef _LISON_IMPLEMENTATION
namespace lison
{
    // object
	Object::Object(const Token& t)
		: token(t)
	{}

	Object::Object(const Object& other)
		: token(other.token)
	{}
	
    std::string Object::to_string() const
	{
		// the new variant visitor magic
		std::stringstream ss;
		auto visitor = overload
		{
			[&ss](const Tkn_Literal& literal)
			{
                ss << "'";
                ss << literal.value;
                ss << "'";
			},
			[&ss](const Tkn_Object& object)
			{
                ss << "( ";
                for (auto o : object.value)
                {
                    ss << o.to_string();
                }
                ss << ")";
			},
			[&ss](const Tkn_Error& error)
			{
				ss << "ERROR";
			},
			[&ss](const Tkn_Integer& integer)
			{
				ss << integer.value;
			},
			[&ss](const Tkn_Float& floating)
			{
				std::stringstream innerSS;
				innerSS << floating.value;
				std::string tmp = innerSS.str();
				if (std::find(tmp.begin(),
							  tmp.end(),
							  '.') == tmp.end())
				{
					innerSS << ".0";
				}
				ss << innerSS.str();
			}
		};
		std::visit(visitor, token);
		ss << " ";
		return ss.str();
	}

	Object Object::fromString(const std::string& str)
	{
		return Object(Token{Tkn_Literal{str}});
	}

	Object Object::fromLiSON(const LiSON& lison)
	{
		return lison.revert();
	}

	Object Object::fromInt(long int i)
	{
		return Object(Token{Tkn_Integer{i}});
	}

	Object Object::fromFloat(double f)
	{
		return Object(Token{Tkn_Float{f}});
	}

	template <class T>
	Object Object::fromObject(T t,
							  std::function<Object(const T&)> f)
	{
		return f(t);
	}

	void Object::foreachObjectData(
		std::function<void(const Object&)> f) const
	{
		if (!std::holds_alternative<Tkn_Object>(token))
			return;
		auto& t = std::get<Tkn_Object>(token);
		for (auto it : t.value)
		{
			f(it);
		}
	}

	void Object::add(const Object& obj)
	{
		if (!std::holds_alternative<Tkn_Object>(token))
			return;
		auto& t = std::get<Tkn_Object>(token);
		t.value.push_back(obj);
	}

	std::optional<std::string> Object::expectLiteralData() const
	{
		if (!std::holds_alternative<Tkn_Literal>(token))
			return {};
		auto& t = std::get<Tkn_Literal>(token);
		return {t.value};
	}

	std::optional<std::list<Object>> Object::expectObjectData() const
	{
		if (!std::holds_alternative<Tkn_Object>(token))
			return {};
		auto& t = std::get<Tkn_Object>(token);
		return {t.value};
	}

	std::optional<long int> Object::expectIntData() const
	{
		if (!std::holds_alternative<Tkn_Integer>(token))
			return {};
		auto& t = std::get<Tkn_Integer>(token);
		return {t.value};
	}
	
	std::optional<double> Object::expectFloatData() const
	{
		if (!std::holds_alternative<Tkn_Float>(token))
			return {};
		auto& t = std::get<Tkn_Float>(token);
		return {t.value};
	}

    // tokenizer
    std::list<Tokenizer::SymbolObject> Tokenizer::tokenize(const std::string& src)
    {
        std::list<SymbolObject> symbolStream;
		std::string numerics = "0123456789";
        for (unsigned i=0;i<src.length();i++)
        {
            char c = src[i];
            SymbolObject sym;
            if (c == '\'')
                sym.sym = Sym_Quote;
            else if (c == '(')
                sym.sym = Sym_LeftParen;
            else if (c == ')')
                sym.sym = Sym_RightParen;
            else if (c == '\t' || c == '\n' || c == ' ')
            {
                sym.sym = Sym_Whitespace;
            }
			else if (std::find(numerics.begin(),numerics.end(),c)
					 != numerics.end())
			{
				sym.sym = Sym_Numeric;
				sym.character = c;
			}
            else
            {
                sym.sym = Sym_Character;
                sym.character = c;
            }
            symbolStream.push_back(sym);
        }
        return symbolStream;
    }

    // parser

	// consume symbol, but don't use it
	// can be used to check if a special symbol is present
    bool Parser::accept(Tokenizer::Symbol req, char c)
    {
		bool res = false;
        if (iter->sym == req)
        {
			if (c == 0)
				res = true;
			else
			{
				if (iter->character == c)
					res = true;
				else
					res = false;
			}
        }
		if (res)
			++iter;
        return res;
    }

	bool Parser::expect(Tokenizer::Symbol req, char c)
	{
		if (iter->sym == req)
		{
			if (c == 0)
				return true;
			else
			{
				if (iter->character == c)
					return true;
				else
					return false;
			}
		}
		else
			return false;
	}

    char Parser::character()
    {
        if (iter->sym == Tokenizer::Sym_Character)
        {
            char c =  iter->character;
            ++iter;
            return c;
        }
        else
            return 0;
    }
	
	char Parser::numeric()
	{
        if (iter->sym == Tokenizer::Sym_Numeric)
        {
            char c =  iter->character;
            ++iter;
            return c;
        }
        else
            return 0;
	}

	// ^'[^']*'
    Object Parser::literal()
    {
        if (accept(Tokenizer::Sym_Quote))
        {
            std::stringstream ss;
            char c;
            bool whitespace;
            bool loop = true;
            while (loop)
            {
                whitespace = accept(Tokenizer::Sym_Whitespace);
                if (whitespace)
                {
                    ss << ' ';
                    continue;
                }
                c = character();
                if (c != 0)
                    ss << c;
                loop = c != 0 || whitespace;
            }
            if (accept(Tokenizer::Sym_Quote))
            {
                // Object o;
                // o.token = Object::Tkn_Literal;
                // o.str = ss.str();
				Object o(Token{Tkn_Literal{ss.str()}});
                return o;
            }
        }
        Object o(Token{Tkn_Error{}});
        // o.token = Object::Tkn_Error;
        return o;
    }

	/*
	  Object => [LeftParen] -> [String | [Whitespace -> Object] -> [RightParen]
	            |------------------------|
	 */

    Object Parser::object()
    {
		Object o(Token{Tkn_Object{}});
		auto& token = std::get<Tkn_Object>(o.token);
		// yank the whitespaces
        while (accept(Tokenizer::Sym_Whitespace));

		// outer if: looks for left paren, that means a new Object object
        if (accept(Tokenizer::Sym_LeftParen))
        {
            // yank the whitespaces
            while (accept(Tokenizer::Sym_Whitespace));

			// if it's an empty object, we return
            if (accept(Tokenizer::Sym_RightParen))
                return o;
			// the object has some contents
            else 
            {
				// get the first object in the object
				// at this point there must be at least one
                Object first_obj = object();

				// put them object in with a visitor
				if (std::holds_alternative<Tkn_Error>(first_obj.token))
				{
					o = Object(Token{Tkn_Error{}});
					return o;
				}

				token.value.push_back(first_obj);

                // optional objects
                bool list = true;
                while (list)
                {
                    if (!accept(Tokenizer::Sym_Whitespace))
                    {
                        if (accept(Tokenizer::Sym_RightParen))
						{
							list = false;
							break;
						}
                    }
                    if (accept(Tokenizer::Sym_RightParen))
                    {
                        list = false;
                        break;
                    }

					if (iter == endIter)
					{
						o = Object(Token{Tkn_Error{}});
						list = false;
						break;
					}
                    Object list_obj = object();
					token.value.push_back(list_obj);
                }
            }
        }
		// outer else:
		// happens if the object doesn't start with a left paren -> it's a literal (or later something else)
        else
        {
			// we try with the literal
            Object tmp = literal();
			if (!std::holds_alternative<Tkn_Error>(tmp.token))
			{
				o = tmp;
				return o;
			}
			// float
			tmp = floating();
			if (!std::holds_alternative<Tkn_Error>(tmp.token))
			{
				o = tmp;
				return o;
			}
			// int
			tmp = integer();
			if (!std::holds_alternative<Tkn_Error>(tmp.token))
			{
				o = tmp;
				return o;
			}

			// if it errors, than we propagate the error
			if (std::holds_alternative<Tkn_Error>(tmp.token))
			{
				o = Object(Token{Tkn_Error{}});
				return o;
			}
        }
		return o;
    }

    Object Parser::parse(std::list<Tokenizer::SymbolObject> symbolStream)
    {
        iter = symbolStream.begin();
		endIter = symbolStream.end();
        return object();
    }

	// ^\d*[\s$)]
	Object Parser::integer()
	{
		// if it doesn't start with a number, it is not a number
		// -> fact.
		auto snapshot = iter;
		if (!expect(Tokenizer::Sym_Numeric))
			return Object(Token{Tkn_Error{}});
		// start by yanking numbers
		std::stringstream ss;
		bool loop = true; 

		// read in the first digit
		char first = numeric();
		ss << first;

		// look for more digits, anything not a decimal means it is not an int
		// the end is a whitespace (-> do not consume!)
		while (loop)
		{
			if (expect(Tokenizer::Sym_Whitespace)
				|| iter == endIter
				|| expect(Tokenizer::Sym_RightParen))
			{
				// the end is here
				loop = false;
				return Object::fromInt(std::stoi(ss.str()));
			}
			else if (expect(Tokenizer::Sym_Numeric))
			{
				char c = numeric();
				ss << c;
			}
			else
			{
				loop = false;
			}

		}
		
		iter = snapshot;
		return Object(Token{Tkn_Error{}});
	}
	
	
	// ^\d{1,*}\.\d{1,*}[\s$)]
	Object Parser::floating()
	{
		auto snapshot = iter;
		if (!expect(Tokenizer::Sym_Numeric))
			return Object(Token{Tkn_Error{}});
		std::stringstream ss;
		bool loop = true; 

		// read in the first digit
		char first = numeric();
		ss << first;

		// first part before the dot
		while (loop)
		{
			if (accept(Tokenizer::Sym_Character,'.'))
			{
				loop = false;
				ss << ".";
			}
			else if (expect(Tokenizer::Sym_Numeric))
				ss << numeric();
			else
			{
				iter = snapshot;
				return Object(Token{Tkn_Error{}});
			}
		}
		loop = true;

		// same as the integer
		while (loop)
		{
			if (expect(Tokenizer::Sym_Whitespace)
				|| iter == endIter
				|| expect(Tokenizer::Sym_RightParen))
			{
				// the end is here
				loop = false;
				double d = std::stod(ss.str());
				return Object::fromFloat(d);
			}
			else if (expect(Tokenizer::Sym_Numeric))
			{
				char c = numeric();
				ss << c;
			}
			else
			{
				loop = false;
			}

		}
		
		iter = snapshot;
		return Object(Token{Tkn_Error{}});
	}

    // lison
    void LiSON::deserialize(const std::string& src)
    {
        Tokenizer tokenizer;
        Parser parser;
        std::list<Tokenizer::SymbolObject> symbolStream = tokenizer.tokenize(src);
        Object o = parser.parse(symbolStream);
        interpret(o);
    }

    std::string LiSON::serialize() const
    {
        return revert().to_string();
    }

    // wstring <-> lison
    LiSON& operator >>(const std::string& src, LiSON& lison)
    {
        lison.deserialize(src);
        return lison;
    }

    std::string& operator <<(std::string& res, const LiSON& lison)
    {
        res =  lison.serialize();
        return res;
    }

    // serializer <-> lison
    LiSON& operator >>(const Serializer& serializer, LiSON& lison)
    {
        lison.deserialize(serializer.read());
        return lison;
    }

    const Serializer& operator <<(const Serializer& serializer, const LiSON& lison)
    {
        serializer.write(lison.serialize());
        return serializer;
    }

    // wstring -> serializer
    Serializer& operator >>(const std::string& filename, Serializer& serializer)
    {
        serializer.set_file(filename);
        return serializer;
    }

    // serializer
    Serializer::Serializer(const std::string& _filename)
        :filename(_filename)
    {}

    void Serializer::set_file(const std::string& _filename)
    {
        this->filename = _filename;
    }

    std::string Serializer::read() const
    {
        if (filename.empty())
            return "";
        std::ifstream file;
        file.open(filename,std::ios_base::in);
        if (!file.is_open())
            return "";
        std::stringstream ss;
        ss << file.rdbuf();
        file.close();
        return ss.str();
    }

    void Serializer::write(const std::string& source) const
    {
        if (filename.empty())
            return;
        std::ofstream file;
        file.open(filename,std::ios_base::out);
        if (!file.is_open())
            return;
        file << source;
        file.close();
    }
}
#define _LISON_IMPLEMENTATION
#endif // _LISON_IMPLEMENTATION
#endif
