#include "Character.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#define COM_SIZE 5

//for given community finds character by given name and returns it.
//if the character with given name is dead, returns the alphabeticaly before name.

Character& search_by_name(Character (&com)[COM_SIZE], const string &name, const bool &checkIsAlive = true) {
	for(int i=0;i<COM_SIZE;i++)
		if(com[i].name == name) {
			if(checkIsAlive) {
				for(int j=i;j>=0;j--)
					if(com[j].isAlive)
						return com[j];
				for(int j=i+1;j<COM_SIZE;j++)
					if(com[j].isAlive)
						return com[j];
			}
			return com[i];
		}
	return com[0];
}

// for given community returns the character by given type.

Character& search_by_type(Character (&com)[COM_SIZE], const string &type) {
	for(auto &ch : com)
		if(ch.type == type)
			return ch;
	return com[0];
}

//updates all of the characters healthHistories and nRoundsSinceSpecial.
void record_health_history(Character (&coms)[2][COM_SIZE], const int &round) {
	for(auto &com : coms)
		for(auto &ch : com) {
			ch.healthHistory[round] = ch.remainingHealth;
			ch.nRoundsSinceSpecial++;
		}
}

int main(int argc, char* argv[]) {
	
		
		string winner = "Draw", attacker_name, defender_name, if_special;	//variables for winner, name of attacker, defender name and whether special round or not.
		ifstream input_file;												//input stream for reading input file.
		input_file.open(argv[1]);

		int nMaxRounds, round = 0, casualties[2] = {0, 0};					//an array for counting casualties of each community.

		string lastDeadMemberName[2] = {"", ""};							//keeping last dead member name for wizards special skill.
		input_file >> nMaxRounds;

		//2 by 5 array for storing each community and their members.
		Character coms[2][COM_SIZE] = {{{"","",0,0,0,nMaxRounds}, {"","",0,0,0,nMaxRounds}, {"","",0,0,0,nMaxRounds}, {"","",0,0,0,nMaxRounds}, {"","",0,0,0,nMaxRounds}},
										{{"","",0,0,0,nMaxRounds}, {"","",0,0,0,nMaxRounds}, {"","",0,0,0,nMaxRounds}, {"","",0,0,0,nMaxRounds}, {"","",0,0,0,nMaxRounds}}};


		vector<string> names;												//holding the original order of names to be used later with output stream.

		//for loop for initializing characters and sorting them alphabetically.	
		for(auto &com : coms) {
			for(auto &ch : com) {
				input_file >> ch.name >> ch.type >> ch.attack >> ch.defense >> ch.remainingHealth;
				ch.healthHistory[0] = ch.remainingHealth;
				names.push_back(ch.name);
			}
			for(int i=0;i<COM_SIZE-1;i++)
				for(int j=0;j<COM_SIZE-i-1;j++)
					if(com[j] < com[j+1])
						swap(com[j], com[j+1]);
		}

		//this is the part where fight begins and ends. 
		while(round < nMaxRounds) {
			const int attacker_com = round%2, defender_com = !attacker_com;		//indexes of rows in the matxrix for choosing attacer community and defender community.
			input_file >> attacker_name >> defender_name >> if_special;			//reads the attacker name, defender name and type of round.
			Character &attacker = search_by_name(coms[attacker_com], attacker_name), &defender = search_by_name(coms[defender_com], defender_name);	//finds the attacker and defender.
			int damage = attacker.attack - defender.defense;					//calculates the damage.

			if(if_special == "SPECIAL") {

				//does the special skill of elves.
				if(attacker.type == "Elves" && attacker.nRoundsSinceSpecial >= 11) {
					attacker.nRoundsSinceSpecial = 0;
					const int conveyed_health = attacker.remainingHealth/2;
					attacker.remainingHealth -= conveyed_health;
					search_by_type(coms[attacker_com], "Hobbit").remainingHealth += conveyed_health;
				}

				//does the special skill of Dwarfs.
				else if(attacker.type == "Dwarfs" && attacker.nRoundsSinceSpecial >= 21) {
					attacker.nRoundsSinceSpecial = 0;
					damage *= 2;
				}
				//does the special skill of wizards.
				else if(attacker.type == "Wizards" && attacker.nRoundsSinceSpecial >= 51) {
					if(!lastDeadMemberName[attacker_com].empty()) {
						attacker.nRoundsSinceSpecial = 0;
						Character &dead_guy = search_by_name(coms[attacker_com], lastDeadMemberName[attacker_com], false);
						if(!dead_guy.isAlive) {
							casualties[attacker_com]--;
							dead_guy.isAlive = true;
						}
						dead_guy.remainingHealth = dead_guy.healthHistory[0];
					}
				}
			}

			//if damage > 0 attackers attack and defenders health drop by damage. and checks the winning conditions. checks whether if anyone dead or not.
			if(damage > 0){
				defender.remainingHealth -= damage;
				if((defender.remainingHealth) <= 0) {
					defender.isAlive = false;
					defender.remainingHealth = 0;
					casualties[defender_com]++;
					lastDeadMemberName[defender_com] = defender.name;
					if(defender.type == "Hobbit") {
						winner = attacker_com ? "Community-1" : "Community-2";
						break;
					}
					else if(casualties[defender_com] >= COM_SIZE-1) {
						winner = attacker_com ? "Community-2" : "Community-1";
						record_health_history(coms, ++round);
						break;
					}
				}
			}
			//updates every characters healthhistory.
			record_health_history(coms, ++round);
		}

		input_file.close();

		ofstream output_file;					//outstream for printing the results.
		output_file.open(argv[2]);

		output_file << winner << endl << round << endl << (casualties[0] + casualties[1]) << endl;

		//pritns all character by given order at the beginning and prints heahlthHistory of characters.
		for(int i=0;i<2*COM_SIZE;i++) {
			output_file << names[i] << " ";
			const Character &ch = search_by_name(coms[i/COM_SIZE], names[i], false);
			for(int j=0;j<=round;j++){
				output_file << ch.healthHistory[j] << " ";
			}
			output_file << endl;
		}
		output_file.close();

	
	return 0;
}
