# include "Results.h"

Results::Results() {
	resArr[0][0] = "First: ";
	resArr[1][0] = "Second: ";
	resArr[2][0] = "Third: ";
	resArr[3][0] = "Fourth: ";
}

void Results::setPosition(Competitor c) {
	if (resArr[0][1].empty())
		resArr[0][1] = c.getTeam();
	else if (resArr[1][1].empty())
		resArr[1][1] = c.getTeam();
	else if (resArr[2][1].empty())
		resArr[2][1] = c.getTeam();
	else if (resArr[3][1].empty())
		resArr[3][1] = c.getTeam();
}

void Results::printResults() {
	for (int i = 0; i < 4; i++) {
		std::cout << resArr[i][0] << "Team " << resArr[i][1] << std::endl;
	}
}
