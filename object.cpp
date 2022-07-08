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

}
