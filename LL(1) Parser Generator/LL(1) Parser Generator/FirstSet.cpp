#include "ParserGenerator.h"

#include <iostream>
#include <sstream>
#include <algorithm>



using namespace std;


void ParserGenerator::displayLFrules()
{

/*	vector<string>::iterator vectorItr;
	int rule_size = m_rules.size();
	cout << "**********m_rules**********" << endl;
	for (int i = 0; i < rule_size; ++i)
	{
		
		cout << m_rules[i] << endl;
	} 


	int nonTerms_size = m_nonTerms.size();
	cout << "**********m_nonTerms**********" << endl;
	for (int i = 0; i < nonTerms_size; ++i)
	{

		cout << m_nonTerms[i] << endl;
	}


	int terms_size = m_terms.size();
	cout << "**********m_terms**********" << endl;
	for (int i = 0; i < terms_size; ++i)
	{

		cout << m_terms[i] << endl;
	}
	*/
}

void ParserGenerator::getFirstSet()
{
	string my_string;
	string dash = "-";
	




	string first_nonterm;
	string str_rule;
	string first_item;
	string buffer;
	


	int split_loc; // Location of the ">" in the string
	int space_loc; // Location of the space following the first terminal or nonterminal
	int diff; // The length between split_loc and space_loc


	int nonTerms_size = m_nonTerms.size();
	int rule_size = m_rules.size();
	for (int i = 0; i < rule_size; ++i)
	{
		str_rule = m_rules[i];
		split_loc = str_rule.find(">");
		first_nonterm = str_rule.substr(0,split_loc - 1);

		//cout << "First nonTerm: " << first_nonterm << endl;

		buffer = str_rule.substr(split_loc+2);
		//cout << "Buffer: " << buffer << endl;
		space_loc = buffer.find(" ");
		first_item = buffer.substr(0, space_loc);
		//cout << "First(" << first_item << ")" << endl;
		
		for (int idx = 0; idx < nonTerms_size; ++idx)
		{
			my_string = m_nonTerms[idx];
			
			if (first_nonterm == my_string)
			{
				my_string.append(" ");

			/*	if(find(m_nonTerms.begin(), m_nonTerms.end(), first_item) != m_nonTerms.end())
				{
					cout << "FOUND IT" << endl;
					int index = 0;
					while (index < m_nonTerms.size())
					{



					}
				}*/

				my_string.append(first_item);

				my_string.append(" ");
				cout << my_string << endl;
				m_firstSet.push_back(my_string);
			}
		}

		
	}
	cout << "*********************************************" << endl;

	int first_set_size = m_firstSet.size();
	for (int idx = 0; idx < first_set_size; ++idx)
	{
		cout << m_firstSet[idx] << endl;
	}


}