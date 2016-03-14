/***********************************************************
* Author:				Max Brandstetter and Aaron Wallace
* Date Created:			3/13/2016
* Last Modification Date:	3/14/2016
* Lab Number:			CST 320 Final Project
* Filename:				GrammarSet.cpp
*
* Overview:
*	Defines the member functions of the GrammarSet class.
*
************************************************************/

#include "GrammarSet.h"

using namespace std;


GrammarSet::GrammarSet()
{}

GrammarSet::~GrammarSet()
{}

// Set the nonterminal
void GrammarSet::setNonTerm(string newNonTerm)
{
	m_nonTerm = newNonTerm;
}

// Add a terminal to the set
void GrammarSet::addTerminal(string newTerm)
{
	// Check that the terminal isn't already part of the set
	if (find(m_terms.begin(), m_terms.end(), newTerm) != m_terms.end())
		; // If so, do nothing
	else
		m_terms.push_back(newTerm); // Otherwise, push the new terminal
}

// Remove a terminal from the set
void GrammarSet::removeTerminal(int pos)
{
	m_terms.erase(m_terms.begin() + pos);
}

// Return the nonterminal
string GrammarSet::getNonTerm()
{
	return m_nonTerm;
}

// Return the vector of terminals
vector<string> GrammarSet::getTermVector()
{
	return m_terms;
}