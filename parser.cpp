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
#include "LiSON_base.h"

namespace lison
{
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
        if (iter->sym == Tokenizer::Sym_Character || iter->sym == Tokenizer::Sym_Numeric)
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

			// :keyword
			tmp = keyword();
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

	// ^:\p{1,*}[\w)$]
	Object Parser::keyword()
	{
		if (!accept(Tokenizer::Sym_Character,':'))
			return Object::error();

		std::stringstream ss;

		bool loop = true;

		while (loop)
		{
			if (expect(Tokenizer::Sym_Character,')')
				|| expect(Tokenizer::Sym_RightParen)
				|| expect(Tokenizer::Sym_Whitespace)
				|| iter == endIter)
			{
				loop = false;
			}
			else if (expect(Tokenizer::Sym_Character))
			{
				ss << character();
			}
		}

		return Object::fromKeyword(ss.str());
	}

}
