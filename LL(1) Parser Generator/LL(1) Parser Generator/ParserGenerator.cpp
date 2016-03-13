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

	// Loop through the vector of rules and operate on each rule
	for (vectorItr = rules.begin(); vectorItr != rules.end(); ++vectorItr)
	{
		string firstRule = *vectorItr; // Set vectorItr to the current rule
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
							break;
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
				continue;
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
						while (currentRule[i] == storedRule[i])
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
					newRules.push_back(*ruleItr);
			}
				
			
		}

		// REMOVE OLD RULES X
		// ADD NEW NONTERMINAL X
		// ADD NEW RULE 
		// ADD RULE(S) FOR NEW NONTERMINAL
		// REPEAT

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
		string ruleToPush = firstNonterm + " > " + storedRule + newNonterm; // Create the rule from known components
		newRules.push_back(ruleToPush); // Push the new rule
 
		// Iterate through remaining rhs
		vector<string>::iterator rhsItr = newRhs.begin();
		while (rhsItr != newRhs.end())
		{
			ruleToPush = newNonterm + " > " + *rhsItr; // Create a new rule from rhs parts
			newRules.push_back(ruleToPush); // Push the new rule
			++rhsItr; // Increment
		}


	}
}