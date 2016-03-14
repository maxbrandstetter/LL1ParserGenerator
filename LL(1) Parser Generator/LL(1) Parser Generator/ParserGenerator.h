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

#include "GrammarSet.h"

#include <string>
#include <vector>
#include <fstream>

using namespace std;

class ParserGenerator
{
	public:
		ParserGenerator(); // Constructor
		~ParserGenerator(); // Destructor
		void start(); // Runs all necessary processes to generate an LL(1) parse table
		void getRules(); // Opens and processes the given file, stores grammar rules
		void getNonterm(); // Goes through the stored rules and stores all nonterminals
		void getTerm(); // Goes through the stored rules and stores all terminals
		void leftFactor(vector<string> rules); // Left factors the grammar based on the stored rules
		void getFirstSet(); // Uses the left factored rules to generate the first sets
		bool checkForLambda(string nonTerm); // Checks for a lambda production in the given nonterminal and returns true if one is found 
		void replaceNonterms(); // Replace any nonterminals in the first sets
		void createTable(); // Generates the LL(1) Table and outputs it to a file
		void makeCell(ofstream &fout, int row, int column); // Creates and fills a cell for the LL(1) table
		void expandRules(); // Expands all grammar rules by substituting nonterminals for their productions
		vector<string> ruleToTerms(string nonTerm, vector<string>::iterator originalRule); // Smaller version of expandRules, returns the terminal productions from one rule

	private:
		vector<string> m_rules; // Stores grammar rules
		vector<string> m_nonTerms; // Stores nonterminals of the grammar
		vector<string> m_terms; // Stores terminals of the grammar
		vector<string> m_factoredRules; // Stores all left factored grammar rules
		vector<GrammarSet> m_firstSets; //Stores the first set of all rules
		vector<GrammarSet> m_ruleProductions; // Stores full productions of all rules
};