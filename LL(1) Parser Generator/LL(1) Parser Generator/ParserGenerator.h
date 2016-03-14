/************************************************************************
* Author:				Max Brandstetter and Aaron Wallace
* Date Created:			3/11/2016
* Last Modification Date:	3/14/2016
* Lab Number:			CST 320 Final Project
* Filename:				ParserGenerator.h
*
* Class: ParserGenerator
*
* Constructors:	
*	
*
* Methods:		
*		
*
*************************************************************************/

#include <string>
#include <vector>

using namespace std;

class ParserGenerator
{
	public:
		ParserGenerator(); // Constructor, performs initial processing such as file reading and storage
		~ParserGenerator(); // Destructor
		void getRules(); // Opens and processes the given file, stores grammar rules
		void getNonterm(); // Goes through the stored rules and stores all nonterminals
		void getTerm(); // Goes through the stored rules and stores all terminals
		void leftFactor(vector<string> rules); // Left factors the grammar based on the stored rules
		void getFirstSet(); // Uses the left factored rules to generate the First set
		void displayLFrules(); 
		//string get_first_nonterm();

	private:
		vector<string> m_rules; // Stores grammar rules
		vector<string> m_nonTerms; // Stores nonterminals of the grammar
		vector<string> m_terms; // Stores terminals of the grammar
		vector<string> m_factoredRules; // Stores all left factored grammar rules
		vector<string> m_firstSet; //Stores the first set of all rules

};