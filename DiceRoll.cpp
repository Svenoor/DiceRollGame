#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <iomanip>
#include <cstdlib>

using namespace std;

const unsigned NUM_PLAYERS = 4;
const unsigned NUM_ROUNDS = 6;

enum roll { ONE = 1, TWO, THREE, FOUR, FIVE, SIX };

// Function prototypes

// Opens input and output files based on user input.
// Preconditions: None.
// Postconditions: fin is opened for reading input file,
//                 fout is opened for writing output file.
void openFile(ifstream& fin, ofstream& fout);

// Reads a roll result for a player in a specific round from input stream in.
// Preconditions: in is an open input stream connected to a valid data file.
// Postconditions: player array is updated with the roll result for the specified round.
void readRoll(istream& in, roll player[], unsigned roundNum);

// Converts a roll enumeration value to its corresponding string representation.
// Preconditions: currentRoll is a valid roll enum value (ONE to SIX).
// Postconditions: Returns the string representation of currentRoll.
string rollToString(roll currentRoll);

// Converts a string representation of a roll to its corresponding enum value.
// Preconditions: currentString is a valid string representation of a roll (e.g., "One", "Two").
// Postconditions: Returns the roll enum value corresponding to currentString.
roll stringToRoll(string currentString);

// Calculates bonus points for each round based on the highest roll per round.
// Preconditions: results is a 2D array containing roll results for each player and round.
//                bonus is an array of size NUM_ROUNDS initialized to -1.
// Postconditions: bonus array is updated with indices of players receiving bonuses per round.
void calcBonus(const roll results[][NUM_ROUNDS], int bonus[]);

// Retrieves total bonus points accumulated by a specific player.
// Preconditions: bonus is an array of size NUM_ROUNDS containing bonus information.
// Postconditions: Returns the total bonus points earned by playerNum based on bonus array.
int getBonus(int bonus[], unsigned playerNum);

// Calculates the total score for a player including bonus points.
// Preconditions: result is an array of size NUM_ROUNDS containing roll results for a player.
//                bonusPoints is the total bonus points earned by the player.
// Postconditions: Returns the total score for the player after adding bonusPoints.
int calcScore(const roll result[], int bonusPoints);

// Writes the roll results for a player to the output stream out.
// Preconditions: out is an open output stream.
// Postconditions: Roll results for the player are written to the output stream.
void writeGame(ostream& out, const roll result[]);

// Reads all roll results from input file fin into results array.
// Preconditions: fin is an open input stream connected to a valid data file.
// Postconditions: results array is populated with roll results for all players and rounds.
void readGameResults(ifstream& fin, roll results[][NUM_ROUNDS]);

// Writes the total scores for both teams (Team A and Team B) to output stream fout.
// Preconditions: fout is an open output stream.
// Postconditions: Team scores are written to the output stream fout.
void writeTeamScores(ofstream& fout, const int teamPlayer1, const int teamPlayer2, const int teamPlayer3, const int teamPlayer4);

// Writes the bonus points awarded per round to output stream fout.
// Preconditions: fout is an open output stream.
// Postconditions: Bonus points per round are written to the output stream fout.
void writeBonusPerRound(ofstream& fout, const int bonusWinner[]);

int main() {
	ifstream fin;
	ofstream fout;
	openFile(fin, fout);

	roll results[NUM_PLAYERS][NUM_ROUNDS] = { ONE };
	readGameResults(fin, results);

	fout << "=====================\n"
		<< "Dice Rolling Game Results\n"
		<< "=====================\n";

	// Writing individual player results
	for (unsigned i = 0; i < NUM_PLAYERS; ++i) {
		fout << "Player " << i + 1 << ": ";
		writeGame(fout, results[i]);
		fout << "\n";
	}

	int bonusWinner[NUM_ROUNDS] = { -1 };
	calcBonus(results, bonusWinner);

	fout << "\nBonus Points per Round:\n";
	writeBonusPerRound(fout, bonusWinner);

	fout << '\n';

	// Calculating and writing team scores
	int teamPlayer1 = calcScore(results[0], getBonus(bonusWinner, 0)),
		teamPlayer2 = calcScore(results[1], getBonus(bonusWinner, 1)),
		teamPlayer3 = calcScore(results[2], getBonus(bonusWinner, 2)),
		teamPlayer4 = calcScore(results[3], getBonus(bonusWinner, 3));

	writeTeamScores(fout, teamPlayer1, teamPlayer2, teamPlayer3, teamPlayer4);

	// Determining and writing game result (win, lose, tie)
	if (teamPlayer1 + teamPlayer3 > teamPlayer2 + teamPlayer4) {
		fout << "Team A wins.\n";
	}
	else if (teamPlayer1 + teamPlayer3 < teamPlayer2 + teamPlayer4) {
		fout << "Team B wins.\n";
	}
	else {
		fout << "It's a tie!\n";
	}

	fin.close();
	fout.close();

	return 0;
}

void openFile(ifstream& fin, ofstream& fout) {
	string filename = "";
	unsigned openAttempts = 0;

	do {
		cout << "Roll data filename:\n";
		cin >> filename;

		fin.open(filename);

		if (!fin.is_open()) {
			cerr << "Could not open " << filename << ".\n";
			openAttempts++;
			if (openAttempts == 3) {
				cerr << "Program is exiting.\n";
				exit(1);
			}
		}
	} while (!fin.is_open());

	// Generating output filename based on input filename
	string outfile = "";
	size_t found = filename.rfind(".");
	if (found != filename.size()) {
		outfile = filename.substr(0, found);
	}
	else {
		outfile = filename;
	}
	outfile = outfile + ".rpt";
	fout.open(outfile);
}

void readRoll(istream& in, roll player[], unsigned roundNum) {
	string rollRead;
	getline(in, rollRead);
	player[roundNum] = stringToRoll(rollRead);
}

void readGameResults(ifstream& fin, roll results[][NUM_ROUNDS]) {
	for (unsigned k = 0; !fin.eof() && k < NUM_ROUNDS; ++k) {
		for (unsigned i = 0; !fin.eof() && i < NUM_PLAYERS; ++i) {
			readRoll(fin, results[i], k);
		}
	}
}

string rollToString(roll currentRoll) {
	switch (currentRoll) {
	case ONE: return "One";
	case TWO: return "Two";
	case THREE: return "Three";
	case FOUR: return "Four";
	case FIVE: return "Five";
	case SIX: return "Six";
	default: return "ERROR";
	}
}

roll stringToRoll(string currentString) {
	for (unsigned i = 0; i < currentString.size(); ++i) {
		currentString.at(i) = tolower(currentString.at(i));
	}

	if (currentString.size() == 0 || (currentString.at(0) == 'o' && currentString.size() == 1)) {
		return ONE;
	}

	string compare = "two";
	if (compare.find(currentString) == 0) {
		return TWO;
	}

	compare = "three";
	if (compare.find(currentString) == 0) {
		return THREE;
	}

	compare = "four";
	if (compare.find(currentString) == 0) {
		return FOUR;
	}

	compare = "five";
	if (compare.find(currentString) == 0) {
		return FIVE;
	}

	compare = "six";
	if (compare.find(currentString) == 0) {
		return SIX;
	}

	return ONE;
}

void calcBonus(const roll results[][NUM_ROUNDS], int bonus[]) {
	for (unsigned i = 0; i < NUM_ROUNDS; ++i) {
		int highestRoll = 0;
		int player = -1;

		// Finding the player with the highest roll in each round
		for (unsigned k = 0; k < NUM_PLAYERS; ++k) {
			if (static_cast<int>(results[k][i]) > highestRoll) {
				highestRoll = static_cast<int>(results[k][i]);
				player = k;
			}
		}

		// Assigning bonus to the player with the highest roll in round i
		if (player != -1) {
			bonus[i] = player;
		}
		else {
			bonus[i] = -1;
		}
	}
}

int getBonus(int bonus[], unsigned playerNum) {
	int points = 0;
	for (unsigned i = 0; i < NUM_ROUNDS; i++) {
		if (bonus[i] == static_cast<int>(playerNum))
			points += 5;
	}
	return points;
}

int calcScore(const roll result[], int bonusPoints) {
	int points = 0;
	for (unsigned i = 0; i < NUM_ROUNDS; ++i) {
		points += static_cast<int>(result[i]);
	}
	return points + bonusPoints;
}

void writeGame(ostream& out, const roll result[]) {
	for (unsigned i = 0; i < NUM_ROUNDS; ++i) {
		out << left << setw(6) << rollToString(result[i]);
	}
}

void writeTeamScores(ofstream& fout, const int teamPlayer1, const int teamPlayer2, const int teamPlayer3, const int teamPlayer4) {
	fout << '\n';
	fout << "Player 1 : " << right << setw(3) << teamPlayer1 << '\n'
		<< "Player 3 : " << right << setw(3) << teamPlayer3 << '\n'
		<< "      -----\n "
		<< "Team A" << right << setw(4) << teamPlayer1 + teamPlayer3 << '\n';

	fout << '\n';
	fout << "Player 2 : " << right << setw(3) << teamPlayer2 << '\n'
		<< "Player 4 : " << right << setw(3) << teamPlayer4 << '\n'
		<< "      -----\n "
		<< "Team B" << right << setw(4) << teamPlayer2 + teamPlayer4 << '\n';

	fout << '\n';
}

void writeBonusPerRound(ofstream& fout, const int bonusWinner[]) {
	for (unsigned i = 0; i < NUM_ROUNDS; ++i) {
		fout << "Round " << i + 1 << ": Player " << bonusWinner[i] + 1 << " gets a bonus of 5 points.\n";
	}
}
