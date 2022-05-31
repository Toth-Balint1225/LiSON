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

/**
 * LiSON - LiSp Object Notation version 1.0
 * Tóth Bálint, University of Pannonia
 *
 * LiSON is a simple markup language that (at this point) can interpret single-quotet
 * strings and list of strings and/or other strings. This makes it kind of similar to
 * the LISP programming language.
 * 
 * This is the one-file version of the LiSON library.
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
 * its type (can be Tkn_Literal or Tkn_Object) and the data in the respective member
 * (str or obj). The LiSON interface does the conversion between Object and custom class.
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
 * #include <LiSON.h>
 *
 * class MyObj : public lison::LiSON
 * {
 * private:
 *   std::string data;
 * protected:
 *   virtual void interpret(const lison::Object& obj) override
 *   {
 *     if (obj.token != lison::Object::Tkn_Literal)
 *       return;
 *     data = obj.str;
 *   }
 *
 *   virtual Object revert() const override
 *   {
 *     lison::Object o_data;
 *     o_data.token = lison::Object::Tkn_Literal;
 *     o_data.str = data;
 *     return o_data;
 *   }
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
    struct Object
    {
        enum Token
        {
            Tkn_Literal,
            Tkn_Object,
            Tkn_Error,
        };
        Token token;
        // second best thing to a union
        std::string str;
        std::list<Object> obj;

        Object() = default;
        ~Object() = default;
        std::string to_string() const;
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

        bool accept(Tokenizer::Symbol req);
        char character();
        Object literal();
        Object object();
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
    std::string Object::to_string() const
    {
        std::stringstream ss;
        switch (token)
        {
            case Object::Tkn_Literal:
            {
                ss << "'";
                ss << str;
                ss << "'";
                break;
            }
            case Object::Tkn_Object:
            {
                ss << "( ";
                for (auto o : obj)
                {
                    ss << o.to_string();
                }
                ss << ")";
                break;
            }
            default:
            {
                ss <<"()";
            }
        }
        ss << " ";
        return ss.str();
    }

    // tokenizer
    std::list<Tokenizer::SymbolObject> Tokenizer::tokenize(const std::string& src)
    {
        std::list<SymbolObject> symbolStream;
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
    bool Parser::accept(Tokenizer::Symbol req)
    {
        if (iter->sym == req)
        {
            ++iter;
            return true;
        }
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
                Object o;
                o.token = Object::Tkn_Literal;
                o.str = ss.str();
                return o;
            }
        }
        Object o;
        o.token = Object::Tkn_Error;
        return o;
    }

    Object Parser::object()
    {
        Object o;
        while (accept(Tokenizer::Sym_Whitespace));
        if (accept(Tokenizer::Sym_LeftParen))
        {
            o.token = Object::Tkn_Object;
            // yank the whitespaces
            while (accept(Tokenizer::Sym_Whitespace));
            if (accept(Tokenizer::Sym_RightParen))
            {
                return o;
            }
            else 
            {
                Object first_obj = object();
                if (first_obj.token == Object::Tkn_Error)
                {
                    o.token = Object::Tkn_Error;
                    return o;
                }
                o.obj.push_back(first_obj);
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
                        o.token = Object::Tkn_Error;
                        return o;
                    }
                    if (accept(Tokenizer::Sym_RightParen))
                    {
                        list = false;
                        break;
                    }
                    Object list_obj = object();
                    if (list_obj.token == Object::Tkn_Error)
                    {
                        o.token = Object::Tkn_Error;
                        return o;
                    }
                    o.obj.push_back(list_obj);
                }
                return o;
            }
        }
        else
        {
            Object l = literal();   
            if (l.token != Object::Tkn_Error)
            {
                o.token = Object::Tkn_Literal;
                o.str = l.str;
                return o;
            }
            return l;
        }
        o.token = Object::Tkn_Error;
        return o;
    }

    Object Parser::parse(std::list<Tokenizer::SymbolObject> symbolStream)
    {
        iter = symbolStream.begin();
        return object();
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
