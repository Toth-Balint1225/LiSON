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
//     std::string Object::to_string() const
//     {
//         std::stringstream ss;
//         switch (token)
//         {
//             case Object::Tkn_Literal:
//             {
//                 ss << "'";
//                 ss << str;
//                 ss << "'";
//                 break;
//             }
//             case Object::Tkn_Object:
//             {
//                 ss << "( ";
//                 for (auto o : obj)
//                 {
//                     ss << o.to_string();
//                 }
//                 ss << ")";
//                 break;
//             }
//             default:
//             {
//                 ss <<"()";
//             }
//         }
//         ss << " ";
//         return ss.str();
//     }

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
			}
		};
		std::visit(visitor, token);
		ss << " ";
		return ss.str();
	}
}
