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
bool lexicalAnalyzeSIMSourceCode(vector<lexeme> & lexemes, vector<token> & tokens, string fileName);
bool syntaticalAnalyzeSIMSourceCode(vector<token> & tokens, string fileName);
bool semanticalAnalyzeSIMSourceCode(vector<token> & tokens, string fileName);
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
string lexemesToString(vector<lexeme> & lexemes);

// Token functions

token createToken(tokenType type, int lineNumber = -1, string attribute = EMPTY_STRING, string extra = EMPTY_STRING);
string tokensToString(vector<token> & tokens, bool prettyPrint = true);
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

// Symbol, symbol table and semantical analysis functions

symbol createSymbol(symbolCategory category, symbolType type, int number = -1, string name = EMPTY_STRING, string scope = EMPTY_STRING, string value = EMPTY_STRING);
string symbolCategoryToString(symbolCategory category);
string symbolTypeToString(symbolType type);
vector<symbol> getSymbolTableFromTokens(vector<token> & tokens);
bool doesSymbolExist(vector<symbol> & symbolTable, int number);
symbol getSymbolFromSymbolTable(vector<symbol> & symbolTable, int number);
bool removeSymbolFromSymbolTable(vector<symbol> & symbolTable, int number);
bool updateSymbolToSymbolTable(vector<symbol> & symbolTable, symbol instance);
string symbolTableToString(vector<symbol> & symbolTable, bool prettyPrint = true);
stack<innerVector> getTypeCheckStackForToken(vector<token> & tokens, int tokenIndex, stack<token> & scopesStack);
vector<innerStack> getTypeCheckListFromTokens(vector<token> & tokens);

// Debug functions

void debugToConsole(string message, int textColor = RED_TEXT_DEBUG_COLOR, int backgroundColor = WHITE_BACKGROUND_DEBUG_COLOR);