// SIMFECompilerFunctions.h
// SIMFECompiler
// Created by Kaê Angeli Coutinho and Ricardo Oliete Ogata
// MIT license

// Included dependencies

#include "SIMFECompilerDefinitions.h"

// Function prototypes

// Compiler functions

bool compileSIMSourceCode(int argumentsCount, char ** arguments);
void handleError(errorType type, string extraMessage = EMPTY_STRING);
void showHelp();
void showOutput(bool failed, string fileName);
vector<token> lexicalAnalyzeSIMSourceCode(vector<lexeme> & lexemes, string fileName);
bool syntaticalAnalyzeSIMSourceCode(vector<token> & tokens, string fileName);
int isInputValid(int argumentsCount);

// SIM file functions

string getSIMFilePathFromInput(int argumentsCount, char ** arguments);
ifstream * readSIMFile(string SIMFilePath);
void rewindSIMFile(ifstream * SIMFile);

// SIM source code functions

string getSIMSourceCode(ifstream * SIMFile, bool trimSourceCode = true, bool closeAfterDone = true, bool deleteAfterDone = true);
void trimCommentsFromSIMSourceCode(string & SIMSourceCode);
string trimCommentsFromSIMSourceCodeToString(string SIMSourceCode);
void trimUnneededCharactersFromSIMSourceCode(string & SIMSourceCode);
string trimUnneededCharactersFromSIMSourceCodeToString(string SIMSourceCode);

// Lexeme functions

vector<lexeme> getLexemesFromSIMSourceCode(string & SIMSourceCode, ifstream * SIMFile, bool closeAfterDone = true, bool deleteAfterDone = true);
bool isLexemeCharacterValid(char character);
int doesLexemeNeedsSplit(char currentChar, char nextChar);
bool isCharacterSingleSplittable(char character);
bool isCharacterDoubleSplittable(char character);
void addLexemeIntoVector(string & lexemeBuffer, vector<lexeme> & lexemes, bool trimLexeme);
lexeme createLexeme(string contents, int lineNumber = -1);
void resolvePropertiesForLexemes(vector<lexeme> & lexemes, ifstream * SIMFile);
void trimLexemes(vector<lexeme> & lexemes);
string lexemesToString(vector<lexeme> lexemes);

// Token functions

token createToken(tokenType type, int lineNumber = -1, string attribute = EMPTY_STRING);
string tokensToString(vector<token> tokens, bool prettyPrint = true);
string tokenTypeToString(tokenType type);
int findFixedAttibuteNumberForToken(tokenType type, string attributeName);
string findFixedAttibuteNameForToken(tokenType type, int attributeNumber);

// Pseudo token functions

pseudoToken createPseudoToken(string attribute = EMPTY_STRING, int lineNumber = -1, string original = EMPTY_STRING);

// Grammar and syntatical analysis functions

map<string,innerMap> getll1GrammarMapFromGrammarFile(ifstream & ll1GrammarFile);
queue<pseudoToken> getInputQueueFromTokens(vector<token> & tokens);
stack<string> getSymbolsStack();
bool isSymbolNonterminal(string symbol);
vector<string> getSymbolsFromRule(string rule);

// Debug functions

void debugToConsole(string message, int textColor = RED_TEXT_DEBUG_COLOR, int backgroundColor = WHITE_BACKGROUND_DEBUG_COLOR);