#include "Character.h"

Character::Character(string _name, string _type, int _attack, int _defense, int _remainingHealth, int _nMaxRounds) {	
	this->name=_name;
	this->type=_type;
	this->attack=_attack;
	this->defense=_defense;
	this->remainingHealth=_remainingHealth;
	this->nMaxRounds=_nMaxRounds;
	this->nRoundsSinceSpecial=0;
	this->healthHistory=new int[_nMaxRounds+1];
}

Character::Character(const Character& character) : isAlive(character.isAlive), name(character.name), type(character.type), attack(character.attack), defense(character.defense),
remainingHealth(character.remainingHealth), nMaxRounds(character.nMaxRounds), nRoundsSinceSpecial(character.nRoundsSinceSpecial), healthHistory(new int[nMaxRounds+1]) {
	isAlive=character.isAlive;
	name=character.name;
	type=character.type;
	attack=character.attack;
	defense=character.defense;
	remainingHealth=character.remainingHealth;
	nMaxRounds=character.nMaxRounds;
	nRoundsSinceSpecial=character.nRoundsSinceSpecial;
	for(int i=0;i<nMaxRounds+1;i++)
		healthHistory[i] = character.healthHistory[i];
}

Character& Character::operator=(const Character& character) {
	isAlive = character.isAlive;
	name = character.name;
	type = character.type;
	attack = character.attack;
	defense = character.defense;
	remainingHealth = character.remainingHealth;
	nMaxRounds = character.nMaxRounds;
	nRoundsSinceSpecial = character.nRoundsSinceSpecial;
	delete [] healthHistory;
	healthHistory = new int[nMaxRounds+1];
	for(int i=0;i<nMaxRounds+1;i++)
		healthHistory[i] = character.healthHistory[i];
	return *this;
}

bool Character::operator<(const Character& other) {
	return name < other.name;
}

Character::~Character() {
	delete [] healthHistory;
}
