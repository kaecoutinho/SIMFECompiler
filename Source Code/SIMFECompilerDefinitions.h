// SIMFECompilerDefinitions.h
// SIMFECompiler
// Created by Kaê Angeli Coutinho
// MIT license

// Included dependencies

#include "SIMFECompilerDependencies.h"

// Required constants

// Preprocessor constants

#ifndef PROJECT_DIRECTORY_PATH || MAKEFILE_PATH
	#define MAKEFILE_REQUIRED_USAGE_CRITICAL_ERROR true
#else
	#define MAKEFILE_REQUIRED_USAGE_CRITICAL_ERROR false
#endif

// Compiler constants

#define COMPILER_VERSION 1.2
#define OUTPUT_PRECISION 1

// Input treatment constants

#define VALID_INPUT 0
#define REQUIRED_ARGUMENTS_NUMBER 2
#define SIM_FILE_EXTENSION ".sim"
#define SIM_PATH_ARGUMENT_INDEX 1

// String constants

#define SINGLE_CHARACTER_LENGTH 1
#define FIRST_CHARACTER 0
#define WHITE_SPACE_CHARACTER ' '
#define NEWLINE_CHARACTER '\n'
#define TABULATION_CHARACTER '\t'
#define EMPTY_CHARACTER '\0'
#define COMMENT_STARTED '{'
#define COMMENT_ENDED '}'
#define EXTRA_NEWLINE_SEQUENCE "\n\n"
#define EQUAL_STRINGS 0
#define EMPTY_STRING ""

// Debug constansts

#define BLACK_TEXT_DEBUG_COLOR 30
#define RED_TEXT_DEBUG_COLOR 31
#define GREEN_TEXT_DEBUG_COLOR 32
#define BLUE_TEXT_DEBUG_COLOR 34
#define BLACK_BACKGROUND_DEBUG_COLOR 40
#define WHITE_BACKGROUND_DEBUG_COLOR 47
#define CUSTOM_OUTPUT_START "\e["
#define BOLD "1"
#define UNDERLINE "4"
#define CUSTOM_OUTPUT_CONTINUE "m"
#define SEPARATOR ";"
#define CUSTOM_OUTPUT_END "\e[0m"

// Recognizers constants

#define IDENTIFIERS_RECOGNIZER_PARTIAL_PATH "/Recognizers/SIMFEIdentifiersRecognizer.json"
#define KEYWORDS_RECOGNIZER_PARTIAL_PATH "/Recognizers/SIMFEKeywordsRecognizer.json"
#define LITERALS_RECOGNIZER_PARTIAL_PATH "/Recognizers/SIMFELiteralsRecognizer.json"
#define NUMBERS_RECOGNIZER_PARTIAL_PATH "/Recognizers/SIMFENumbersRecognizer.json"
#define PUNCTUATIONS_RECOGNIZER_PARTIAL_PATH "/Recognizers/SIMFEPunctuationsRecognizer.json"
#define RELATIONAL_LOGICAL_OPERATORS_RECOGNIZER_PARTIAL_PATH "/Recognizers/SIMFERelationalLogicalOperatorsRecognizer.json"

// Grammar constants

#define LL1_GRAMMAR_PARTIAL_PATH "/Grammar/SIMLL1Grammar.json"
#define LL1_GRAMMAR_DELIMITER "$"
#define LL1_GRAMMAR_EPSILON "EPSILON"
#define SA_AND "&&"
#define SA_EQUAL "="
#define SA_LESS_THAN "<"
#define SA_UNEQUAL "<>"
#define SA_LESS_EQUAL "<="
#define SA_GREATER_THAN ">"
#define SA_GREATER_EQUAL ">="
#define SA_OR "||"
#define SA_PLUS "+"
#define SA_MINUS "-"
#define SA_SEMICOLON ";"
#define SA_COLON ":"
#define SA_ASSIGNMENT ":="
#define SA_DOT "."
#define SA_LEFT_PARENTHESES "("
#define SA_RIGHT_PARENTHESES ")"
#define SA_INCREMENTER ":+"
#define SA_DECREMENTER ":-"
#define SA_COMMA ","
#define SA_QUOTE "\""
#define FIXED_ATTRIBUTE_NOT_FOUND -1

// Grammar rules (non-terminal symbols) constants

#define GR_PROGRAM "PROGRAM"
#define GR_BLOCK "BLOCK"
#define GR_BLOCK_EXTENSION "BLOCK_EXTENSION"
#define GR_BLOCK_SECOND_EXTENSION "BLOCK_SECOND_EXTENSION"
#define GR_VARIABLES_DECLARATION_STAGE "VARIABLES_DECLARATION_STAGE"
#define GR_VARIABLES_DECLARATION_STAGE_AUX "VARIABLES_DECLARATION_STAGE_AUX"
#define GR_VARIABLES_DECLARATION_STAGE_AUX_EXTENSION "VARIABLES_DECLARATION_STAGE_AUX_EXTENSION"
#define GR_VARIABLES_DECLARATION "VARIABLES_DECLARATION"
#define GR_VARIABLES_DECLARATION_EXTENSION "VARIABLES_DECLARATION_EXTENSION"
#define GR_TYPE "TYPE"
#define GR_SUB_ROUTINES_DECLARATION_STAGE "SUB-ROUTINES_DECLARATION_STAGE"
#define GR_SUB_ROUTINES_DECLARATION_STAGE_EXTENSION "SUB-ROUTINES_DECLARATION_STAGE_EXTENSION"
#define GR_PROCEDURE_DECLARATION "PROCEDURE_DECLARATION"
#define GR_FUNCTION_DECLARATION "FUNCTION_DECLARATION"
#define GR_COMMANDS "COMMANDS"
#define GR_COMMANDS_AUX "COMMANDS_AUX"
#define GR_COMMANDS_AUX_EXTENSION "COMMANDS_AUX_EXTENSION"
#define GR_COMMANDS_AUX_SECOND_EXTENSION "COMMANDS_AUX_SECOND_EXTENSION"
#define GR_COMMAND "COMMAND"
#define GR_COMMAND_EXTENSION "COMMAND_EXTENSION"
#define GR_PROCEDURE_CALL_OR_ATTRIBUTION_COMMAND "PROCEDURE_CALL_OR_ATTRIBUTION_COMMAND"
#define GR_ATTRIBUTION_COMMAND "ATTRIBUTION_COMMAND"
#define GR_ATTRIBUTION_TYPE "ATTRIBUTION_TYPE"
#define GR_CONDITIONAL_COMMAND "CONDITIONAL_COMMAND"
#define GR_CONDITIONAL_COMMAND_EXTENSION "CONDITIONAL_COMMAND_EXTENSION"
#define GR_WHILE_COMMAND "WHILE_COMMAND"
#define GR_READ_COMMAND "READ_COMMAND"
#define GR_WRITE_COMMAND "WRITE_COMMAND"
#define GR_WRITE_COMMAND_EXTENSION "WRITE_COMMAND_EXTENSION"
#define GR_EXPRESSION "EXPRESSION"
#define GR_EXPRESSION_EXTENSION "EXPRESSION_EXTENSION"
#define GR_RELATIONAL_LOGICAL_OPERATOR "RELATIONAL_LOGICAL_OPERATOR"
#define GR_SIMPLE_EXPRESSION "SIMPLE_EXPRESSION"
#define GR_FACTOR "FACTOR"
#define GR_VARIABLE "VARIABLE"
#define GR_FUNCTION_RETURN "FUNCTION_RETURN"
#define GR_FUNCTION_RETURN_EXTENSION "FUNCTION_RETURN_EXTENSION"
#define GR_IDENTIFIER "IDENTIFIER"
#define GR_IDENTIFIER_EXTENSION "IDENTIFIER_EXTENSION"
#define GR_IDENTIFIER_SECOND_EXTENSION "IDENTIFIER_SECOND_EXTENSION"
#define GR_NUMBER "NUMBER"
#define GR_NUMBER_EXTENSION "NUMBER_EXTENSION"
#define GR_DIGIT "DIGIT"
#define GR_LETTER "LETTER"

// Custom types

// Enumerated type that represents token's classes
typedef enum tokenType
{
	TT_PROGRAM,
	TT_VAR,
	TT_INTEGER,
	TT_BOOLEAN,
	TT_PROCEDURE,
	TT_FUNCTION,
	TT_BEGIN,
	TT_END,
	TT_IF,
	TT_THEN,
	TT_ELSE,
	TT_END_IF,
	TT_WHILE,
	TT_END_WHILE,
	TT_DO,
	TT_READ,
	TT_WRITE,
	TT_RETURN,
	TT_TRUE,
	TT_FALSE,
	TT_NOT,
	TT_RELATIONAL_LOGICAL_OPERATOR,
	TT_ARITHMETIC_OPERATOR,
	TT_ASSIGNMENT_OPERATOR,
	TT_IDENTIFIER,
	TT_LITERAL,
	TT_NUMBER,
	TT_PUNCTUATION
}tokenType;

// Enumerated type that represents all of the keywords recognizer's final states
typedef enum keywordsRecognizerFinalStates
{
	FS_BOOLEAN = 8,
	FS_WHILE = 16,
	FS_THEN = 19,
	FS_WRITE = 25,
	FS_BEGIN = 31,
	FS_INTEGER = 36,
	FS_FUNCTION = 42,
	FS_DO = 45,
	FS_END = 47,
	FS_READ = 51,
	FS_IF = 53,
	FS_ELSE = 56,
	FS_VAR = 59,
	FS_PROGRAM = 67,
	FS_PROCEDURE = 76,
	FS_TRUE = 85,
	FS_FALSE = 88,
	FS_RETURN = 95,
	FS_NOT = 98,
	FS_END_IF = 100,
	FS_END_WHILE = 108
}keywordsRecognizerFinalStates;

// Enumerated type that represents all of the relational logical operators recognizer's final states
typedef enum relationalLogicalOperatorsRecognizerFinalStates
{
	FS_AND = 1,
	FS_EQUAL = 2,
	FS_LESS_THAN = 3,
	FS_UNEQUAL = 4,
	FS_LESS_EQUAL = 5,
	FS_GREATER_THAN = 6,
	FS_GREATER_EQUAL = 7,
	FS_OR = 9,
	FS_PLUS = 10,
	FS_MINUS = 11
}relationalLogicalOperatorsRecognizerFinalStates;

// Enumerated type that represents all of the punctuations recognizer's final states
typedef enum punctuationsRecognizerFinalStates
{
	FS_SEMICOLON = 1,
	FS_COLON = 2,
	FS_ASSIGNMENT = 3,
	FS_DOT = 4,
	FS_LEFT_PARENTHESES = 5,
	FS_RIGHT_PARENTHESES = 6,
	FS_INCREMENTER = 7,
	FS_DECREMENTER = 8,
	FS_COMMA = 9,
	FS_QUOTE = 10
}punctuationsRecognizerFinalStates;

// Enumerated type that represents all of the possible compiler errors
typedef enum errorType
{
	UNKNOWN_ERROR,
	TOO_MUCH_ARGUMENTS_ERROR,
	MAKEFILE_NOT_USED_ERROR,
	RECOGNIZERS_MODEL_FILES_OPENING_ERROR,
	LEXICAL_ERROR,
	SYNTATICAL_ERROR,
	GRAMMAR_FILE_OPENING_ERROR,
	BAD_GRAMMAR_FILE,
	SEMANTICAL_ERROR,
	MISSING_SIM_EXTENSION_ERROR,
	INVALID_SIM_FILE_ERROR
}errorType;

// Enumerated type that represents all of the symbol categories
typedef enum symbolCategory
{
	SC_VAR,
	SC_PROGRAM,
	SC_PROCEDURE,
	SC_FUNCTION
}symbolCategory;

// Enumerated type that represents all of the symbol types
typedef enum
{
	ST_INTEGER,
	ST_BOOLEAN,
	ST_NONE
}symbolType;

// Represents a lexeme containing its contents and corresponding line number
typedef struct lexeme lexeme;
struct lexeme
{
	string contents;
	int lineNumber;
};

// Represents a token containing its type, corresponding line number, optional and extra attribute
typedef struct token token;
struct token
{
	tokenType type;
	int lineNumber;
	string attribute;
	string extra;
};

// Represents a pseudo token containing its attribute, corresponding line number and original attribute (from token)
typedef struct pseudoToken pseudoToken;
struct pseudoToken
{
	string attribute;
	int lineNumber;
	string original;
};

// Represents an inner map inside a map structure
typedef map<string,string> innerMap;

// Represents a symbol containing its category, type, number, name, value and scope
typedef struct symbol symbol;
struct symbol
{
	symbolCategory category;
	symbolType type;
	int number;
	string name;
	string scope;
	string value;
};

// Represents an inner vector inside a stack structure
typedef vector<token> innerVector;

// Represents an inner stack inside a vector structure
typedef stack<innerVector> innerStack;
