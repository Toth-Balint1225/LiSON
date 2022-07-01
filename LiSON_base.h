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

#include <string>
#include <sstream>
#include <list>
#include <fstream>
#include <variant>

/**
 * LiSON - LiSp Object Notation
 * Tóth Bálint, University of Pannonia
 * TODO: Exception on parsing error
 * TODO: change all std::string to std::wstring and char to wchar_t
 */
namespace lison
{
	struct Tkn_Literal
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

	using Token = std::variant<Tkn_Literal,Tkn_Object,Tkn_Error>;

	template <class... Ts>
	struct overload : Ts...
	{
		using Ts::operator ()...;
	};

	template <class... Ts>
	overload(Ts...) -> overload<Ts...>;

	struct Object
	{
		Token token;
		Object(const Token& t); 
		Object(const Object& other);
		~Object() = default;
		std::string to_string() const;
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

        bool accept(Tokenizer::Symbol req);
        char character();
        Object literal();
        Object object();
    public:
        Object parse(std::list<Tokenizer::SymbolObject> symbolStream);
    };

    /**
     * Abstract
     */
    class LiSON
    {
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
