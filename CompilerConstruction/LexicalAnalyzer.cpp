//dot operator

#include "LexicalAnalyzer.h"
#include "Token.h"
#include <iostream>
#include <conio.h>
#include <fstream>
#include <stdio.h>

#include <regex>
#include<vector>
using namespace std;
struct WL {
	string word;
	unsigned line;
};
Token token;
vector<Token> tokenlist;
vector<string> word;
vector<int> line_number;
std::ofstream outfile("output.txt");
vector<WL> wordLine;

LexicalAnalyzer::LexicalAnalyzer(std::vector<string> fileInput)
{
	this->input = fileInput;
}


LexicalAnalyzer::~LexicalAnalyzer()
{
}

bool pushTempStr(string &temp, unsigned Line)
{
	if (temp != "")
	{
		wordLine.push_back({ temp, Line+1 });
		temp = "";
		return true;
	}
	return false;
}

void LexicalAnalyzer::tokenizer_() {
	

	int a = +1;
	//variable declaration
	string 
		currentChar,
		nextChar, 
		temp,
		lineStr;
	
	unsigned 
		lineSize,
		noOfLines = input.size();

	bool 
		multiLineComment = false,
		isFloat = false;

	

	//logic implementation
	for (unsigned line = 0; line < noOfLines ; line++)
	{
		lineSize = input.at(line).size();
		lineStr = input.at(line);

		for (unsigned charIndex = 0; charIndex < lineSize; charIndex++)
		{
			currentChar = lineStr[charIndex];
			if (lineSize > charIndex + 1)
			{
				nextChar = lineStr[charIndex + 1];
			}
			else
			{
				nextChar = "";
			}

			if (multiLineComment) 
			{
				if (currentChar == "#" && nextChar == "#")
					multiLineComment = false;
				continue;
			}

		
			if (currentChar == "#")
			{
				pushTempStr(temp, line);
				if (nextChar == "#")
				{
					multiLineComment = true;
				}
				else
					break;
				continue;
			}
			
			if (isFloat)
			{
				int e = 1;
				temp += currentChar;

				while (lineStr[++charIndex] )
				{	
					if ((lineStr[charIndex] == 'e' || lineStr[charIndex] == 'E') && e)
					{

						e = 0;
						temp += lineStr[charIndex];
						if (lineSize > charIndex + 1)
						{
							if (lineStr[charIndex + 1] == '+' || lineStr[charIndex + 1] == '-')
							{
								temp += lineStr[++charIndex];
							}
							while (lineSize > (charIndex + 1))							//edit
							{
								if (isdigit(lineStr[++charIndex]))
									temp += lineStr[charIndex];
							}
						}
						currentChar = "";
					}
					else if (isalnum(lineStr[charIndex]))
					{
						temp += lineStr[charIndex];
						currentChar = "";
						e = 0;
					}
					else
					{
						pushTempStr(temp, line);
						temp = lineStr[charIndex];
						break;
					}
				}
				currentChar = "";
				isFloat = false;
			}
		
			if (currentChar == "'")
			{
				pushTempStr(temp, line);
				if (nextChar == "\\")
				{
					if(lineSize > charIndex+3)
					{
						wordLine.push_back({ lineStr.substr(charIndex, 4),line+1 });
						charIndex += 3;

					}
					else
					{
						wordLine.push_back({ lineStr.substr(charIndex, (charIndex+5)-lineSize),line+1 });
						charIndex += (charIndex + 4) - lineSize;

					}
					
				}
				else
				{
					if (lineSize > charIndex + 2)
					{
						wordLine.push_back({ lineStr.substr(charIndex, 3),line+1 });
						charIndex += 2;
					}
					else
					{
						wordLine.push_back({ lineStr.substr(charIndex, (charIndex + 4) - lineSize),line+1 });
						charIndex += (charIndex + 3) - lineSize;
					}
					
					
				}
				continue;
			}
			

			if (currentChar == "\"")
			{
				pushTempStr(temp, line);
				temp = currentChar;
				while (++charIndex < lineSize)
				{
					if (lineStr[charIndex] == '\"')
					{
						temp += lineStr[charIndex];
						pushTempStr(temp, line);
						break;
					}
					else if (lineStr[charIndex] == '\\')
					{
						temp += lineStr[charIndex];
						if (++charIndex < lineSize)
						{
							temp += lineStr[charIndex];
							continue;
						}
					}
					else {
						temp += lineStr[charIndex];
					}
				/*	temp += lineStr[charIndex];
					if (lineStr[charIndex] + "" != "\\")
					{
						if (++charIndex < lineSize)
						{
							temp += lineStr[charIndex];
							continue;
						}
						break;
					}
					if (lineStr[charIndex] + "" != "\"")
					{
						temp += lineStr[charIndex];
						break;
					}*/
				}
				pushTempStr(temp, line);
				continue;
			}


			if (isPunctuator(currentChar))
			{
				pushTempStr(temp, line);
				wordLine.push_back({currentChar,line+1});
				continue;
			}
			
			if (currentChar == ".")
			{
				if (!temp.empty())
				{
					//if (isdigit(temp.back()))
					if (regex_match(temp, regex("[+-]?[0-9]+")))
					{
						if (isdigit(nextChar[0]))//1.1
						{
							temp += currentChar;
							isFloat = true;
						}
						else                      //1.a
						{
							pushTempStr(temp, line);
							pushTempStr(currentChar, line);
						}

					}
					else
					{
						pushTempStr(temp, line);
						if (isdigit(nextChar[0]))//a.1
						{
							temp += currentChar;
							isFloat = true;
						}
						else                      //a.a
						{
							pushTempStr(currentChar, line);
						}

					}

				}
				else {
					if (isdigit(nextChar[0]))
					{
						temp = currentChar;
						isFloat = true;
					}
					else
					{
						pushTempStr(currentChar, line);
					}
				}
				continue;
			}

			if (isOperator_(currentChar))
			{
				pushTempStr(temp, line);
				if (isOperator_(currentChar + nextChar))
				{
					wordLine.push_back({ currentChar + nextChar, line+1 });
					++charIndex;
					
				}
				else if ((currentChar == "+" || currentChar == "-") && isdigit(nextChar[0]))
				{
					if(temp == "")
						temp += currentChar;
				
				}
				else {
					wordLine.push_back({ currentChar, line+1 });
				}
				continue;
			}

			if (currentChar == " ")
			{
				pushTempStr(temp, line);
				continue;
			}

		
			temp += currentChar;
		}
		pushTempStr(temp, line);

	}
	for (unsigned i = 0; i < wordLine.size(); i++)  //printing word list on console
		std::cout << wordLine.at(i).word << endl;

	//cout << "======================" << endl;

}

void LexicalAnalyzer::tokenizer()
{
	int line_num;
	string current_char, next_char, temp = "", op;
	bool multilineComment = false, isFloat=false;//edit
	for (unsigned i = 0; i < input.size(); i++) //for reading each line
	{
		line_num = i + 1;
		this->line = this->input.at(i);
		for (unsigned j = 0; j < this->line.size(); j++)   //for reading each char of line i
		{
			current_char = this->line[j];
			next_char = this->line[j + 1];
			
			if (multilineComment)//edit 
			{
				if (current_char == "#" && next_char == "#")
					multilineComment = false;
				continue;

			}
			//for single line comment
			if (current_char == "#" && next_char != "#")//edit
				break;

			//for multiline comment
			if (current_char == "#")//edit
			{
				multilineComment = true;
				j += 1;
				continue;
			}

			//for single letter character eg. 'n'
			if (current_char == "\'" && next_char == "\\")
			{
				if (this->line[j + 2] && this->line[j + 3])
				{
					if (temp != "")
					{
						word.push_back(temp);
						line_number.push_back(line_num);
					}
					temp = current_char + next_char + this->line[j + 2] + this->line[j + 3];
					word.push_back(temp);
					line_number.push_back(line_num);
					temp = "";
					j += 3;//edit
					continue;
				}
			}

			//for 2 letter character eg.'\n'
			if (current_char == "\'")
			{
				if (this->line[j + 2])
				{
					if (temp != "")
					{
						word.push_back(temp);
						line_number.push_back(line_num);
					}
					temp = current_char + next_char + this->line[j + 2];
					word.push_back(temp);
					line_number.push_back(line_num);
					temp = "";
					j += 2;//edit
					continue;
				}
			}

			//for string
			if (current_char == "\"")
			{
				if (temp != "")
				{
					word.push_back(temp);
					line_number.push_back(line_num);
					temp = "";
				}
				while (this->line[j])
				{
					if (this->line[j] != '\\' && this->line[j + 1] == '\"')
					{
						temp += this->line[j++];
						temp += this->line[j++];
						break;
					}
					else
						temp += this->line[j++];
				}
				word.push_back(temp);
				line_number.push_back(line_num);
				temp = "";
				continue;
			}

			if (isPunctuator(current_char))//edit
			{
				word.push_back(current_char);
				line_number.push_back(line_num);
				continue;
			}
			if (isOperator(current_char) != "false")//edit  ,j
			{
				string diChar = current_char + next_char;
				if (isOperator(diChar) != "false")  //,j
				{
					word.push_back(diChar);
					j++;
				}
				else
					word.push_back(current_char);
				line_number.push_back(line_num);
				continue;
			}
			if (current_char == " ")//edit
			{
				if (temp != "")
				{
					word.push_back(current_char);
					line_number.push_back(line_num);
					temp = "";

				}
				continue;
			}

			//for 2 character operators eg. ++ -- >= etc
			if (j + 2 <= this->line.size())
				op = next_char + this->line[j + 2];
			else op = next_char + " ";

			//storing each char in temp
			temp += current_char;

			if (next_char == " " || isOperator(op) != "false" || isOperator(next_char) != "false" || isPunctuator(next_char))
			{
				//edit start
				string str;
				str= this->line[j + 2];
				if (next_char == "." && (temp == "" || isInt(temp)) && isInt(str))
				{
					temp += next_char;
					temp += str;
					j++;
					

					isFloat = true;
					continue;
				}
				if (!isFloat)
				{
					word.push_back(temp);                  //saving temp
					line_number.push_back(line_num);       //saving line_number
					temp = "";                             //clearing temp
					isFloat = true;
				}
				//edit end
				if (temp != "")
				{
					word.push_back(temp);                  //saving temp
					line_number.push_back(line_num);       //saving line_number
					temp = "";                             //clearing temp
				}

				if (isOperator(op) != "false")
				{
					j += 2;
					word.push_back(op);  //saving 2 char operator
					line_number.push_back(line_num);
				}
				else if (isOperator(next_char) != "false" || isPunctuator(next_char))
				{
					j++;
					word.push_back(next_char);  //saving single char operator or punctuator
					line_number.push_back(line_num);
				}
				if (j + 1 <= this->line.size() && this->line[j + 1] == ' ')   //ignoring spaces
					j++;
			}
		}
		if (temp != "")
		{
			//if temp is not null then saving temp before going to next line
			word.push_back(temp);
			line_number.push_back(line_num);
			temp = "";
		}
	}
	for (unsigned i = 0; i < word.size(); i++)  //printing word list on console
		std::cout << word.at(i) << endl;
}



void LexicalAnalyzer::classifier()
{
	
	
	for (unsigned i = 0; i < wordLine.size(); i++)
	{
		if (isID_(wordLine.at(i).word, wordLine.at(i).line) != "false")
		{
			if (isKeyword(wordLine.at(i).word) != "false")
			{
				if (isKeyword(wordLine.at(i).word) == wordLine.at(i).word)
					token = Token(isKeyword(wordLine.at(i).word), "", wordLine.at(i).line);
				else
					token = Token(isKeyword(wordLine.at(i).word), wordLine.at(i).word, wordLine.at(i).line);
			} else
			      token = Token("ID", wordLine.at(i).word, wordLine.at(i).line);
		}
		else if (isInt(wordLine.at(i).word))
			token = Token("int_const", wordLine.at(i).word, wordLine.at(i).line);
		else if (isFloat(wordLine.at(i).word))
			token = Token("float_const", wordLine.at(i).word, wordLine.at(i).line);
		else if (isChar(wordLine.at(i).word))
		{
			string w = wordLine.at(i).word.substr(1, wordLine.at(i).word.size() - 2);
			token = Token("char_const", w, wordLine.at(i).line);
		}
		else if (isString(wordLine.at(i).word))
		{
			string w = wordLine.at(i).word.substr(1, wordLine.at(i).word.size() - 2);
			token = Token("string_const", w, wordLine.at(i).line);
		}
		else if (isPunctuator(wordLine.at(i).word))
			token = Token(wordLine.at(i).word, "", wordLine.at(i).line);
		else if (isOperator(wordLine.at(i).word) != "false")
		{
			if (isOperator(wordLine.at(i).word) == wordLine.at(i).word)
				token = Token(wordLine.at(i).word, "", wordLine.at(i).line);
			else
				token = Token(isOperator(wordLine.at(i).word), wordLine.at(i).word, wordLine.at(i).line);
		}
		else
			token = Token("Invalid Token", wordLine.at(i).word, wordLine.at(i).line);

		outfile << "(" << token.getClassPart() << ", " << token.getValuePart() << ", " << token.getLineNo() << ")" << std::endl;
		tokenlist.push_back(token);
	}
}

string LexicalAnalyzer::isID_(string &word, int num)
{
	
	if (word[0] == '_')
	{

		if (word[1] == '_')
		{
			word = word.substr(2);
			token = Token("accessModifier", "private", num);
		
		}
		else {
			word = word.substr(1);
			token = Token("accessModifier", "protected", num);
			
		}
		outfile << "(" << token.getClassPart() << ", " << token.getValuePart() << ", " << token.getLineNo() << ")" << std::endl;
		tokenlist.push_back(token);
	}
	if (word[0] == '@')
	{
		token = Token("static", "@", num);
		outfile << "(" << token.getClassPart() << ", " << token.getValuePart() << ", " << token.getLineNo() << ")" << std::endl;
		tokenlist.push_back(token);
		if (word[1] == '$')
		{
			word = word.substr(2);
			token = Token("final", "$", num);
			outfile << "(" << token.getClassPart() << ", " << token.getValuePart() << ", " << token.getLineNo() << ")" << std::endl;
			tokenlist.push_back(token);
		}
		else {
			word = word.substr(1);
		}
	}
	else if (word[0] == '$')
	{
		token = Token("final", "$", num);
		outfile << "(" << token.getClassPart() << ", " << token.getValuePart() << ", " << token.getLineNo() << ")" << std::endl;
		tokenlist.push_back(token);
		if (word[1] == '$')
		{
			word = word.substr(2);
			token = Token("static", "@", num);
			outfile << "(" << token.getClassPart() << ", " << token.getValuePart() << ", " << token.getLineNo() << ")" << std::endl;
			tokenlist.push_back(token);
		}
		else {
			word = word.substr(1);
		}
	}
	
	if (regex_match(word, regex("[A-Za-z][A-Za-z0-9_]*")))
		return word;
	
	return "false";
}
string LexicalAnalyzer::isID(string word, int num)
{
	string w = "";
	regex b("[A-Za-z][A-Za-z0-9_]*");
	if (word[0] == '_' && word[1] == '_')
	{
		w = word.substr(2, word.size() - 1);
		token = Token("accessmodifier", "private", num);
	}
	else if (word[0] == '_' && word[1] != '_')
	{
		w = word.substr(1, word.size() - 1);
		token = Token("accessmodifier", "protected", num);
	}
	else if (word[0] == '@')
	{
		w = word.substr(1, word.size() - 1);
		token = Token("static", "", num);
	}
	else if (word[0] == '$')
	{
		w = word.substr(1, word.size() - 1);
		token = Token("final", "", num);
	}
	else
	{
		if (regex_match(word, b))
			return word;
	}
	if (w != "")
	{
		if (isKeyword(w) == "false")
		{
			if (regex_match(w, b))
			{
				outfile << "(" << token.getClassPart() << ", " << token.getValuePart() << ", " << token.getLineNo() << ")" << std::endl;
				tokenlist.push_back(token);
				return w;
			}
		}
	}
	return "false";
}
// <=======================================INTEGER===============================================>

bool LexicalAnalyzer::isInt(string word)
{
	regex b("[+-]?[0-9]+");
	if (regex_match(word, b))
		return true;
	return false;
}

// <=========================================FLOAT===============================================>

bool LexicalAnalyzer::isFloat(string word)
{
	regex b("[+-]?[0-9]*\\.[0-9]+([Ee][+-]?[0-9]+)?");
	if (regex_match(word, b))
		return true;
	return false;
}

// <=====================================CHARACTER===============================================>

bool LexicalAnalyzer::isChar(string word)
{
	//regex b("^'([^'\\\\])'$");
	regex b("'(\\\\([\\\\'tn0rb])|[a-zA-Z0-9 ])'");
	//regex b("\'(\\(\"|[rbtn0]|\'|\\)|[rbtn0]|\\S)\'");
	if (regex_match(word, b))
		return true;
	return false;
}

// <========================================STRING===============================================>

bool LexicalAnalyzer::isString(string word)
{
	
	regex b("\"([^\\\\\"]|\\\\[rbtn0\"\\\\])*\"");
	//regex b("\"(\\\\([\\\\'tn0rb0-9])|[a-zA-Z0-9|.)*\"");
	//regex b("\"(\\(\"|[rbtn0]|\'|\\)|[rbtn0]|\\S)*\"");
	if (regex_match(word, b))
		return true;
	return false;
}

// <====================================PUNCTUATOR===============================================>

bool LexicalAnalyzer::isPunctuator(string word)
{
	vector<string> punc = { ",", ":", "(", ")", "{", "}", "[", "]"};
	for (unsigned i = 0; i < punc.size(); i++)
		if(punc.at(i)==word)
			return true;
	return false;
}

// <=====================================OPERATOR===============================================>

string LexicalAnalyzer::isOperator(string word)
{
	if (word == "!" || word == "~")
		return "not";
	else if (word == "/" || word == "%")
		return "DM";
	else if (word == "==" || word == "!=" || word == "<=" || word == ">=" || word == "<" || word == ">")
		return "RelOp";
	else if (word == "+=" || word == "-=" || word == "*=" || word == "/=" || word == "%=")
		return "AsOp";
	else if (word == "->" || word == ".")
		return "AcOp";
	else if (word == "+" || word == "-")
		return "PM";
	else if (word == "*" || word == "|" || word == "&" || word == "=" || word == "^" || word == ":" || word == "?" )
		return word;
	return "false";
}



bool LexicalAnalyzer::isOperator_(string word)
{
	
	vector<string> operators = { "!" , "~" ,  "/", "%", "==", "!=",  "<=" , ">=", "<" , ">" ,  "+=", "-=", "*=" , "/=", "%=","->", ".", "*",  "|", "&", "=","^",":", "?","+" ,"-" };
	return (find(operators.begin(), operators.end(), word) != operators.end());
}

// <=======================================KEYWORD===============================================>

string LexicalAnalyzer::isKeyword(string word)
{
	if (word == "int" || word == "float" || word == "char" || word == "boolean")
		return "DT";
	else if (word == "True" || word == "1" || word == "False" || word == "0")
		return "bool";
	else if (word == "self" || word == "this")
		return "self";

	vector<string> keywords = {"And", "assert", "break", "class", "continue", "def", "del",
		"elif", "else", "except", "finally", "for", "global", "if", "in", "lambda", "new",
		"None", "Not", "Or", "pass", "raise", "return", "self", "string", "super", "this",
		"try", "while"};

	for (unsigned i = 0; i < keywords.size(); i++)
		if (keywords.at(i) == word)
			return word;
	return "false";
}