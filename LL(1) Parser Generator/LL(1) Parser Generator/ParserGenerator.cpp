/***********************************************************
* Author:				Max Brandstetter and Aaron Wallace
* Date Created:			3/11/2016
* Last Modification Date:	3/14/2016
* Lab Number:			CST 320 Final Project
* Filename:				ParserGenerator.cpp
*
* Overview:
*	Defines the member functions of the ParserGenerator class.
*
************************************************************/

#include "ParserGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;


// Constructor, performs initial processing such as file reading and storage
ParserGenerator::ParserGenerator()
{
	cout << "Welcome to the LL(1) Parser Generator!" << endl;
	cout << "Make sure your grammar follows the following format:" << endl << endl;
	cout << "'S > a B c' Nonterminal first, then a >," << endl;
	cout << "then the contents of the rule (nonterminals and terminals)." << endl << endl;
	cout << "Each rule must be on a separate line." << endl;
	cout << "Rules with the same nonterminal must be listed in succession." << endl;
	cout << "Every terminal/nonterminal must be separated by a space." << endl;
	cout << "Lambda productions are represented by a period (.)." << endl;
	cout << "If the grammar contains left recursion, the generated table may be incorrect." << endl << endl;

	// Retrieve all components necessary for generating an LL(1) table
	getRules();
	getNonterm();
	getTerm();
	// Left factor the rules
	leftFactor(m_rules);
	getFirstSet();
}

// Destructor
ParserGenerator::~ParserGenerator()
{}

// Opens and processes the given file, stores grammar rules
void ParserGenerator::getRules()
{
	string filename;
	string line;

	cout << "Please enter the file name for your grammar (i.e. test.txt): ";
	cin >> filename;
	cout << endl;

	ifstream in(filename, ios::in | ios::binary);

	// Ensure the file is open
	if (in.is_open())
	{
		while (getline(in, line))
		{
			// Check for and remove \r on Windows systems, since getline reads up to \n
			if (line[line.size() - 1] == '\r')
				line.resize(line.size() - 1);

			m_rules.push_back(line); // Push each line into the vector of rules
		}

		in.close();
	}
	else
	{
		cout << "File not found, exiting function" << endl;
		return;
	}
}

// Goes through the stored rules and stores all nonterminals
void ParserGenerator::getNonterm()
{
	vector<string>::iterator vectorItr;

	// Loop through the vector of rules and operate on each rule
	for (vectorItr = m_rules.begin(); vectorItr != m_rules.end(); ++vectorItr)
	{
		string temp = *vectorItr; // Set temp to the current rule

		size_t idx = temp.find('>'); // Get the position of >
		temp = temp.substr(0, idx - 1); // Set temp to JUST the nonterminal

		// If the nonterminal is already stored, move on
		if (find(m_nonTerms.begin(), m_nonTerms.end(), temp) != m_nonTerms.end())
			continue;
		else
			m_nonTerms.push_back(temp); // Otherwise, push the nonterminal
	}
}

// Goes through the stored rules and stores all terminals
void ParserGenerator::getTerm()
{
	vector<string>::iterator vectorItr;

	// Loop through the vector of rules and operate on each rule
	for (vectorItr = m_rules.begin(); vectorItr != m_rules.end(); ++vectorItr)
	{
		string temp = *vectorItr; // Set temp to the current rule

		size_t idx = temp.find('>'); // Get the position of >
		temp = temp.substr(idx + 2); // Set temp to everything past the >

		// Loop through the right hand side of the rule to find terminals
		for (unsigned int i = 0; i < temp.size(); ++i)
		{
			int currentPos = i;

			// Begin iteration if a potential terminal is found
			while (!isspace(temp[i]) && temp[i] != NULL)
			{
				++i;

				// Check that the end of the terminal has been reached
				if (isspace(temp[i]) || temp[i] == '\0')
				{
					string terminal = temp.substr(currentPos, i - currentPos); // Separate the terminal

					// If the terminal is already stored, move on
					if (find(m_terms.begin(), m_terms.end(), terminal) != m_terms.end())
						continue;
					else
					{
						// Check if the terminal in question is a nonterminal
						if (find(m_nonTerms.begin(), m_nonTerms.end(), terminal) != m_nonTerms.end())
							continue; // Do nothing
						m_terms.push_back(terminal); // Otherwise, push the terminal
					}
				}
			}
		}
	}
}

// Left factors the grammar based on the stored rules
void ParserGenerator::leftFactor(vector<string> rules)
{
	string storedRule; // Stores a matched rule, if found, for left factoring
	vector<string> newRhs; // Store remaining rhs of rules to be factored
	vector<string> newRules; // Stores new rules, used in recursion to check for proper left factoring
	vector<string>::iterator vectorItr;
	vector<string>::iterator storedItr = rules.end(); // Use as storage throughout function, for resetting

	bool factored = true; // When true, the grammar is properly left factored and the recursion will exit

	// Loop through the vector of rules and operate on each rule
	for (vectorItr = rules.begin(); vectorItr != rules.end(); ++vectorItr)
	{
		if (storedItr != rules.end())
		{
			vectorItr = storedItr; // Set the current vectorItr to the stored position
			storedItr = rules.end(); // Set to rules.end() to reset for next iteration
		}

		string firstRule = *vectorItr; // Set vectorItr to the current rule
		storedRule.clear(); // Empty the stored rule for the new iteration
		string firstNonterm; // Store the first nonterminal
		vector<string>::iterator compareItr;

		size_t idx = firstRule.find('>'); // Get the position of >
		firstNonterm = firstRule.substr(0, idx - 1); // Set the nonterminal
		firstRule = firstRule.substr(idx + 2); // Set everything past the >

		// Loop through all other rules for comparison to find instances of left factoring
		for (compareItr = vectorItr + 1; compareItr != rules.end(); ++compareItr)
		{
			string secondRule = *compareItr; // Set compareItr to the current rule

			size_t idx = secondRule.find('>'); // Get the position of >
			string secondNonterm = secondRule.substr(0, idx - 1); // Set the nonterminal
			secondRule = secondRule.substr(idx + 2); // Set everything past the >

			if (firstNonterm == secondNonterm) // Check that the nonterminals are the same
			{
				if (storedRule.empty())
				{
					// Loop through the right hand side of both rules
					for (unsigned int i = 0; (i < firstRule.size()) && (i < secondRule.size()); ++i)
					{
						// If they are the same, keep iterating until they differ
						if (firstRule[i] == secondRule[i])
						{
							factored = false; // Set to false since left factoring is needed
							size_t startPos = i;
							// Iterate...
							while (firstRule[i] == secondRule[i])
							{
								++i;
							}

							storedRule = firstRule.substr(startPos, i); // Store the rule in question
							break;
						}
						else
						{
							break;
						}
					}
				}
				else
				{
					// Loop through the right hand side of both rules
					for (unsigned int i = 0; (i < storedRule.size()) && (i < secondRule.size()); ++i)
					{
						// If they are the same, keep iterating until they differ
						if (storedRule[i] == secondRule[i])
						{
							size_t startPos = i;
							// Iterate...
							while (storedRule[i] == secondRule[i])
							{
								++i;
							}

							storedRule = storedRule.substr(startPos, i); // Store the rule in question
							break;
						}
						else
							break;
					}
				}
			}
			else
			{
				storedItr = compareItr;
				break;
			}
		}

		vector<string>::iterator ruleItr;
		// Loop a second time to store all rules minus the stored rule
		for (ruleItr = rules.begin(); ruleItr < rules.end(); ++ruleItr)
		{
			string currentRule = *ruleItr; // Set vectorItr to the current rule
			size_t idx = currentRule.find('>'); // Get the position of >
			string currentNonterm = currentRule.substr(0, idx - 1); // Set the nonterminal
			currentRule = currentRule.substr(idx + 2); // Set everything past the >

			// Loop through the rule in question and the stored rule
			for (unsigned int i = 0; (i < currentRule.size()) && (i < storedRule.size()); ++i)
			{
				if (currentNonterm == firstNonterm) // Check that the nonterminals are the same
				{
					// If they are the same, keep iterating until they differ
					if (currentRule[i] == storedRule[i])
					{
						size_t startPos = i;
						// Iterate...
						while ((currentRule[i] == storedRule[i]) && (currentRule[i] != '\0')) 
						{
							++i;
						}

						// Check if the new rule already exists
						if (find(newRhs.begin(), newRhs.end(), currentRule.substr(i)) != newRhs.end())
							; // If so, do nothing
						else
						{
							newRhs.push_back(currentRule.substr(i)); // Otherwise, store what remains of the rhs after removing the stored rule
						}
						
						break;
					}
					else
					{
						newRules.push_back(*ruleItr);
						break;
					}
				}
				else
				{
					newRules.push_back(*ruleItr);
					break;
				}
			}
				
			
		}

		// Only iterate if there is a stored rule and are trying to left factor
		if (!storedRule.empty())
		{
			// Add a new nonterminal for our left factored rules
			bool nontermCreated = false;
			string newNonterm;

			while (nontermCreated == false)
			{
				newNonterm = firstNonterm + '\'';

				// Check if the new nonterminal already exists
				if (find(m_nonTerms.begin(), m_nonTerms.end(), newNonterm) != m_nonTerms.end())
					; // If so, do nothing
				else
				{
					m_nonTerms.push_back(newNonterm); // Push new non terminal
					nontermCreated = true; // Set to true to exit loop
				}
			}

			// Merge the changed rules into one with the new nonterminal at the end; for left factoring
			string ruleToPush;
			if (isspace(storedRule[storedRule.size() - 1]))
				ruleToPush = firstNonterm + " > " + storedRule + newNonterm; // Create the rule from known components
			else
				ruleToPush = firstNonterm + " > " + storedRule + " " + newNonterm; // Create the rule from known components
			newRules.push_back(ruleToPush); // Push the new rule
 
			// Iterate through remaining rhs
			vector<string>::iterator rhsItr = newRhs.begin();
			while (rhsItr != newRhs.end())
			{
				if (*rhsItr == "")
					ruleToPush = newNonterm + " > ."; // Create lambda rule if the rhsItr is an empty string
				else
				{
					string rhsString = *rhsItr;
					if (isspace(rhsString[0]))
						ruleToPush = newNonterm + " >" + rhsString;
					else
						ruleToPush = newNonterm + " > " + *rhsItr; // Create a new rule from rhs parts
				}
				newRules.push_back(ruleToPush); // Push the new rule
				++rhsItr; // Increment
			}
		}


	}

	if (factored)
	{
		m_rules = rules; // Set the member variable to the left factored grammar
		return;
	}
	else
		leftFactor(newRules);
}

// Uses the left factored rules to generate the First set
void ParserGenerator::getFirstSet()
{
	string first_nonterm;
	string str_rule;
	string first_item;
	string buffer;
	int split_loc; // Location of the ">" in the string
	int space_loc; // Location of the space following the first terminal or nonterminal

	int nonTerms_size = m_nonTerms.size();
	int rule_size = m_rules.size();

	// Set nonterminals of first sets
	GrammarSet set;

	for (int i = 0; i < m_nonTerms.size(); ++i)
	{
		set.setNonTerm(m_nonTerms[i]);
		m_firstSets.push_back(set);
	}

	for (int i = 0; i < rule_size; ++i)
	{
		// Get the nonterminal
		str_rule = m_rules[i];
		split_loc = str_rule.find(">");
		first_nonterm = str_rule.substr(0, split_loc - 1);

		// Get the terminals (RHS)
		buffer = str_rule.substr(split_loc + 2);
		space_loc = buffer.find(" ");
		first_item = buffer.substr(0, space_loc);
		
		// Check that the terminal we are operating is a nonterminal
		for (int idx = 0; idx < m_firstSets.size(); ++idx)
		{
			string checkNonterm = m_firstSets[idx].getNonTerm();
			
			// If the current first set matches the nonterminal of our rule, modify it by adding new terminals
			if (first_nonterm == checkNonterm)
			{
				// Split the item string by spaces to get just the terminals
				stringstream ss(first_item);
				istream_iterator<string> begin(ss);
				istream_iterator<string> end;
				vector<string> terms(begin, end); // And store them here

				// Push the terminal
				m_firstSets[idx].addTerminal(terms[0]);
			}
		}
	}

	replaceNonterms();
}

// Replace any nonterminals in the first sets
void ParserGenerator::replaceNonterms()
{
	// Loop through first sets and replace any terminals listed as nonterminals with their first sets
	int idx;
	bool replaced = true;
	vector<int> toRemove;

	for (idx = 0; idx < m_firstSets.size(); ++idx)
	{
		toRemove.clear();
		vector<string> terms = m_firstSets[idx].getTermVector();

		// Check each terminal
		for (int j = 0; j < terms.size(); ++j)
		{
			// See if it's a nonterminal
			if (find(m_nonTerms.begin(), m_nonTerms.end(), terms[j]) != m_nonTerms.end())
			{
				// If so, replace with contents of that nonterminal's first set
				vector<string> newTerms;
				toRemove.push_back(j);
				replaced = false;

				// Find the first set with the correct nonterminal
				for (int h = 0; h < m_firstSets.size(); ++h)
				{
					if (m_firstSets[h].getNonTerm() == terms[j])
						newTerms = m_firstSets[h].getTermVector();
				}

				// Add each terminal from the matched nonterminal to the first set in question
				for (int h = 0; h < newTerms.size(); ++h)
				{
					m_firstSets[idx].addTerminal(newTerms[h]);
				}

			}
		}

		// Remove the replaced nonterminals as stored in toRemove
		for (int j = toRemove.size() - 1; j >= 0; --j)
		{
			m_firstSets[idx].removeTerminal(toRemove[j]);
		}
	}

	if (replaced == false)
		replaceNonterms(); // Recurse if a replacement occurred to check for other replacements
}