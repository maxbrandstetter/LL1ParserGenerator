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
#include <sstream>
#include <algorithm>
#include <iomanip>

using namespace std;


// Constructor, performs initial processing such as file reading and storage
ParserGenerator::ParserGenerator()
{
	start();
}

// Destructor
ParserGenerator::~ParserGenerator()
{}

void ParserGenerator::start()
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
	// Expand rules into all productions available from a rule
	expandRules();
	// Get the first set of each rule
	getFirstSet();
	// Get the follow set of each rule
	// Create the LL(1) parse table
	createTable();
}

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

						if (terminal == ".")
							continue; // Do nothing if it's a lambda production

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
	string buffer;
	int split_loc; // Location of the ">" in the string
	int space_loc; // Location of the space following the first terminal or nonterminal

	int nonTerms_size = m_nonTerms.size();
	int rule_size = m_rules.size();

	// Set nonterminals of first sets
	GrammarSet set;

	for (size_t i = 0; i < m_nonTerms.size(); ++i)
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
		
		// Check that the terminal we are operating is a nonterminal
		for (size_t idx = 0; idx < m_firstSets.size(); ++idx)
		{
			string checkNonterm = m_firstSets[idx].getNonTerm();
			
			// If the current first set matches the nonterminal of our rule, modify it by adding new terminals
			if (first_nonterm == checkNonterm)
			{
				// Split the item string by spaces to get just the terminals
				stringstream ss(buffer);
				istream_iterator<string> begin(ss);
				istream_iterator<string> end;
				vector<string> terms(begin, end); // And store them here

				// Check for a lambda production until every terminal has been added, or a lambda production is NOT found in the current terminal
				
				for (size_t yetAnotherIdx = 0; yetAnotherIdx < terms.size(); ++yetAnotherIdx)
				{
					m_firstSets[idx].addTerminal(terms[yetAnotherIdx]);

					if (!checkForLambda(terms[yetAnotherIdx]))
						break;
				}
				
			}
		}
	}

	replaceNonterms();
}

// Checks for a lambda production in the given nonterminal and returns true if one is found
bool ParserGenerator::checkForLambda(string nonTerm)
{
	// Check if the first term is a nonterminal
	if (find(m_nonTerms.begin(), m_nonTerms.end(), nonTerm) != m_nonTerms.end())
	{
		// Loop through the expanded rules
		for (size_t anotherIdx = 0; anotherIdx < m_ruleProductions.size(); ++anotherIdx)
		{
			// Check if the current expanded rule is for the current nonterminal
			if (m_ruleProductions[anotherIdx].getNonTerm() == nonTerm)
			{
				// Check for a lambda production in the current nonterminal
				vector<string> tempTerms = m_ruleProductions[anotherIdx].getTermVector();
				if (find(tempTerms.begin(), tempTerms.end(), ".") != tempTerms.end())
				{
					return true; // If there's a lambda production, return true
				}
				
			}
		}
	}
	
	return false; // Return false if there is no lambda production in the current terminalt
}

// Replace any nonterminals in the first sets
void ParserGenerator::replaceNonterms()
{
	// Loop through first sets and replace any terminals listed as nonterminals with their first sets
	size_t idx;
	bool replaced = true;
	vector<int> toRemove;

	for (idx = 0; idx < m_firstSets.size(); ++idx)
	{
		toRemove.clear();
		vector<string> terms = m_firstSets[idx].getTermVector();

		// Check each terminal
		for (size_t j = 0; j < terms.size(); ++j)
		{
			// See if it's a nonterminal
			if (find(m_nonTerms.begin(), m_nonTerms.end(), terms[j]) != m_nonTerms.end())
			{
				// If so, replace with contents of that nonterminal's first set
				vector<string> newTerms;
				toRemove.push_back(j);
				replaced = false;

				// Find the first set with the correct nonterminal
				for (size_t h = 0; h < m_firstSets.size(); ++h)
				{
					if (m_firstSets[h].getNonTerm() == terms[j])
						newTerms = m_firstSets[h].getTermVector();
				}

				// Add each terminal from the matched nonterminal to the first set in question
				for (size_t h = 0; h < newTerms.size(); ++h)
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

// Generates the LL(1) Table and outputs it to a file
void ParserGenerator::createTable()
{
	ofstream fout;
	fout.setf(ios::fixed);

	string filename;

	cout << "Please enter a name for the output file: ";
	cin >> filename;
	cout << endl;

	fout.open(filename);

	if (fout.fail())
	{
		cout << "Output file could not be created, aborting LL(1) table creation." << endl;
		return;
	}

	fout << left << setw(30) << "";

	for (size_t i = 0; i < m_terms.size(); ++i)
	{
		// Don't include lambda productions as part of the parse table headers
		if (m_terms[i] == ".")
			continue;
		fout << left << setw(30) << m_terms[i];
	}

	// Loop through the first sets, representing rows
	for (size_t i = 0; i < m_firstSets.size(); ++i)
	{
		fout << "\n" << left << setw(30) << m_firstSets[i].getNonTerm();
		// Loop through the terminals, representing columns
		for (size_t j = 0; j < m_terms.size(); ++j)
		{
			makeCell(fout, i, j);
		}
	}

	cout << filename << " was created successfully." << endl;
}

// Creates and fills a cell for the LL(1) table
void ParserGenerator::makeCell(ofstream &fout, int row, int column)
{
	bool written = false; // True if the function has written to the file, false otherwise
	
	// Loop through the vector of rules and operate on each rule
	for (size_t vectorIdx = 0; vectorIdx < m_rules.size(); ++vectorIdx)
	{
		string tempRule = m_rules[vectorIdx]; // Set nonTerm to the current rule

		size_t idx = tempRule.find('>'); // Get the position of >
		string nonTerm = tempRule.substr(0, idx - 1); // Set nonTerm to the nonterminal

		vector<string> firstTerms = m_firstSets[row].getTermVector();
		//vector<string> followTerms = m_followSets[row].getTermVector();

		// Make sure that the rule has the right nonterminal
		if (m_firstSets[row].getNonTerm() == nonTerm)
		{
			// Check if the nonterminal has a first set containing the column terminal
			if (find(firstTerms.begin(), firstTerms.end(), m_terms[column]) != firstTerms.end())
			{
				// Loop through expanded productions to find valid rules
				for (size_t i = 0; i < m_ruleProductions.size(); ++i)
				{
					// Only move on to writing if the rule corresponds to the expanded productions
					if (vectorIdx == i)
					{
						// Check that the rule production has the same nonterminal as the rule in use
						if (m_ruleProductions[i].getNonTerm() == nonTerm)
						{
							vector<string> tempVector = m_ruleProductions[i].getTermVector();
							// Check if the column terminal is in the productions
							if (find(tempVector.begin(), tempVector.end(), m_terms[column]) != tempVector.end())
							{
								if (written == false)
								{
									written = true;
									fout << left << setw(30) << m_rules[i]; // If so, write a cell to the file
								}
								else
									cout << "Grammar is not LL(1), clash in row " << row << " column " << column << ". Table creation will continue." << endl;
							}
						}
					}

				}
			}

			/* Lambda productions below; remove notation once follow set is finished
			// Check if the current nonterminal contains a lambda production in the first set
			if (find(firstTerms.begin(), firstTerms.end(), ".") != firstTerms.end())
			{
				// Check if the nonterminal has a follow set containing the column terminal
				if (find(followTerms.begin(), followTerms.end(), m_terms[column]) != followTerms.end())
				{
					if (written == false)
					{
						// Loop through the rules to find one with the same nonterminal
						for (size_t i = 0; i < m_rules.size(); ++i)
						{
							
							tempRule = m_rules[i]; // Set tempRule
							size_t idx = tempRule.find('>'); // Get the position of >
							string currentNonTerm = tempRule.substr(0, idx - 1); // Set nonTerminal to JUST the nonterminal
							tempRule = tempRule.substr(idx + 2); // Set tempRule to everything past the >

							if (currentNonTerm == m_nonTerms[row])
							{
								// Split the rule string by spaces to get just the terminals
								stringstream ss(tempRule);
								istream_iterator<string> begin(ss);
								istream_iterator<string> end;
								vector<string> terms(begin, end); // And store them here

								// Check if the current rule contains a lambda production
								if (find(terms.begin(), terms.end(), ".") != terms.end())
								{
									writtern = true;
									fout << left << setw(30) << m_rules[i];
								}
							}
						}
					}
				}
			}

			*/
		}
		else
			continue;
	}

	
	if (written == false)
		fout << left << setw(30) << ""; // Create an empty cell if nothing was written
}

// Expands all grammar rules by substituting nonterminals for their productions
void ParserGenerator::expandRules()
{
	string tempRule;
	vector<string> newTerms; // Store new "rules" here to be pushed
	vector<string>::iterator vectorItr;

	// Loop through the vector of rules and operate on each rule
	for (vectorItr = m_rules.begin(); vectorItr != m_rules.end(); ++vectorItr)
	{
		newTerms.clear();
		tempRule = *vectorItr; // Set tempRule to the current rule

		size_t idx = tempRule.find('>'); // Get the position of >
		string currentNonTerm = tempRule.substr(0, idx - 1); // Set nonTerminal to JUST the nonterminal
		tempRule = tempRule.substr(idx + 2); // Set tempRule to everything past the >

		// Split the rule string by spaces to get just the terminals
		stringstream ss(tempRule);
		istream_iterator<string> begin(ss);
		istream_iterator<string> end;
		vector<string> terms(begin, end); // And store them here

		// Loop through terms
		for (size_t i = 0; i < terms.size(); ++i)
		{
			// Check if the current term is a nonterminal
			if (find(m_nonTerms.begin(), m_nonTerms.end(), terms[i]) != m_nonTerms.end())
			{
				vector<string> tempTerms = ruleToTerms(terms[i], vectorItr); // Set newTerms to the result

				// Loop through tempTerms and push each terminal
				for (size_t j = 0; j < tempTerms.size(); ++j)
				{
					newTerms.push_back(tempTerms[j]);
				}
			}
			else
				newTerms.push_back(terms[i]);
		}

		GrammarSet toPush;
		toPush.setNonTerm(currentNonTerm);
		for (size_t i = 0; i < newTerms.size(); ++i)
		{
			toPush.addTerminal(newTerms[i]);
		}
		m_ruleProductions.push_back(toPush);
	}
}

// Smaller version of expandRules, returns the terminal productions from one rule
vector<string> ParserGenerator::ruleToTerms(string nonTerm, vector<string>::iterator originalRule)
{
	string tempRule;
	vector<string> newTerms; // Store new "rules" here to be pushed
	vector<string>::iterator vectorItr;

	// Loop through the vector of rules and operate on each rule
	for (vectorItr = m_rules.begin(); vectorItr != m_rules.end(); ++vectorItr)
	{
		tempRule = *vectorItr; // Set tempRule to the current rule

		size_t idx = tempRule.find('>'); // Get the position of >
		string currentNonTerm = tempRule.substr(0, idx - 1); // Set nonTerminal to JUST the nonterminal

		if ((currentNonTerm == nonTerm) && (vectorItr != originalRule))
		{
			tempRule = tempRule.substr(idx + 2); // Set tempRule to everything past the >

			// Split the rule string by spaces to get just the terminals
			stringstream ss(tempRule);
			istream_iterator<string> begin(ss);
			istream_iterator<string> end;
			vector<string> terms(begin, end); // And store them here

			// Loop through terms
			for (size_t i = 0; i < terms.size(); ++i)
			{
				// Check if the current term is a nonterminal
				if (find(m_nonTerms.begin(), m_nonTerms.end(), terms[i]) != m_nonTerms.end())
				{
					vector<string> tempTerms = ruleToTerms(terms[i], vectorItr); // Set newTerms to the result

					// Loop through tempTerms and push each terminal
					for (size_t j = 0; j < tempTerms.size(); ++j)
					{
						newTerms.push_back(tempTerms[j]);
					}
				}
				else
					newTerms.push_back(terms[i]);
			}

		}
		else 
			continue;
	}

	return newTerms; // Return after everything has been expanded
}