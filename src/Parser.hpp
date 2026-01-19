/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: razaccar <razaccar@student.42lausanne.ch>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 06:02:27 by razaccar          #+#    #+#             */
/*   Updated: 2026/01/18 23:50:22 by razaccar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
# define PARSER_HPP

#include <string>
#include <vector>

typedef struct Message {
    bool                        hasPrefix;
    std::string                 prefix;
    std::string                 command;
    std::vector<std::string>    params;

    Message() : hasPrefix(false) {}
} Message; 

class Parser {
    public:
        static bool parseMsg(std::string const& line,
                             Message& out,
                             std::string* err);

    private:
        static bool         readToken(std::string const& s, size_t& i, std::string& token);
        static std::string  toUpper(std::string const& s);
        static void         skipSpaces(std::string const& s, size_t& i);
        
};

#endif
