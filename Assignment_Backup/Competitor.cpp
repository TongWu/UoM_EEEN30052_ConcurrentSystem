#include "Competitor.h"
#include <iostream>

Competitor::Competitor() {}

Competitor::Competitor(string tN, string pN) : teamName(tN), personName(pN) {}

void Competitor::setTeam(string tN) { teamName = tN; }
string Competitor::getTeam() { return teamName; }

void Competitor::setPerson(string pN) { personName = pN; }
string Competitor::getPerson() { return personName; }

Competitor Competitor::makeNull() {
    return *(new Competitor(" ", " "));
}

void Competitor::printCompetitor() {
    std::cout << "Team = " << teamName << " Person = " << personName << std::endl;
}