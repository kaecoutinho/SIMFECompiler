// SIMFECompilerFunctions.cpp
// SIMFECompiler
// Created by Kaê Angeli Coutinho
// MIT license

// Included dependencies

#include "SIMFECompilerFunctions.h"

// Function bodies

// Compiler functions

// Compiles a SIM source code
bool compileSIMSourceCode(int argumentsCount, char ** arguments)
{
	bool failed = false, overrideOutput = false;
	int inputValidation = isInputValid(argumentsCount);
	string SIMFileName = EMPTY_STRING, SIMFilePath, SIMSourceCode;
	path * SIMFileNamePath = NULL;
	ifstream * SIMFile = NULL;
	vector<lexeme> lexemes;
	vector<token> tokens;
	try
	{
		if(inputValidation == VALID_INPUT)
		{
			SIMFilePath = getSIMFilePathFromInput(argumentsCount,arguments);
			SIMFileNamePath = new path(SIMFilePath);
			SIMFileName = SIMFileNamePath->filename().string();
			SIMFile = readSIMFile(SIMFilePath);
			SIMSourceCode = getSIMSourceCode(SIMFile,true,false,false);
			lexemes = getLexemesFromSIMSourceCode(SIMSourceCode,SIMFile);
			failed = !lexicalAnalyzeSIMSourceCode(lexemes,tokens,SIMFileName);
			if(!failed)
			{
				failed = !syntaticalAnalyzeSIMSourceCode(tokens,SIMFileName);
				if(!failed)
				{
					failed = !semanticalAnalyzeSIMSourceCode(tokens,SIMFileName);
				}
			}
			else
			{
				failed = true;
			}
		}
		else if(inputValidation < VALID_INPUT)
		{
			overrideOutput = true;
			showHelp();
		}
		else
		{
			handleError(TOO_MUCH_ARGUMENTS_ERROR);
		}
	}
	catch(const char * exception)
	{
		cout << exception;
		failed = true;
	}
	catch(const string exception)
	{
		cout << exception;
		failed = true;
	}
	if(!overrideOutput)
	{
		showOutput(failed,SIMFileName);
	}
	if(SIMFileNamePath != NULL)
	{
		delete SIMFileNamePath;
	}
	return ((failed) ? EXIT_FAILURE : EXIT_SUCCESS);
}

// Handles an error message
void handleError(errorType type, string extraMessage)
{
	ostringstream message;
	message << CUSTOM_OUTPUT_START << UNDERLINE << SEPARATOR << BOLD << SEPARATOR << RED_TEXT_DEBUG_COLOR << CUSTOM_OUTPUT_CONTINUE;
	switch(type)
	{
		case TOO_MUCH_ARGUMENTS_ERROR:
			message << "Too much arguments, expected only one: [SIM file path : string]" << CUSTOM_OUTPUT_END;
			break;
		case MAKEFILE_NOT_USED_ERROR:
			message << "SIMFECompiler must be generated through Makefile" << CUSTOM_OUTPUT_END;
			break;
		case RECOGNIZERS_MODEL_FILES_OPENING_ERROR:
			message << "Could not open the recognizers model files" << CUSTOM_OUTPUT_END;
			break;
		case LEXICAL_ERROR:
			message << "Lexical error" << CUSTOM_OUTPUT_END << CUSTOM_OUTPUT_START << BOLD << SEPARATOR << RED_TEXT_DEBUG_COLOR << CUSTOM_OUTPUT_CONTINUE << " - " << extraMessage << CUSTOM_OUTPUT_END;
			break;
		case SYNTATICAL_ERROR:
			message << "Syntatical error" << CUSTOM_OUTPUT_END << CUSTOM_OUTPUT_START << BOLD << SEPARATOR << RED_TEXT_DEBUG_COLOR << CUSTOM_OUTPUT_CONTINUE << " - " << extraMessage << CUSTOM_OUTPUT_END;
			break;
		case GRAMMAR_FILE_OPENING_ERROR:
			message << "Could not open the LL(1) grammar file" << CUSTOM_OUTPUT_END;
			break;
		case BAD_GRAMMAR_FILE:
			message << "Something is wrong with the LL(1) grammar file" << CUSTOM_OUTPUT_END;
			break;
		case SEMANTICAL_ERROR:
			message << "Semantical error" << CUSTOM_OUTPUT_END << CUSTOM_OUTPUT_START << BOLD << SEPARATOR << RED_TEXT_DEBUG_COLOR << CUSTOM_OUTPUT_CONTINUE << " - " << extraMessage << CUSTOM_OUTPUT_END;
			break;
		case MISSING_SIM_EXTENSION_ERROR:
			message << "SIM file must have .sim extension" << CUSTOM_OUTPUT_END;
			break;
		case INVALID_SIM_FILE_ERROR:
			message << "Invalid SIM file provided" << CUSTOM_OUTPUT_END;
			break;
		case UNKNOWN_ERROR:
		default:
			message << "Something happened and compilation had to stop" << CUSTOM_OUTPUT_END;
			break;
	}
	throw message.str();
}

// Shows the help message
void showHelp()
{
	cout.precision(OUTPUT_PRECISION);
	cout << "SIMFECompiler V" << fixed << COMPILER_VERSION << endl;
	cout << "Created by Kaê Angeli Coutinho and Ricardo Oliete Ogata" << endl;
	cout << "\n\tNumber of required arguments: 1" << endl;
	cout << "\n\tArguments" << endl;
	cout << "\n\t\t1 - SIM file path" << endl;
	cout << "\n\t\tIn case of no arguments, this help message will appear" << endl;
	cout << "\n\tOutput" << endl;
	cout << "\n\t\tReturns an ouput message whether the file was successfully compiled or not" << endl;
}

// Shows output message
void showOutput(bool failed, string fileName)
{
	if(!failed)
	{
		cout << CUSTOM_OUTPUT_START << BOLD << SEPARATOR << GREEN_TEXT_DEBUG_COLOR << CUSTOM_OUTPUT_CONTINUE << "Successfully compiled" << ((fileName.length() > 0) ? (" " + fileName) : EMPTY_STRING) << CUSTOM_OUTPUT_END;
		cout << CUSTOM_OUTPUT_START << BOLD << SEPARATOR << GREEN_TEXT_DEBUG_COLOR << CUSTOM_OUTPUT_CONTINUE << " - No errors were found" << CUSTOM_OUTPUT_END << endl;
	}
	else
	{
		cout << CUSTOM_OUTPUT_START << BOLD << SEPARATOR << RED_TEXT_DEBUG_COLOR << CUSTOM_OUTPUT_CONTINUE << " - Could not compile" << CUSTOM_OUTPUT_END << endl;
	}
}

// Does the lexical analysis on a SIM source code 
bool lexicalAnalyzeSIMSourceCode(vector<lexeme> & lexemes, vector<token> & tokens, string fileName)
{
	bool validAnalysis = true;
	lexeme unidentifiedLexeme;
	map<string,int> identifiersNumbers;
	int currentIdentifiersNumber = 0;
	DFA * identifiersRecognizer = NULL, * keywordsRecognizer = NULL, * literalsRecognizer = NULL, * numbersRecognizer = NULL, * punctuationsRecognizer = NULL, * relationalLogicalOperatosRecognizer = NULL;
	ifstream identifiersRecognizerModel, keywordsRecognizerModel, literalsRecognizerModel, numbersRecognizerModel, punctuationsRecognizerModel, relationalLogicalOperatosRecognizerModel;
	DFAValidationResult resultKeywordsRecognizer, resultRelationalLogicalOperatorsRecognizer, resultPunctuationsRecognizer, resultNumbersRecognizer, resultLiteralsRecognizer, resultIdentifiersRecognizer;
	
	// Checks if project was compiled through Makefile
	if(MAKEFILE_REQUIRED_USAGE_CRITICAL_ERROR)
	{
		handleError(MAKEFILE_NOT_USED_ERROR);
	}

	// Open the recognizers JSON models
	identifiersRecognizerModel.open(string(PROJECT_DIRECTORY_PATH).append(string(IDENTIFIERS_RECOGNIZER_PARTIAL_PATH)),ios::in);
	keywordsRecognizerModel.open(string(PROJECT_DIRECTORY_PATH).append(string(KEYWORDS_RECOGNIZER_PARTIAL_PATH)),ios::in);
	literalsRecognizerModel.open(string(PROJECT_DIRECTORY_PATH).append(string(LITERALS_RECOGNIZER_PARTIAL_PATH)),ios::in);
	numbersRecognizerModel.open(string(PROJECT_DIRECTORY_PATH).append(string(NUMBERS_RECOGNIZER_PARTIAL_PATH)),ios::in);
	punctuationsRecognizerModel.open(string(PROJECT_DIRECTORY_PATH).append(string(PUNCTUATIONS_RECOGNIZER_PARTIAL_PATH)),ios::in);
	relationalLogicalOperatosRecognizerModel.open(string(PROJECT_DIRECTORY_PATH).append(string(RELATIONAL_LOGICAL_OPERATORS_RECOGNIZER_PARTIAL_PATH)),ios::in);

	// Checks if files were properly opened
	if(identifiersRecognizerModel.bad() || keywordsRecognizerModel.bad() || literalsRecognizerModel.bad() || numbersRecognizerModel.bad() || punctuationsRecognizerModel.bad() || relationalLogicalOperatosRecognizerModel.bad())
	{
		handleError(RECOGNIZERS_MODEL_FILES_OPENING_ERROR);
	}

	// Instantiate the recognizers
	identifiersRecognizer = new DFA(identifiersRecognizerModel);
	keywordsRecognizer = new DFA(keywordsRecognizerModel);
	literalsRecognizer = new DFA(literalsRecognizerModel);
	numbersRecognizer = new DFA(numbersRecognizerModel);
	punctuationsRecognizer = new DFA(punctuationsRecognizerModel);
	relationalLogicalOperatosRecognizer = new DFA(relationalLogicalOperatosRecognizerModel);

	// Close the recognizers JSON models
	identifiersRecognizerModel.close();
	keywordsRecognizerModel.close();
	literalsRecognizerModel.close();
	numbersRecognizerModel.close();
	punctuationsRecognizerModel.close();
	relationalLogicalOperatosRecognizerModel.close();

	// Validate and fix wrong transitions of the recognizers
	identifiersRecognizer->preValidate();
	keywordsRecognizer->preValidate();
	literalsRecognizer->preValidate();
	numbersRecognizer->preValidate();
	punctuationsRecognizer->preValidate();
	relationalLogicalOperatosRecognizer->preValidate();

	// Tokens validation, waterfall aproach
	for(int lexemeIndex = 0; lexemeIndex < lexemes.size(); lexemeIndex++)
	{
		string currentLexeme = lexemes[lexemeIndex].contents;
		int currentLineNumber = lexemes[lexemeIndex].lineNumber;

		// Validations result
		resultKeywordsRecognizer = keywordsRecognizer->validate(currentLexeme);
		resultRelationalLogicalOperatorsRecognizer = relationalLogicalOperatosRecognizer->validate(currentLexeme);
		resultPunctuationsRecognizer = punctuationsRecognizer->validate(currentLexeme);
		resultNumbersRecognizer = numbersRecognizer->validate(currentLexeme);
		resultLiteralsRecognizer = literalsRecognizer->validate(currentLexeme);
		resultIdentifiersRecognizer = identifiersRecognizer->validate(currentLexeme);

		// Keyword found
		if(resultKeywordsRecognizer.valid)
		{
			int currentStateNumber = atoi(resultKeywordsRecognizer.currentState->getName().substr(1).c_str());
			tokenType type;

			// Finds out keyword type
			switch(currentStateNumber)
			{
				case FS_BOOLEAN:
					type = TT_BOOLEAN;
					break;
				case FS_WHILE:
					type = TT_WHILE;
					break;
				case FS_THEN:
					type = TT_THEN;
					break;
				case FS_WRITE:
					type = TT_WRITE;
					break;
				case FS_BEGIN:
					type = TT_BEGIN;
					break;
				case FS_INTEGER:
					type = TT_INTEGER;
					break;
				case FS_FUNCTION:
					type = TT_FUNCTION;
					break;
				case FS_DO:
					type = TT_DO;
					break;
				case FS_END:
					type = TT_END;
					break;
				case FS_READ:
					type = TT_READ;
					break;
				case FS_ELSE:
					type = TT_ELSE;
					break;
				case FS_VAR:
					type = TT_VAR;
					break;
				case FS_PROGRAM:
					type = TT_PROGRAM;
					break;
				case FS_PROCEDURE:
					type = TT_PROCEDURE;
					break;
				case FS_TRUE:
					type = TT_TRUE;
					break;
				case FS_FALSE:
					type = TT_FALSE;
					break;
				case FS_RETURN:
					type = TT_RETURN;
					break;
				case FS_IF:
					type = TT_IF;
					break;
				case FS_NOT:
					type = TT_NOT;
				case FS_END_IF:
					type = TT_END_IF;
					break;
				case FS_END_WHILE:
					type = TT_END_WHILE;
					break;
				default:
					validAnalysis = false;
					unidentifiedLexeme.contents = currentLexeme;
					unidentifiedLexeme.lineNumber = currentLineNumber;
					break;
			}
			tokens.push_back(createToken(type,currentLineNumber,currentLexeme));
		}

		// Relational logical operator found
		else if(resultRelationalLogicalOperatorsRecognizer.valid)
		{
			int currentStateNumber = atoi(resultRelationalLogicalOperatorsRecognizer.currentState->getName().substr(1).c_str());
			tokenType type;
			
			// Finds out relational logical operator type
			switch(currentStateNumber)
			{
				case FS_AND:
				case FS_EQUAL:
				case FS_LESS_THAN:
				case FS_UNEQUAL:
				case FS_LESS_EQUAL:
				case FS_GREATER_THAN:
				case FS_GREATER_EQUAL:
				case FS_OR:
					type = TT_RELATIONAL_LOGICAL_OPERATOR;
					break;
				case FS_PLUS:
				case FS_MINUS:
					type = TT_ARITHMETIC_OPERATOR;
					break;
				default:
					validAnalysis = false;
					unidentifiedLexeme.contents = currentLexeme;
					unidentifiedLexeme.lineNumber = currentLineNumber;
					break;
			}
			tokens.push_back(createToken(type,currentLineNumber,currentLexeme));
		}

		// Punctuation found
		else if(resultPunctuationsRecognizer.valid)
		{
			int currentStateNumber = atoi(resultPunctuationsRecognizer.currentState->getName().substr(1).c_str());
			tokenType type;
			
			// Finds out punctuation type
			switch(currentStateNumber)
			{
				case FS_SEMICOLON:
				case FS_COLON:
				case FS_COMMA:
				case FS_QUOTE:
				case FS_DOT:
				case FS_LEFT_PARENTHESES:
				case FS_RIGHT_PARENTHESES:
					type = TT_PUNCTUATION;
					break;
				case FS_ASSIGNMENT:
				case FS_INCREMENTER:
				case FS_DECREMENTER:
					type = TT_ASSIGNMENT_OPERATOR;
					break;
				default:
					validAnalysis = false;
					unidentifiedLexeme.contents = currentLexeme;
					unidentifiedLexeme.lineNumber = currentLineNumber;
					break;
			}
			tokens.push_back(createToken(type,currentLineNumber,currentLexeme));
		}

		// Number found
		else if(resultNumbersRecognizer.valid)
		{
			tokens.push_back(createToken(TT_NUMBER,currentLineNumber,currentLexeme));
		}

		// Literal found
		else if(resultLiteralsRecognizer.valid)
		{
			// Splits the into punctuations and literal
			erase_all(currentLexeme,SA_QUOTE);
			tokens.push_back(createToken(TT_PUNCTUATION,currentLineNumber,SA_QUOTE));
			tokens.push_back(createToken(TT_LITERAL,currentLineNumber,currentLexeme));
			tokens.push_back(createToken(TT_PUNCTUATION,currentLineNumber,SA_QUOTE));
		}

		// Identifier found
		else if(resultIdentifiersRecognizer.valid)
		{
			if(identifiersNumbers.find(currentLexeme) == identifiersNumbers.end())
			{
				identifiersNumbers[currentLexeme] = currentIdentifiersNumber++;
			}
			tokens.push_back(createToken(TT_IDENTIFIER,currentLineNumber,currentLexeme,to_string(identifiersNumbers[currentLexeme])));
		}

		// Error found
		else
		{
			validAnalysis = false;
			unidentifiedLexeme.contents = currentLexeme;
			unidentifiedLexeme.lineNumber = currentLineNumber;
			tokens.clear();
			break;
		}
	}

	// Reports lexical error if happened
	if(!validAnalysis)
	{
		handleError(LEXICAL_ERROR,(fileName + ":" + to_string(unidentifiedLexeme.lineNumber) + ": unidentified lexeme called '" + unidentifiedLexeme.contents + "'"));
	}

	// Checks if recognizers were properly allocated before deletion
	if(identifiersRecognizer != NULL && keywordsRecognizer != NULL && literalsRecognizer != NULL &&
		numbersRecognizer != NULL && punctuationsRecognizer != NULL && relationalLogicalOperatosRecognizer != NULL)
	{
			// Delete the recognizers
			delete identifiersRecognizer;
			delete keywordsRecognizer;
			delete literalsRecognizer;
			delete numbersRecognizer;
			delete punctuationsRecognizer;
			delete relationalLogicalOperatosRecognizer;
	}

	// Returns whether the analysis was successful or not
	return validAnalysis;
}

// Does the syntax analysis (using LL(1) top-down approach) on a SIM source code
bool syntaticalAnalyzeSIMSourceCode(vector<token> & tokens, string fileName)
{
	bool validAnalysis = true, stopAnalysis = false, overrideFlag = false;
	ifstream ll1GrammarFile;
	map<string,innerMap> ll1GrammarMap;
	queue<pseudoToken> inputQueue;
	stack<string> symbolsStack;
	string currentSymbol, currentInput;
	pseudoToken mistakenPseudoToken;

	// Opens the SIM's LL(1) grammar file
	ll1GrammarFile.open(string(PROJECT_DIRECTORY_PATH).append(string(LL1_GRAMMAR_PARTIAL_PATH)),ios::in);

	// Checks if file was properly opened
	if(ll1GrammarFile.bad())
	{
		handleError(GRAMMAR_FILE_OPENING_ERROR);
		stopAnalysis = true;
		validAnalysis = false;
		overrideFlag = true;
	}

	// Gets the full grammar map
	ll1GrammarMap = getll1GrammarMapFromGrammarFile(ll1GrammarFile);

	// Checks if LL(1) grammar file was properly read
	if(ll1GrammarMap.empty())
	{
		handleError(BAD_GRAMMAR_FILE);
		stopAnalysis = true;
		validAnalysis = false;
		overrideFlag = true;
	}

	// Gets the input queue
	inputQueue = getInputQueueFromTokens(tokens);

	// Gets the symbols stack
	symbolsStack = getSymbolsStack();

	// Analyses the tokens stream
	while(!stopAnalysis && !symbolsStack.empty() && !inputQueue.empty())
	{
		// Get the current symbol (X) and input (t) for the current analysis step
		currentSymbol = symbolsStack.top();
		currentInput = inputQueue.front().attribute;

		// Checks if X = $ and t = $, making the analysis right and done
		if(currentSymbol.compare(LL1_GRAMMAR_DELIMITER) == EQUAL_STRINGS && currentInput.compare(LL1_GRAMMAR_DELIMITER) == EQUAL_STRINGS)
		{
			stopAnalysis = true;
		}

		// Checks if X = t and t != $, making a match and proceding with the symbol stack and input queue
		else if(currentSymbol.compare(currentInput) == EQUAL_STRINGS && currentInput.compare(LL1_GRAMMAR_DELIMITER) != EQUAL_STRINGS)
		{
			symbolsStack.pop();
			inputQueue.pop();
		}

		// Checks if X is a nonterminal symbol
		else if(isSymbolNonterminal(currentSymbol))
		{
			// Checks if there's a rule for X and t (M(X,t))
			if(ll1GrammarMap[currentSymbol].find(currentInput) != ll1GrammarMap[currentSymbol].end())
			{
				symbolsStack.pop();
				vector<string> ruleSymbols = getSymbolsFromRule(ll1GrammarMap[currentSymbol][currentInput]);
				for(int symbolIndex = 0; symbolIndex < ruleSymbols.size(); symbolIndex++)
				{
					string symbolToPush = ruleSymbols[symbolIndex];
					if(symbolToPush.compare(LL1_GRAMMAR_EPSILON) != EQUAL_STRINGS)
					{
						symbolsStack.push(ruleSymbols[symbolIndex]);
					}
				}
			}

			// Error found
			else
			{
				mistakenPseudoToken = inputQueue.front();
				stopAnalysis = true;
				validAnalysis = false;
			}
		}

		// Error found
		else
		{
			mistakenPseudoToken = inputQueue.front();
			stopAnalysis = true;
			validAnalysis = false;
		}
	}

	// Reports syntatical error if happened
	if(!validAnalysis && !overrideFlag)
	{
		handleError(SYNTATICAL_ERROR,(fileName + ":" + to_string(mistakenPseudoToken.lineNumber) + ": invalid syntax structure"));
	}

	// Returns whether the analysis was successful or not
	return validAnalysis;
}

// Does the semantic analysis on a SIM source code
bool semanticalAnalyzeSIMSourceCode(vector<token> & tokens, string fileName)
{
	bool validAnalysis = true;
	int errorLineNumber;
	vector<symbol> symbolTable;
	vector<innerStack> typeCheckList;
	string errorMessage;

	// Gets the symbol table
	symbolTable = getSymbolTableFromTokens(tokens);

	// Checks if a unidentified identifier (not declared) was used
	for(int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++)
	{
		if(validAnalysis)
		{
			token currentToken = tokens[tokenIndex];
			if(currentToken.type == TT_IDENTIFIER)
			{
				// Unidentified identifier found
				if(!doesSymbolExist(symbolTable,stoi(currentToken.extra)))
				{
					validAnalysis = false;
					errorLineNumber = currentToken.lineNumber;
					errorMessage = "unidentified identifier called '" + currentToken.attribute + "'";
				}
			}
		}
		else
		{
			break;
		}
	}

	// Checks if a semantical error already happened
	if(validAnalysis)
	{
		// Gets the list of types to check
		typeCheckList = getTypeCheckListFromTokens(tokens);

		// Checks all expressions eligible for type check
		for(int typeCheckIndex = 0; typeCheckIndex < typeCheckList.size(); typeCheckIndex++)
		{
			bool firstExpression = true;
			stack<innerVector> currentTypeCheckStack = typeCheckList[typeCheckIndex];
			symbolType currentType, previousType, currentTokenType, previousTokenType;
			token currentToken, previousToken;
			while(!currentTypeCheckStack.empty() && validAnalysis)
			{
				vector<token> currentExpressionList = currentTypeCheckStack.top();

				// Checks if current expression has relational operators
				if(currentExpressionList.size() > 1)
				{
					for(int expressionIndex = 0; expressionIndex < currentExpressionList.size(); expressionIndex++)
					{
						currentToken = currentExpressionList[expressionIndex];
						switch(currentToken.type)
						{
							case TT_IDENTIFIER:
								currentTokenType = getSymbolFromSymbolTable(symbolTable,stoi(currentToken.extra)).type;
								break;
							case TT_NUMBER:
							case TT_ASSIGNMENT_OPERATOR:
								currentTokenType = ST_INTEGER;
								break;
							case TT_TRUE:
							case TT_FALSE:
							case TT_NOT:
								currentTokenType = ST_BOOLEAN;
								break;
						}
						if(expressionIndex == 0)
						{
							previousTokenType = currentTokenType;
						}

						// Unequal types found
						if(previousTokenType != currentTokenType)
						{
							validAnalysis = false;
							errorLineNumber = currentToken.lineNumber;
							errorMessage = "types are not equal";
							break;
						}
						previousTokenType = currentTokenType;
					}
					currentType = ST_BOOLEAN;
					if(firstExpression)
					{
						firstExpression = false;
						previousType = currentType;
					}
				}

				// Single expression found
				else
				{
					currentToken = currentExpressionList[0];
					switch(currentToken.type)
					{
						case TT_IDENTIFIER:
							currentTokenType = getSymbolFromSymbolTable(symbolTable,stoi(currentToken.extra)).type;
							break;
						case TT_NUMBER:
						case TT_ASSIGNMENT_OPERATOR:
							currentTokenType = ST_INTEGER;
							break;
						case TT_TRUE:
						case TT_FALSE:
						case TT_NOT:
							currentTokenType = ST_BOOLEAN;
							break;
					}
					currentType = currentTokenType;
					if(firstExpression)
					{
						firstExpression = false;
						previousType = currentType;
						previousToken = currentToken;
					}
				}

				// Unequal types found
				if(currentType != previousType)
				{
					validAnalysis = false;
					errorLineNumber = previousToken.lineNumber;
					errorMessage = "types are not equal";
				}
				previousType = currentType;
				previousToken = currentToken;
				currentTypeCheckStack.pop();
			}
		}
	}

	// Reports semantical error if happened
	if(!validAnalysis)
	{
		handleError(SEMANTICAL_ERROR,(fileName + ":" + to_string(errorLineNumber) + ": " + errorMessage));
	}

	// Returns whether the analysis was successful or not
	return validAnalysis;
}

// Checks if an input is valid and if so, tells the input length
int isInputValid(int argumentsCount)
{
	int result = VALID_INPUT;
	if(argumentsCount > REQUIRED_ARGUMENTS_NUMBER)
	{
		result = 1;
	}
	else if(argumentsCount < REQUIRED_ARGUMENTS_NUMBER)
	{
		result = -1;
	}
	return result;
}

// SIM file functions

// Gets the SIM file path from a input
string getSIMFilePathFromInput(int argumentsCount, char ** arguments)
{
	string simFilePath = arguments[SIM_PATH_ARGUMENT_INDEX];
	if(simFilePath.find(SIM_FILE_EXTENSION) != string::npos)
	{
    	return simFilePath;
	}
	else
	{
		handleError(MISSING_SIM_EXTENSION_ERROR);
	}
}

// Reads a SIM file
ifstream * readSIMFile(string SIMFilePath)
{
	ifstream * SIMFile = new ifstream(SIMFilePath,ios::in);
	if(SIMFile->is_open() && SIMFile->good())
	{
		return SIMFile;
	}
	else
	{
		handleError(INVALID_SIM_FILE_ERROR);
	}
}

// Rewinds a SIM file
void rewindSIMFile(ifstream * SIMFile)
{
	if(SIMFile->eof())
	{
		SIMFile->clear();
		SIMFile->seekg(FIRST_CHARACTER,ios::beg);
	}
}

// SIM source code functions

// Gets the SIM source code from a SIM file
string getSIMSourceCode(ifstream * SIMFile, bool trimSourceCode, bool closeAfterDone, bool deleteAfterDone)
{
	ostringstream buffer;
	string line;
	rewindSIMFile(SIMFile);
	while(getline((*SIMFile),line))
	{
		buffer << line << endl;
	}
	if(closeAfterDone)
	{
		SIMFile->close();
	}
	if(deleteAfterDone)
	{
		delete SIMFile;
	}
	return ((trimSourceCode) ? trimUnneededCharactersFromSIMSourceCodeToString(buffer.str()) : buffer.str());
}

// Trims all comments from a SIM source code
void trimCommentsFromSIMSourceCode(string & SIMSourceCode)
{
	bool insideCommentBlock = false;
	bool overrideFlag = false;
	for(int letterIndex = 0; letterIndex < SIMSourceCode.length(); letterIndex++)
	{
		if(SIMSourceCode[letterIndex] == COMMENT_STARTED)
		{
			insideCommentBlock = true;
		}
		else if(SIMSourceCode[letterIndex] == COMMENT_ENDED)
		{
			insideCommentBlock = false;
			overrideFlag = true;
		}
		if(insideCommentBlock || overrideFlag)
		{
			if(overrideFlag)
			{
				overrideFlag = false;
			}
			SIMSourceCode.erase(letterIndex--,SINGLE_CHARACTER_LENGTH);
		}
	}
}

// Trims all comments from a SIM source code
string trimCommentsFromSIMSourceCodeToString(string SIMSourceCode)
{
	bool insideCommentBlock = false;
	bool overrideFlag = false;
	for(int letterIndex = 0; letterIndex < SIMSourceCode.length(); letterIndex++)
	{
		if(SIMSourceCode[letterIndex] == COMMENT_STARTED)
		{
			insideCommentBlock = true;
		}
		else if(SIMSourceCode[letterIndex] == COMMENT_ENDED)
		{
			insideCommentBlock = false;
			overrideFlag = true;
		}
		if(insideCommentBlock || overrideFlag)
		{
			if(overrideFlag)
			{
				overrideFlag = false;
			}
			SIMSourceCode.erase(letterIndex--,SINGLE_CHARACTER_LENGTH);
		}
	}
	return SIMSourceCode;
}

// Trims all useless characters from a SIM source code
void trimUnneededCharactersFromSIMSourceCode(string & SIMSourceCode)
{
	size_t emptySpaceIndex;
	trimCommentsFromSIMSourceCode(SIMSourceCode);
	while((emptySpaceIndex = SIMSourceCode.find(EXTRA_NEWLINE_SEQUENCE,FIRST_CHARACTER)) != string::npos)
	{
	    SIMSourceCode.erase(emptySpaceIndex,SINGLE_CHARACTER_LENGTH);
	}
	while((emptySpaceIndex = SIMSourceCode.find(TABULATION_CHARACTER,FIRST_CHARACTER)) != string::npos)
	{
	    SIMSourceCode.erase(emptySpaceIndex,SINGLE_CHARACTER_LENGTH);
	}
	while((emptySpaceIndex = SIMSourceCode.find(EXTRA_NEWLINE_SEQUENCE,FIRST_CHARACTER)) != string::npos)
	{
	    SIMSourceCode.erase(emptySpaceIndex,SINGLE_CHARACTER_LENGTH);
	}
	trim(SIMSourceCode);
}

// Trims all useless characters from a SIM source code
string trimUnneededCharactersFromSIMSourceCodeToString(string SIMSourceCode)
{
	size_t emptySpaceIndex;
	trimCommentsFromSIMSourceCode(SIMSourceCode);
	while((emptySpaceIndex = SIMSourceCode.find(EXTRA_NEWLINE_SEQUENCE,FIRST_CHARACTER)) != string::npos)
	{
	    SIMSourceCode.erase(emptySpaceIndex,SINGLE_CHARACTER_LENGTH);
	}
	while((emptySpaceIndex = SIMSourceCode.find(TABULATION_CHARACTER,FIRST_CHARACTER)) != string::npos)
	{
	    SIMSourceCode.erase(emptySpaceIndex,SINGLE_CHARACTER_LENGTH);
	}
	while((emptySpaceIndex = SIMSourceCode.find(EXTRA_NEWLINE_SEQUENCE,FIRST_CHARACTER)) != string::npos)
	{
	    SIMSourceCode.erase(emptySpaceIndex,SINGLE_CHARACTER_LENGTH);
	}
	trim(SIMSourceCode);
	return SIMSourceCode;
}

// Lexeme functions

// Gets the list of lexeme from a SIM source code
vector<lexeme> getLexemesFromSIMSourceCode(string & SIMSourceCode, ifstream * SIMFile, bool closeAfterDone, bool deleteAfterDone)
{
	vector<lexeme> lexemes;
	string lexemeBuffer;
	bool enteredLiteral = false, overrideFlag = false;
	int characterIndex = 0, splitSize;
	char currentChar, nextChar;
	for(int letterIndex = 0; letterIndex < SIMSourceCode.length(); letterIndex++)
	{
		currentChar = SIMSourceCode[letterIndex];
		if(isLexemeCharacterValid(currentChar) || enteredLiteral)
		{
			if(currentChar == '\"')
			{
				overrideFlag = true;
				if(!enteredLiteral)
				{
					enteredLiteral = true;
					addLexemeIntoVector(lexemeBuffer,lexemes,true);
					lexemeBuffer.push_back(currentChar);
				}
				else
				{
					enteredLiteral = false;
					lexemeBuffer.push_back(currentChar);
					addLexemeIntoVector(lexemeBuffer,lexemes,false);
				}
			}
			if(!overrideFlag)
			{
				if(!enteredLiteral)
				{
					nextChar = ((letterIndex + 1 <= SIMSourceCode.length() - 1) ? SIMSourceCode[letterIndex + 1] : EMPTY_CHARACTER);
					splitSize = doesLexemeNeedsSplit(currentChar,nextChar);
					if(splitSize != 0)
					{
						if(splitSize == 1)
						{
							addLexemeIntoVector(lexemeBuffer,lexemes,true);
							lexemeBuffer.push_back(currentChar);
							addLexemeIntoVector(lexemeBuffer,lexemes,true);
						}
						else
						{
							addLexemeIntoVector(lexemeBuffer,lexemes,true);
							lexemeBuffer.push_back(currentChar);
							lexemeBuffer.push_back(nextChar);
							addLexemeIntoVector(lexemeBuffer,lexemes,true);
							letterIndex++;
						}
					}
					else
					{
						lexemeBuffer.push_back(currentChar);
					}
				}
				else
				{
					lexemeBuffer.push_back(currentChar);
				}
			}
			else
			{
				overrideFlag = false;
			}
			if(letterIndex == SIMSourceCode.length() - 1)
			{
				addLexemeIntoVector(lexemeBuffer,lexemes,true);
			}
		}
		else
		{
			addLexemeIntoVector(lexemeBuffer,lexemes,true);
		}
	}
	trimLexemes(lexemes);
	rewindSIMFile(SIMFile);
	resolvePropertiesForLexemes(lexemes,SIMFile);
	if(closeAfterDone)
	{
		SIMFile->close();
	}
	if(deleteAfterDone)
	{
		delete SIMFile;
	}
	return lexemes;
}

// Checks if a character is part of a lexeme
bool isLexemeCharacterValid(char character)
{
	return (character != WHITE_SPACE_CHARACTER && character != NEWLINE_CHARACTER && character != TABULATION_CHARACTER);
}

// Checks if a lexeme needs to split up and if so, tells the size of the split
int doesLexemeNeedsSplit(char currentChar, char nextChar)
{
	int result = 0;
	if(nextChar == '\0')
	{
		if(isCharacterSingleSplittable(currentChar))
		{
			result = 1;
		}
	}
	else
	{
		if(isCharacterDoubleSplittable(currentChar))
		{
			result = ((isCharacterSingleSplittable(nextChar)) ? 2 : 1);
		}
		else if(isCharacterSingleSplittable(currentChar))
		{
			result = 1;
		}
	}
	return result;
}

// Checks if character is single splittable
bool isCharacterSingleSplittable(char character)
{
	return (character == '>' || character == '<' || character == '=' || character == '(' || character == ')' || 
			character == '+' || character == '-' || character == ',' || character == ':' || character == ';' ||
			character == '.');
}

// Checks if character is double splittable (another useful charactet can come after it)
bool isCharacterDoubleSplittable(char character)
{
	return (character == '>' || character == '<' || character == ':');
}

// Properly adds a lexeme into a list of lexemes
void addLexemeIntoVector(string & lexemeBuffer, vector<lexeme> & lexemes, bool trimLexeme)
{
	if(lexemeBuffer.length() > 0)
	{
		if(trimLexeme)
		{
			trim(lexemeBuffer);
		}
		lexemes.push_back(createLexeme(lexemeBuffer));
	}
	lexemeBuffer.clear();
}

// Creates a lexeme
lexeme createLexeme(string contents, int lineNumber)
{
	lexeme newLexeme;
	newLexeme.contents = contents;
	newLexeme.lineNumber = lineNumber;
	return newLexeme;
}

// Resolves all the lexeme's extra properties in a list of lexemes
void resolvePropertiesForLexemes(vector<lexeme> & lexemes, ifstream * SIMFile)
{
	int lineCounter = 0, lexemeIndex = 0, lexemesSize = lexemes.size();
	bool needsNewLine;
	string line;
	while(getline((*SIMFile),line) && (lexemeIndex < lexemesSize))
	{
		needsNewLine = false;
		lineCounter++;
		while(!needsNewLine && (lexemeIndex < lexemesSize))
		{
			int matchIndex = line.find(lexemes[lexemeIndex].contents);
			if(matchIndex != string::npos)
			{
				lexemes[lexemeIndex++].lineNumber = lineCounter;
			}
			else
			{
				needsNewLine = true;
			}
		}
	}
}

// Removes all empty lexemes from a list of lexemes
void trimLexemes(vector<lexeme> & lexemes)
{
	for(int lexemeIndex = 0; lexemeIndex < lexemes.size(); lexemeIndex++)
	{
		if(lexemes[lexemeIndex].contents.length() == 0)
		{
			lexemes.erase(lexemes.begin() + lexemeIndex--);
		}
	}
}

// Converts a list of lexemes into string
string lexemesToString(vector<lexeme> & lexemes)
{
	ostringstream buffer;
	buffer << "\nLexemes: " << lexemes.size() << endl << endl;
	for(int lexemeIndex = 0; lexemeIndex < lexemes.size(); lexemeIndex++)
	{
		lexeme aux = lexemes[lexemeIndex];
		buffer << "\t" << (lexemeIndex + 1) << ":\n\t\t<contents: " << aux.contents << " | lineNumber: " << aux.lineNumber << ">" << endl;
	}
	return buffer.str();
}

// Token functions

// Creates a token
token createToken(tokenType type, int lineNumber, string attribute, string extra)
{
	token newToken;
	newToken.type = type;
	newToken.lineNumber = lineNumber;
	newToken.attribute = attribute;
	newToken.extra = extra;
	return newToken;
}

// Converts a list of tokens into string
string tokensToString(vector<token> & tokens, bool prettyPrint)
{
	ostringstream buffer;
	token aux;
	if(prettyPrint)
	{
		buffer << "\nTokens: " << tokens.size() << endl << endl;
		for(int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++)
		{
			aux = tokens[tokenIndex];
			buffer << "\t" << (tokenIndex + 1) << ":\n\t\t<type: " << tokenTypeToString(aux.type) << " | lineNumber: " << aux.lineNumber << ((aux.attribute.length() > 0) ? (" | attribute: " + aux.attribute) : EMPTY_STRING) << ((aux.extra.length() > 0) ? (" | extra: " + aux.extra) : EMPTY_STRING) << ">" << endl;
		}
	}
	else
	{
		for(int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++)
		{
			aux = tokens[tokenIndex];
			buffer << "<" << tokenTypeToString(aux.type) << ((aux.attribute.length() > 0) ? (", " + aux.attribute) : EMPTY_STRING) << ((aux.extra.length() > 0) ? (", " + aux.extra) : EMPTY_STRING) << ">" << endl;
		}
	}
	return buffer.str();
}

// Converts the token type to string
string tokenTypeToString(tokenType type)
{
	string typeString;
	switch(type)
	{
		case TT_PROGRAM:
			typeString = "PROGRAM";
			break;
		case TT_VAR:
			typeString = "VAR";
			break;
		case TT_INTEGER:
			typeString = "INTEGER";
			break;
		case TT_BOOLEAN:
			typeString = "BOOLEAN";
			break;
		case TT_PROCEDURE:
			typeString = "PROCEDURE";
			break;
		case TT_FUNCTION:
			typeString = "FUNCTION";
			break;
		case TT_BEGIN:
			typeString = "BEGIN";
			break;
		case TT_END:
			typeString = "END";
			break;
		case TT_IF:
			typeString = "IF";
			break;
		case TT_THEN:
			typeString = "THEN";
			break;
		case TT_ELSE:
			typeString = "ELSE";
			break;
		case TT_END_IF:
			typeString = "END_IF";
			break;
		case TT_WHILE:
			typeString = "WHILE";
			break;
		case TT_END_WHILE:
			typeString = "END_WHILE";
			break;
		case TT_DO:
			typeString = "DO";
			break;
		case TT_READ:
			typeString = "READ";
			break;
		case TT_WRITE:
			typeString = "WRITE";
			break;
		case TT_RETURN:
			typeString = "RETURN";
			break;
		case TT_TRUE:
			typeString = "TRUE";
			break;
		case TT_FALSE:
			typeString = "FALSE";
			break;
		case TT_NOT:
			typeString = "NOT";
			break;
		case TT_RELATIONAL_LOGICAL_OPERATOR:
			typeString = "RELATIONAL_LOGICAL_OPERATOR";
			break;
		case TT_ARITHMETIC_OPERATOR:
			typeString = "ARITHMETIC_OPERATOR";
			break;
		case TT_ASSIGNMENT_OPERATOR:
			typeString = "ASSIGNMENT_OPERATOR";
			break;
		case TT_IDENTIFIER:
			typeString = "IDENTIFIER";
			break;
		case TT_LITERAL:
			typeString = "LITERAL";
			break;
		case TT_NUMBER:
			typeString = "NUMBER";
			break;
		case TT_PUNCTUATION:
			typeString = "PUNCTUATION";
			break;
		default:
			typeString = "UNKNOWN";
			break;
	}
	return typeString;
}

// Finds the fixed attribute number for a token
int findFixedAttibuteNumberForToken(tokenType type, string attributeName)
{
	int attributeNumber = FIXED_ATTRIBUTE_NOT_FOUND;
	switch(type)
	{
		case TT_RELATIONAL_LOGICAL_OPERATOR:
			if(attributeName.compare(SA_AND))
			{
				attributeNumber = FS_AND;
			}
			else if(attributeName.compare(SA_EQUAL))
			{
				attributeNumber = FS_EQUAL;
			}
			else if(attributeName.compare(SA_LESS_THAN))
			{
				attributeNumber = FS_LESS_THAN;
			}
			else if(attributeName.compare(SA_UNEQUAL))
			{
				attributeNumber = FS_UNEQUAL;
			}
			else if(attributeName.compare(SA_LESS_EQUAL))
			{
				attributeNumber = FS_LESS_EQUAL;
			}
			else if(attributeName.compare(SA_GREATER_THAN))
			{
				attributeNumber = FS_GREATER_THAN;
			}
			else if(attributeName.compare(SA_GREATER_EQUAL))
			{
				attributeNumber = FS_GREATER_EQUAL;
			}
			else if(attributeName.compare(SA_OR))
			{
				attributeNumber = FS_OR;
			}
			break;
		case TT_ARITHMETIC_OPERATOR:
			if(attributeName.compare(SA_PLUS))
			{
				attributeNumber = FS_PLUS;
			}
			else if(attributeName.compare(SA_MINUS))
			{
				attributeNumber = FS_MINUS;
			}
			break;
		case TT_PUNCTUATION:
			if(attributeName.compare(SA_SEMICOLON))
			{
				attributeNumber = FS_SEMICOLON;
			}
			else if(attributeName.compare(SA_COLON))
			{
				attributeNumber = FS_COLON;
			}
			else if(attributeName.compare(SA_DOT))
			{
				attributeNumber = FS_DOT;
			}
			else if(attributeName.compare(SA_LEFT_PARENTHESES))
			{
				attributeNumber = FS_LEFT_PARENTHESES;
			}
			else if(attributeName.compare(SA_RIGHT_PARENTHESES))
			{
				attributeNumber = FS_RIGHT_PARENTHESES;
			}
			else if(attributeName.compare(SA_COMMA))
			{
				attributeNumber = FS_COMMA;
			}
			else if(attributeName.compare(SA_QUOTE))
			{
				attributeNumber = FS_QUOTE;
			}
			break;
		case TT_ASSIGNMENT_OPERATOR:
			if(attributeName.compare(SA_ASSIGNMENT))
			{
				attributeNumber = FS_ASSIGNMENT;
			}
			else if(attributeName.compare(SA_INCREMENTER))
			{
				attributeNumber = FS_INCREMENTER;
			}
			else if(attributeName.compare(SA_DECREMENTER))
			{
				attributeNumber = FS_DECREMENTER;
			}
			break;
		default:
			break;
	}
	return attributeNumber;
}

// Finds the fixed attribute name for a token
string findFixedAttibuteNameForToken(tokenType type, int attributeNumber)
{
	string attributeName = EMPTY_STRING;
	switch(type)
	{
		case TT_RELATIONAL_LOGICAL_OPERATOR:
			if(attributeNumber == FS_AND)
			{
				attributeName = SA_AND;
			}
			else if(attributeNumber == FS_EQUAL)
			{
				attributeName = SA_EQUAL;
			}
			else if(attributeNumber == FS_LESS_THAN)
			{
				attributeName = SA_LESS_THAN;
			}
			else if(attributeNumber == FS_UNEQUAL)
			{
				attributeName = SA_UNEQUAL;
			}
			else if(attributeNumber == FS_LESS_EQUAL)
			{
				attributeName = SA_LESS_EQUAL;
			}
			else if(attributeNumber == FS_GREATER_THAN)
			{
				attributeName = SA_GREATER_THAN;
			}
			else if(attributeNumber == FS_GREATER_EQUAL)
			{
				attributeName = SA_GREATER_EQUAL;
			}
			else if(attributeNumber == FS_OR)
			{
				attributeName = SA_OR;
			}
			break;
		case TT_ARITHMETIC_OPERATOR:
			if(attributeNumber == FS_PLUS)
			{
				attributeName = SA_PLUS;
			}
			else if(attributeNumber == FS_MINUS)
			{
				attributeName = SA_MINUS;
			}
			break;
		case TT_PUNCTUATION:
			if(attributeNumber == FS_SEMICOLON)
			{
				attributeName = SA_SEMICOLON;
			}
			else if(attributeNumber == FS_COLON)
			{
				attributeName = SA_COLON;
			}
			else if(attributeNumber == FS_DOT)
			{
				attributeName = SA_DOT;
			}
			else if(attributeNumber == FS_LEFT_PARENTHESES)
			{
				attributeName = SA_LEFT_PARENTHESES;
			}
			else if(attributeNumber == FS_RIGHT_PARENTHESES)
			{
				attributeName = SA_RIGHT_PARENTHESES;
			}
			else if(attributeNumber == FS_COMMA)
			{
				attributeName = SA_COMMA;
			}
			else if(attributeNumber == FS_QUOTE)
			{
				attributeName = SA_QUOTE;
			}
			break;
		case TT_ASSIGNMENT_OPERATOR:
			if(attributeNumber == FS_ASSIGNMENT)
			{
				attributeName = SA_ASSIGNMENT;
			}
			else if(attributeNumber == FS_INCREMENTER)
			{
				attributeName = SA_INCREMENTER;
			}
			else if(attributeNumber == FS_DECREMENTER)
			{
				attributeName = SA_DECREMENTER;
			}
			break;
		default:
			break;
	}
	return attributeName;
}

// Pseudo token functions

// Creates a pseudo token
pseudoToken createPseudoToken(string attribute, int lineNumber, string original)
{
	pseudoToken newPseudoToken;
	newPseudoToken.attribute = attribute;
	newPseudoToken.lineNumber = lineNumber;
	newPseudoToken.original = original;
	return newPseudoToken;
}

// Grammar functions and syntatical analysis functions

// Gets the full grammar LL(1) grammar map from a JSON file
map<string,innerMap> getll1GrammarMapFromGrammarFile(ifstream & ll1GrammarFile)
{
	bool validDocument = true;
	stringstream buffer;
	vector<string> ll1GrammarJSONKeys;
	map<string,innerMap> ll1GrammarMap;
	Document ll1GrammarJSONDocument;
	buffer << ll1GrammarFile.rdbuf();
	ll1GrammarJSONDocument.Parse<0>(buffer.str().c_str());
	if(ll1GrammarJSONDocument.IsObject())
	{
		for(Value::ConstMemberIterator iterator = ll1GrammarJSONDocument.MemberBegin(); iterator != ll1GrammarJSONDocument.MemberEnd(); iterator++)
		{
			ll1GrammarJSONKeys.push_back(iterator->name.GetString());
		}
		for(int keyIndex = 0; keyIndex < ll1GrammarJSONKeys.size(); keyIndex++)
		{
			const char * key = ll1GrammarJSONKeys[keyIndex].c_str();
			if(!ll1GrammarJSONDocument.HasMember(key))
			{
				validDocument = false;
			}
			else
			{
				if(!ll1GrammarJSONDocument[key].IsObject())
				{
					validDocument = false;
				}
			}
		}
		if(validDocument)
		{
			for(int keyIndex = 0; keyIndex < ll1GrammarJSONKeys.size(); keyIndex++)
			{
				const char * key = ll1GrammarJSONKeys[keyIndex].c_str();
				vector<string> temporaryJSONKeys;
				for(Value::ConstMemberIterator iterator = ll1GrammarJSONDocument[key].MemberBegin(); iterator != ll1GrammarJSONDocument[key].MemberEnd(); iterator++)
				{
					temporaryJSONKeys.push_back(iterator->name.GetString());
				}
				for(int innerKeyIndex = 0; innerKeyIndex < temporaryJSONKeys.size(); innerKeyIndex++)
				{
					const char * innerKey = temporaryJSONKeys[innerKeyIndex].c_str();
					if(ll1GrammarJSONDocument[key][innerKey].IsString())
					{
						ll1GrammarMap[ll1GrammarJSONKeys[keyIndex]][temporaryJSONKeys[innerKeyIndex]] = ll1GrammarJSONDocument[key][innerKey].GetString();
					}
					else
					{
						validDocument = false;
					}
				}
				temporaryJSONKeys.clear();
			}
		}
	}
	if(!validDocument)
	{
		ll1GrammarMap.clear();
	}
	return ll1GrammarMap;
}

// Gets the input queue from a list of tokens
queue<pseudoToken> getInputQueueFromTokens(vector<token> & tokens)
{
	bool overrideFlag = false;
	string element;
	queue<pseudoToken> inputQueue;
	for(int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++)
	{
		token currentToken = tokens[tokenIndex];
		element = currentToken.attribute;
		if(currentToken.type == TT_IDENTIFIER)
		{
			overrideFlag = true;
			inputQueue.push(createPseudoToken("i",currentToken.lineNumber,currentToken.attribute));
			for(int letterIndex = 0; letterIndex < currentToken.attribute.length(); letterIndex++)
			{
				string aux;
				aux.push_back(currentToken.attribute[letterIndex]);
				inputQueue.push(createPseudoToken(aux,currentToken.lineNumber,currentToken.attribute));
			}
		}
		else if(currentToken.type == TT_LITERAL)
		{
			overrideFlag = true;
			inputQueue.push(createPseudoToken("l",currentToken.lineNumber,currentToken.attribute));
			inputQueue.push(createPseudoToken("i",currentToken.lineNumber,currentToken.attribute));
			inputQueue.push(createPseudoToken("t",currentToken.lineNumber,currentToken.attribute));
			inputQueue.push(createPseudoToken("e",currentToken.lineNumber,currentToken.attribute));
			inputQueue.push(createPseudoToken("r",currentToken.lineNumber,currentToken.attribute));
			inputQueue.push(createPseudoToken("a",currentToken.lineNumber,currentToken.attribute));
			inputQueue.push(createPseudoToken("l",currentToken.lineNumber,currentToken.attribute));
		}
		if(!overrideFlag)
		{
			inputQueue.push(createPseudoToken(element,currentToken.lineNumber,currentToken.attribute));
		}
		else
		{
			overrideFlag = false;
		}
	}
	inputQueue.push(createPseudoToken(LL1_GRAMMAR_DELIMITER));
	return inputQueue;
}

// Gets an empty and read symbols stack
stack<string> getSymbolsStack()
{
	stack<string> symbolsStack;
	symbolsStack.push(LL1_GRAMMAR_DELIMITER);
	symbolsStack.push("PROGRAM");
	return symbolsStack;
}

// Check if a given symbol is nonterminal
bool isSymbolNonterminal(string symbol)
{
	return symbol.compare(GR_PROGRAM) == EQUAL_STRINGS 
			|| symbol.compare(GR_BLOCK) == EQUAL_STRINGS
			|| symbol.compare(GR_BLOCK_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_BLOCK_SECOND_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_VARIABLES_DECLARATION_STAGE) == EQUAL_STRINGS
			|| symbol.compare(GR_VARIABLES_DECLARATION_STAGE_AUX) == EQUAL_STRINGS
			|| symbol.compare(GR_VARIABLES_DECLARATION_STAGE_AUX_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_VARIABLES_DECLARATION) == EQUAL_STRINGS
			|| symbol.compare(GR_VARIABLES_DECLARATION_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_TYPE) == EQUAL_STRINGS
			|| symbol.compare(GR_SUB_ROUTINES_DECLARATION_STAGE) == EQUAL_STRINGS
			|| symbol.compare(GR_SUB_ROUTINES_DECLARATION_STAGE_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_PROCEDURE_DECLARATION) == EQUAL_STRINGS
			|| symbol.compare(GR_FUNCTION_DECLARATION) == EQUAL_STRINGS
			|| symbol.compare(GR_COMMANDS) == EQUAL_STRINGS
			|| symbol.compare(GR_COMMANDS_AUX) == EQUAL_STRINGS
			|| symbol.compare(GR_COMMANDS_AUX_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_COMMANDS_AUX_SECOND_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_COMMAND) == EQUAL_STRINGS
			|| symbol.compare(GR_COMMAND_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_PROCEDURE_CALL_OR_ATTRIBUTION_COMMAND) == EQUAL_STRINGS
			|| symbol.compare(GR_ATTRIBUTION_COMMAND) == EQUAL_STRINGS
			|| symbol.compare(GR_ATTRIBUTION_TYPE) == EQUAL_STRINGS
			|| symbol.compare(GR_CONDITIONAL_COMMAND) == EQUAL_STRINGS
			|| symbol.compare(GR_CONDITIONAL_COMMAND_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_WHILE_COMMAND) == EQUAL_STRINGS
			|| symbol.compare(GR_READ_COMMAND) == EQUAL_STRINGS
			|| symbol.compare(GR_WRITE_COMMAND) == EQUAL_STRINGS
			|| symbol.compare(GR_WRITE_COMMAND_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_EXPRESSION) == EQUAL_STRINGS
			|| symbol.compare(GR_EXPRESSION_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_RELATIONAL_LOGICAL_OPERATOR) == EQUAL_STRINGS
			|| symbol.compare(GR_SIMPLE_EXPRESSION) == EQUAL_STRINGS
			|| symbol.compare(GR_FACTOR) == EQUAL_STRINGS
			|| symbol.compare(GR_VARIABLE) == EQUAL_STRINGS
			|| symbol.compare(GR_FUNCTION_RETURN) == EQUAL_STRINGS
			|| symbol.compare(GR_FUNCTION_RETURN_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_IDENTIFIER) == EQUAL_STRINGS
			|| symbol.compare(GR_IDENTIFIER_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_IDENTIFIER_SECOND_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_NUMBER) == EQUAL_STRINGS
			|| symbol.compare(GR_NUMBER_EXTENSION) == EQUAL_STRINGS
			|| symbol.compare(GR_DIGIT) == EQUAL_STRINGS
			|| symbol.compare(GR_LETTER) == EQUAL_STRINGS;
}

// Extracts all symbols from a given rule
vector<string> getSymbolsFromRule(string rule)
{
	string buffer;
	vector<string> symbols;
	for(int letterIndex = 0; letterIndex < rule.length(); letterIndex++)
	{
		if(rule[letterIndex] != WHITE_SPACE_CHARACTER)
		{
			buffer += rule[letterIndex];
		}
		else
		{
			symbols.push_back(buffer);
			buffer.clear();
		}
	}
	symbols.push_back(buffer);
	reverse(symbols.begin(),symbols.end());
	return symbols;
}

// Symbol, symbol table and semantical analysis functions

// Creates a symbol
symbol createSymbol(symbolCategory category, symbolType type, int number, string name, string scope, string value)
{
	symbol newSymbol;
	newSymbol.category = category;
	newSymbol.type = type;
	newSymbol.number = number;
	newSymbol.name = name;
	newSymbol.scope = scope;
	newSymbol.value = value;
	return newSymbol;
}

// Converts the symbol category to string
string symbolCategoryToString(symbolCategory category)
{
	string categoryString;
	switch(category)
	{
		case SC_VAR:
			categoryString = "VAR";
			break;
		case SC_PROGRAM:
			categoryString = "PROGRAM";
			break;
		case SC_PROCEDURE:
			categoryString = "PROCEDURE";
			break;
		case SC_FUNCTION:
			categoryString = "FUNCTION";
			break;
		default:
			categoryString = "UNKNOWN";
			break;
	}
	return categoryString;
}

// Converts the symbol type to string
string symbolTypeToString(symbolType type)
{
	string typeString;
	switch(type)
	{
		case ST_INTEGER:
			typeString = "INTEGER";
			break;
		case ST_BOOLEAN:
			typeString = "BOOLEAN";
			break;
		case ST_NONE:
			typeString = "NONE";
			break;
		default:
			typeString = "UNKNOWN";
			break;
	}
	return typeString;
}

// Converts a token type to symbol type
symbolType tokenTypeToSymbolType(tokenType type)
{
	symbolType aux;
	switch(type)
	{
		case TT_INTEGER:
			aux = ST_INTEGER;
			break;
		case TT_BOOLEAN:
			aux = ST_BOOLEAN;
			break;
		default:
			aux = ST_NONE;
			break;
	}
	return aux;
}

// Gets the global symbol table from a list of tokens
vector<symbol> getSymbolTableFromTokens(vector<token> & tokens)
{
	int currentNumber = 0;
	bool pushSymbol;
	string currentName, currentScope, currentValue;
	vector<symbol> symbolTable;
	stack<string> scopesStack;
	symbolCategory currentCategory;
	symbolType currentType;
	for(int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++)
	{
		pushSymbol = false;
		token currentToken = tokens[tokenIndex];
		switch(currentToken.type)
		{
			case TT_PROGRAM:
				pushSymbol = true;
				currentCategory = SC_PROGRAM;
				currentType = ST_NONE;
				currentName = tokens[tokenIndex + 1].attribute;
				scopesStack.push(tokens[tokenIndex + 1].attribute);
				currentScope = "GLOBAL (" + scopesStack.top() + ")";
				currentValue = "NONE";
				break;
			case TT_PROCEDURE:
				pushSymbol = true;
				currentCategory = SC_PROCEDURE;
				currentType = ST_NONE;
				currentName = tokens[tokenIndex + 1].attribute;
				currentScope = "GLOBAL (" + scopesStack.top() + ")";
				scopesStack.push(tokens[tokenIndex + 1].attribute);
				currentValue = "NONE";
				break;
			case TT_FUNCTION:
				pushSymbol = true;
				currentCategory = SC_FUNCTION;
				currentType = tokenTypeToSymbolType(tokens[tokenIndex + 3].type);
				currentName = tokens[tokenIndex + 1].attribute;
				currentScope = "GLOBAL (" + scopesStack.top() + ")";
				scopesStack.push(tokens[tokenIndex + 1].attribute);
				currentValue = "NONE";
				break;
			case TT_END:
				scopesStack.pop();
				break;
			case TT_VAR:
				currentCategory = SC_VAR;
				currentValue = "VALUE ()";
				int innerTokenIndex = tokenIndex;
				token innerCurrentToken;
				while(true)
				{
					int auxiliaryIndex = innerTokenIndex;
					while(true)
					{
						innerCurrentToken = tokens[auxiliaryIndex++];
						if(innerCurrentToken.type == TT_INTEGER || innerCurrentToken.type == TT_BOOLEAN)
						{
							currentType = tokenTypeToSymbolType(innerCurrentToken.type);
							break;
						}
					}
					while(true)
					{
						innerCurrentToken = tokens[innerTokenIndex++];
						if(innerCurrentToken.type == TT_IDENTIFIER)
						{
							currentName = innerCurrentToken.attribute;
							currentScope = string(((scopesStack.size() == 1) ? "GLOBAL" : "LOCAL")) + " (" + scopesStack.top() + ")";
							symbolTable.push_back(createSymbol(currentCategory,currentType,currentNumber++,currentName,currentScope,currentValue));
						}
						else if(innerCurrentToken.type == TT_PUNCTUATION && innerCurrentToken.attribute.compare(SA_SEMICOLON) == EQUAL_STRINGS)
						{
							break;
						}
					}
					if(tokens[innerTokenIndex].type != TT_IDENTIFIER)
					{
						break;
					}
				}
				
				break;
		}
		if(pushSymbol)
		{
			symbolTable.push_back(createSymbol(currentCategory,currentType,currentNumber++,currentName,currentScope,currentValue));
		}
	}
	return symbolTable;
}

// Checks if a symbol exists through its number in the symbol table
bool doesSymbolExist(vector<symbol> & symbolTable, int number)
{
	bool result = false;
	for(int symbolIndex = 0; symbolIndex < symbolTable.size(); symbolIndex++)
	{
		if(symbolTable[symbolIndex].number == number)
		{
			result = true;
			break;
		}
	}
	return result;
}

// Retrieves a symbol through its number from the symbol table
symbol getSymbolFromSymbolTable(vector<symbol> & symbolTable, int number)
{
	symbol result;
	for(int symbolIndex = 0; symbolIndex < symbolTable.size(); symbolIndex++)
	{
		if(symbolTable[symbolIndex].number == number)
		{
			result = symbolTable[symbolIndex];
		}
	}
	return result;
}

// Removes a symbol through its number from the symbol table
bool removeSymbolFromSymbolTable(vector<symbol> & symbolTable, int number)
{
	bool result = false;
	for(int symbolIndex = 0; symbolIndex < symbolTable.size(); symbolIndex++)
	{
		if(symbolTable[symbolIndex].number == number)
		{
			symbolTable.erase(symbolTable.begin() + symbolIndex--);
			result = true;
			break;
		}
	}
	return result;
}

// Updates a symbol in the symbol table
bool updateSymbolToSymbolTable(vector<symbol> & symbolTable, symbol instance)
{
	bool result = false;
	for(int symbolIndex = 0; symbolIndex < symbolTable.size(); symbolIndex++)
	{
		if(symbolTable[symbolIndex].number == instance.number)
		{
			symbolTable[symbolIndex].name = instance.name;
			symbolTable[symbolIndex].value = instance.value;
			symbolTable[symbolIndex].scope = instance.scope;
			symbolTable[symbolIndex].category = instance.category;
			symbolTable[symbolIndex].type = instance.type;
			result = true;
			break;
		}
	}
	return result;
}

// Converts a symbol table into string
string symbolTableToString(vector<symbol> & symbolTable, bool prettyPrint)
{
	ostringstream buffer;
	symbol aux;
	if(prettyPrint)
	{
		buffer << "\nSymbol Table: " << symbolTable.size() << endl << endl;
		buffer << format("\t%8s\t%8s\t%8s\t%8s\t%8s\t%8s\n") % "#" % "Name" % "Value" % "Scope" % "Category" % "Type";
		for(int symbolIndex = 0; symbolIndex < symbolTable.size(); symbolIndex++)
		{
			aux = symbolTable[symbolIndex];
			buffer << format("\t%8s\t%8s\t%8s\t%8s\t%8s\t%8s\n") % aux.number % aux.name % aux.value % aux.scope % symbolCategoryToString(aux.category) % symbolTypeToString(aux.type);
		}
	}
	else
	{
		buffer << format("%8s\t%8s\t%8s\t%8s\t%8s\t%8s\n") % "#" % "Name" % "Value" % "Scope" % "Category" % "Type";
		for(int symbolIndex = 0; symbolIndex < symbolTable.size(); symbolIndex++)
		{
			aux = symbolTable[symbolIndex];
			buffer << format("%8s\t%8s\t%8s\t%8s\t%8s\t%8s\n") % aux.number % aux.name % aux.value % aux.scope % symbolCategoryToString(aux.category) % symbolTypeToString(aux.type);
		}
	}
	return buffer.str();
}

// Retrieves the type check stack for a list of tokens, its current index and scopes stack
stack<innerVector> getTypeCheckStackForToken(vector<token> & tokens, int tokenIndex, stack<token> & scopesStack)
{
	int innerTokenIndex;
	token currentToken;
	stack<innerVector> typeCheckStack;
	vector<token> currentExpressionList;
	switch(tokens[tokenIndex].type)
	{
		case TT_IF:	
			innerTokenIndex = tokenIndex + 2;
			while(true)
			{
				currentToken = tokens[innerTokenIndex++];
				if(currentToken.type == TT_THEN)
				{
					if(currentExpressionList.size() > 0)
					{
						typeCheckStack.push(currentExpressionList);
					}
					currentExpressionList.clear();
					break;
				}
				else if(currentToken.type != TT_RELATIONAL_LOGICAL_OPERATOR && currentToken.type != TT_PUNCTUATION)
				{
					currentExpressionList.push_back(currentToken);
				}
				else if(currentToken.attribute.compare(SA_LEFT_PARENTHESES) == EQUAL_STRINGS)
				{
					if(currentExpressionList.size() > 0)
					{
						typeCheckStack.push(currentExpressionList);
					}
					currentExpressionList.clear();
				}
			}
			break;
		case TT_WHILE:
			innerTokenIndex = tokenIndex + 2;
			while(true)
			{
				currentToken = tokens[innerTokenIndex++];
				if(currentToken.type == TT_DO)
				{
					if(currentExpressionList.size() > 0)
					{
						typeCheckStack.push(currentExpressionList);
					}
					currentExpressionList.clear();
					break;
				}
				else if(currentToken.type != TT_RELATIONAL_LOGICAL_OPERATOR && currentToken.type != TT_PUNCTUATION)
				{
					currentExpressionList.push_back(currentToken);
				}
				else if(currentToken.attribute.compare(SA_LEFT_PARENTHESES) == EQUAL_STRINGS)
				{
					if(currentExpressionList.size() > 0)
					{
						typeCheckStack.push(currentExpressionList);
					}
					currentExpressionList.clear();
				}
			}
			break;
		case TT_ASSIGNMENT_OPERATOR:
			currentExpressionList.push_back(tokens[tokenIndex - 1]);
			typeCheckStack.push(currentExpressionList);
			currentExpressionList.clear();
			innerTokenIndex = tokenIndex + 1;
			while(true)
			{
				currentToken = tokens[innerTokenIndex++];
				if((currentToken.type == TT_PUNCTUATION && currentToken.attribute.compare(SA_SEMICOLON) == EQUAL_STRINGS))
				{
					if(currentExpressionList.size() > 0)
					{
						typeCheckStack.push(currentExpressionList);
					}
					currentExpressionList.clear();
					break;
				}
				else if(currentToken.type != TT_RELATIONAL_LOGICAL_OPERATOR && currentToken.type != TT_PUNCTUATION)
				{
					currentExpressionList.push_back(currentToken);
				}
				else if(currentToken.attribute.compare(SA_LEFT_PARENTHESES) == EQUAL_STRINGS)
				{
					if(currentExpressionList.size() > 0)
					{
						typeCheckStack.push(currentExpressionList);
					}
					currentExpressionList.clear();
				}
			}
			break;
		case TT_RETURN:
			currentExpressionList.push_back(scopesStack.top());
			typeCheckStack.push(currentExpressionList);
			currentExpressionList.clear();
			innerTokenIndex = tokenIndex + 1;
			while(true)
			{
				currentToken = tokens[innerTokenIndex++];
				if((currentToken.type == TT_PUNCTUATION && currentToken.attribute.compare(SA_SEMICOLON) == EQUAL_STRINGS))
				{
					if(currentExpressionList.size() > 0)
					{
						typeCheckStack.push(currentExpressionList);
					}
					currentExpressionList.clear();
					break;
				}
				else if(currentToken.type != TT_RELATIONAL_LOGICAL_OPERATOR && currentToken.type != TT_PUNCTUATION)
				{
					currentExpressionList.push_back(currentToken);
				}
				else if(currentToken.attribute.compare(SA_LEFT_PARENTHESES) == EQUAL_STRINGS)
				{
					if(currentExpressionList.size() > 0)
					{
						typeCheckStack.push(currentExpressionList);
					}
					currentExpressionList.clear();
				}
			}
			break;
	}
	return typeCheckStack;
}

// Retrieves the type check list from a list of tokens
vector<innerStack> getTypeCheckListFromTokens(vector<token> & tokens)
{
	vector<innerStack> typeCheckList;
	stack<token> scopesStack;
	for(int tokenIndex = 0; tokenIndex < tokens.size(); tokenIndex++)
	{
		token currentToken = tokens[tokenIndex];
		switch(currentToken.type)
		{
			case TT_IF:
			case TT_WHILE:
			case TT_ASSIGNMENT_OPERATOR:
			case TT_RETURN:
				typeCheckList.push_back(getTypeCheckStackForToken(tokens,tokenIndex,scopesStack));
				break;
			case TT_PROGRAM:
			case TT_PROCEDURE:
			case TT_FUNCTION:
				scopesStack.push(tokens[tokenIndex + 1]);
				break;
			case TT_END:
				scopesStack.pop();
				break;
		}
	}
	return typeCheckList;
}

// Debug functions

// Prints a message to the console in debug mode
void debugToConsole(string message, int textColor, int backgroundColor)
{
	cout << CUSTOM_OUTPUT_START << BOLD << SEPARATOR << textColor << SEPARATOR << backgroundColor << CUSTOM_OUTPUT_CONTINUE << message << CUSTOM_OUTPUT_END << endl;
}
