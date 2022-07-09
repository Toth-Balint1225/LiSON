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
#ifndef LISON_BASE_H
#define LISON_BASE_H

#include <cstdlib>
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
 * TODO: Exception on parsing error
 */
namespace lison
{
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

	struct Tkn_Keyword
	{
		std::string value;
	};

	struct Object;
	struct Tkn_Object
	{
		std::list<Object> value;
	};

	struct Tkn_Error
	{};

	using Token = std::variant<Tkn_Literal,Tkn_Integer,Tkn_Float,Tkn_Keyword,Tkn_Object,Tkn_Error>;

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
		static Object fromKeyword(const std::string& str);
		static Object error();
		static Object empty();

		template <class T>
		static Object fromObject(
			T t,
			std::function<Object(const T&)> f);

		// injection in lists
		void foreachObjectData(
			std::function<void(const Object&)> f) const;

		// adding for lists 
		void add(const Object& obj);

		// maybe getting
		std::optional<std::string> expectLiteralData() const;
		std::optional<std::list<Object>> expectObjectData() const;
		std::optional<long int> expectIntData() const;
		std::optional<double> expectFloatData() const;
	};

    /**
     * string -> set of symbols
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
     */
    class Parser
    {
    private:
        std::list<Tokenizer::SymbolObject>::iterator iter;
        std::list<Tokenizer::SymbolObject>::iterator endIter;

        bool accept(Tokenizer::Symbol req, char c = 0);
		bool expect(Tokenizer::Symbol req, char c = 0);
        char character();
		char numeric();
        Object literal();
        Object object();
		Object integer();
		Object floating();
    public:
        Object parse(std::list<Tokenizer::SymbolObject> symbolStream);
    };

    /**
     * Abstract
     */
    class LiSON
    {
		friend class Object;
    protected:
        /**
         * Interface methods
         */
        virtual void interpret(const Object& obj) = 0;
        virtual Object revert() const = 0;
    public:
        virtual ~LiSON() = default;
        /**
         * Default implemented methods, shouldn't override
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

#endif // LISON_BASE_H
