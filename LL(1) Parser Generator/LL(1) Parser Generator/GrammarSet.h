/************************************************************************
* Author:				Max Brandstetter and Aaron Wallace
* Date Created:			3/13/2016
* Last Modification Date:	3/14/2016
* Lab Number:			CST 320 Final Project
* Filename:				GrammarSet.h
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

class GrammarSet
{
	public:
		GrammarSet(); // Constructor
		~GrammarSet(); // Destructor
		void setNonTerm(string newNonTerm); // Set the nonterminal
		void addTerminal(string newTerm); // Add a terminal to the set
		void removeTerminal(int pos); // Remove a terminal from the set
		string getNonTerm(); // Return the nonterminal
		vector<string> getTermVector(); // Return the vector of terminals

	private:
		string m_nonTerm;
		vector<string> m_terms;
};