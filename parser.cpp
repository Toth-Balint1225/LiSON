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
}