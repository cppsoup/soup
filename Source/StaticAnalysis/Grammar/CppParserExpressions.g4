grammar CppParserExpressions;
options { tokenVocab = CppLexer; }
import 
	CppParserLiterals;

/****************************************/
/* Primary Expressions
/* https://en.cppreference.com/w/cpp/language/expressions#Primary_expressions
/****************************************/
primaryExpression:
	literal |
	identifierExpression; // |
	//lambdaExpression | // C++ 11
	//foldExpression; // C++ 17

identifierExpression:
	unqualifiedIdentifier;// |
	//qualifiedIdentifier;

/****************************************/
/* Unqualified Identifier
/* https://en.cppreference.com/w/cpp/language/identifiers#Unqualified_identifiers
/****************************************/
unqualifiedIdentifier:
	Identifier |
	operatorFunctionIdentifier |
	conversionFunctionIdentifier |
	literalOperatorIdentifier |
	Tilde className |
	Tilde decltypeSpecifier |
	templateIdentifier;

/****************************************/
/* Operators
/* https://en.cppreference.com/w/cpp/language/identifiers#Unqualified_identifiers
/****************************************/
operatorFunctionIdentifier: 
	Operator anyOperator;

assignmentOperator:
	Equal |
	AsteriskEqual |
	ForwardSlashEqual |
	PercentEqual |
	PlusEqual |
	MinusEqual |
	DoubleGreaterThanEqual |
	DoubleLessThanEqual |
	AmpersandEqual |
	CaretEqual |
	VerticalBarEqual;

unaryOperator:
	VerticalBar |
	Asterisk |
	Ampersand |
	Plus |
	ExclamationMark |
	Tilde |
	Minus;

// Cannot be operator because it is a keyword in C#
anyOperator:
	New |
	Delete|
	New LeftBracket RightBracket |
	Delete LeftBracket RightBracket |
	Plus |
	Minus |
	Asterisk |
	Ampersand |
	Caret |
	Ampersand |
	VerticalBar |
	Tilde |
	ExclamationMark |
	LessThan |
	GreaterThan |
	Equal |
	AsteriskEqual |
	ForwardSlashEqual |
	PercentEqual |
	PlusEqual |
	MinusEqual |
	DoubleGreaterThanEqual |
	DoubleLessThanEqual |
	AmpersandEqual |
	CaretEqual |
	VerticalBarEqual |
	DoubleLessThan |
	DoubleGreaterThan|
	DoubleEqual |
	ExclamationMarkEqual |
	LessThanEqual |
	GreaterThanEqual |
	DoubleAmpersand |
	DoubleVerticalBar |
	DoublePlus |
	DoubleMinus |
	Comma |
	ArrowAsterisk |
	Arrow |
	LeftParenthesis RightParenthesis |
	LeftBracket RightBracket;

conversionFunctionIdentifier: 
	Operator conversionTypeIdentifier;

conversionTypeIdentifier:
	typeSpecifierSequence conversionDeclarator?;

conversionDeclarator:
	pointerOperator conversionDeclarator?;

pointerOperator:
	Asterisk attributeSpecifierSequence? constVirtualQualifierSequence? |
	Ampersand attributeSpecifierSequence? |
	DoubleAmpersand attributeSpecifierSequence? |
	nestedNameSpecifier Asterisk attributeSpecifierSequence? constVirtualQualifierSequence?;

constVirtualQualifierSequence:Delete;
nestedNameSpecifier:Delete;

literalOperatorIdentifier: Delete;
decltypeSpecifier: Delete;
templateIdentifier: Delete;
constantExpression: Delete;

/****************************************/
/* Qualified Identifier
/* https://en.cppreference.com/w/cpp/language/identifiers#Qualified_identifiers
/****************************************/
qualifiedIdentifier:;
//	nestedNameSpecifier Template? unqualifiedIdentifier;

//nestedNameSpecifier:
//	DoubleColon |
//	typeName DoubleColon |
//	namespaceName DoubleColon |
//	decltypeSpecifier DoubleColon |
//	nestedNameSpecifier identifier DoubleColon |
//	nestedNameSpecifier Template? simpleTemplateId DoubleColon;

/**************************/
/* Classes
/**************************/
className:
	Identifier |
	simpleTemplateIdentifier;

/**************************/
/* Templates
/**************************/
simpleTemplateIdentifier:
	templateName LessThan templateArgumentList? GreaterThan;

templateName:
	Identifier;

templateArgumentList:
	templateArgument Ellipsis? |
	templateArgumentList Comma templateArgument Ellipsis?;

templateArgument:
	typeIdentifier |
	constantExpression |
	identifierExpression;

/**************************/
/* Types
/**************************/
typeIdentifier:
	typeSpecifierSequence abstractDeclarator?;

typeSpecifierSequence:Delete;
abstractDeclarator:Delete;


/****************************************/
/* Attributes C++ 11
/****************************************/
attributeSpecifierSequence:
	attributeSpecifier |
	attributeSpecifierSequence attributeSpecifier;

attributeSpecifier:
	alignmentSpecifier |
	LeftBracket LeftBracket attributeList RightBracket RightBracket | // C++ 11
	LeftBracket LeftBracket Using attributeNamespace Colon attributeList RightBracket RightBracket; // C++ 17

alignmentSpecifier:
	AlignAs LeftParenthesis typeId Ellipsis? RightParenthesis |
	AlignAs LeftParenthesis constantExpression Ellipsis? RightParenthesis;

typeId: Delete;
constantExpression: Delete;

// Zero or more comma seperated attributes possibly ending in an ellipsis
attributeList:
	attribute? |
	attributeList Comma attribute? |
	attribute Ellipsis |
	attributeList Comma attribute Ellipsis;

attributeNamespace:
	Identifier;

attributeArgumentClause:
	LeftParenthesis argumentList RightParenthesis;

attribute:
	Identifier attributeArgumentClause? |
	attributeNamespace DoubleColon Identifier attributeArgumentClause?;

argumentList:; // TODO