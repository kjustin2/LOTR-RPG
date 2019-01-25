#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// Initial includes

// Struct for saving and loading
struct gameState {
	char name[30];
	int hp;
	int armor;
	int weapon;
	int level;
	int xp;
	int ehp[10];
	int earmor[10];
	int eweapon[10];
	int elevel[10];
	int exp[10];
};

// Method declarations
void loadSave();
void look();
void introReg();
void setup();
void save();
void displayChar();
void fight(int);
int mypow(int, int);
void sigterm2(int);
void sigint2(int);
void sigrtmin2(int);
void respawn(int);

// Global arrays necessary
char armors[5][20];
char weapons[5][20];
int armorStat[5];
char enemies[10][10];

struct gameState thisGame;
int main() {
	// Setup for signals
	signal(SIGTERM, sigterm2);
	signal(SIGINT, sigint2);
	signal(SIGRTMIN, sigrtmin2);

	// Check for save file
	const char *inFile = "rpg.save";
	char saveAn = 'a';
	char c = 'a';
	int didLoad = 0;
	// Saving working
	if (access(inFile, F_OK) != -1) {
		printf("Found save file. Continue where you left off (y/n)? ");
		scanf("%c", &saveAn);
		do { scanf("%c", &c); } while (c != '\n');
		if (saveAn == 'y') {
			loadSave();
			didLoad = 1;
			printf("\n");
		}
	}
	// End of saving intro

	strcpy(armors[0], "cloth");
	strcpy(armors[1], "studded leather");
	strcpy(armors[2], "ring mail");
	strcpy(armors[3], "chain mail");
	strcpy(armors[4], "plate");
	strcpy(weapons[0], "dagger");
	strcpy(weapons[1], "short sword");
	strcpy(weapons[2], "long sword");
	strcpy(weapons[3], "great sword");
	strcpy(weapons[4], "great axe");
	strcpy(enemies[0], "Sauron");
	int q = 0;
	for (q = 1; q < 9; q++) {
		strcpy(enemies[q], "Orc");
	}
	strcpy(enemies[9], "Gollum");
	int i = 10;
	int w = 0;
	for (i = 10; i < 20; i = i + 2) {
		armorStat[w] = i;
		w++;
	}

	// Check if the player chose to load a save, different intros
	if (!didLoad) {
		introReg();

		printf("List of available armors:\n");
		printf("0: cloth (AC=10)\n");
		printf("1: studded leather (AC=12)\n");
		printf("2: ring mail (AC=14)\n");
		printf("3: chain mail (AC=16)\n");
		printf("4: plate (AC=18)\n\n");


		printf("Choose %s's Armor (0~4): ", thisGame.name);
		scanf("%d", &thisGame.armor);
		do { scanf("%c", &c); } while (c != '\n');

		printf("\nList of available weapons:\n");
		printf("0: dagger (damage=1d4)\n");
		printf("1: short sword (damage=1d6)\n");
		printf("2: long sword (damage=1d8)\n");
		printf("3: great sword (damage=2d6)\n");
		printf("4: great axe (damage=1d12)\n\n");

		printf("Choose %s's Weapon (0~4): ", thisGame.name);
		scanf("%d", &thisGame.weapon);
		do { scanf("%c", &c); } while (c != '\n');

		printf("\nPlayer setting complete:\n");
		printf("[%s: hp=20, armor=%s, weapon=%s, level=1, xp=2000]\n", thisGame.name, armors[thisGame.armor], weapons[thisGame.weapon]);
		setup();
		printf("\n");
	}
	// See which option the player chooses
	char quit[8] = "quit";
	char stats[8] = "stats";
	char look2[8] = "look";
	char fight2[8] = "fight";
	int fightChoice = 0;
	look();
	// Actual game loop
	while (1) {
		printf("command >> ");
		char option[8] = {};
		scanf("%s", &option);
		if (strcmp(option, quit) == 0) {
			save();
			break;
		}
		else if (strcmp(option, stats) == 0) {
			displayChar();
			do { scanf("%c", &c); } while (c != '\n');
			continue;
		}
		else if (strcmp(option, look2) == 0) {
			look();
			do { scanf("%c", &c); } while (c != '\n');
			continue;
		}
		else if (strcmp(option, fight2) == 0) {
			scanf("%d", &fightChoice);
			fight(fightChoice);
			continue;
		}
		printf("Not a real command, try again\n");
	}
	return 0;
}

// Goes to /dev/dice to get how much damage is done randomly
int getDamage(int playerWeapon) {
	// Open the device driver
	int fd = open("/dev/dice", O_RDWR);
	int value = 0;
	int value2 = 0;
	char in;
	switch (playerWeapon) {
	case(0):
		in = 4;
		write(fd, &in, 1); 
		read(fd, &in, 1);
		value = in;
		close(fd);
		return value;
	case(1):
		in = 6;
		write(fd, &in, 1); 
		read(fd, &in, 1);
		value = in;
		close(fd);
		return value;
	case(2):
		in = 8;
		write(fd, &in, 1); 
		read(fd, &in, 1);
		value = in;
		close(fd);
		return value;
	case(3):
		in = 4;
		write(fd, &in, 1); 
		read(fd, &in, 1);
		value = in;
		read(fd, &in, 1);
		value2 = in;
		close(fd);
		return value + value2;
	case(4):
		in = 12;
		write(fd, &in, 1); 
		int a = read(fd, &in, 1);
		value = in;
		close(fd);
		return value;
	}
	return value;

}

// Basic load in struct information
void loadSave() {
	FILE *myFile = fopen("rpg.save", "rb");
	fread(&thisGame, sizeof(struct gameState), 1, myFile);
	fclose(myFile);
}

// Show all enemies and their healths
void look() {
	printf("All is peaceful in the land of Mordor.\n");
	printf("Sauron and his minions are blissfully going about their business:\n");
	int i = 0;
	printf("0: [Sauron: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", thisGame.ehp[0], armors[thisGame.earmor[0]], weapons[thisGame.eweapon[0]], thisGame.elevel[0], thisGame.exp[0]);
	for (i = 1; i < 9; i++) {
		printf("%d: [Orc %d: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", i, i, thisGame.ehp[i], armors[thisGame.earmor[i]], weapons[thisGame.eweapon[i]], thisGame.elevel[i], thisGame.exp[i]);
	}
	printf("9: [Gollum: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", thisGame.ehp[9], armors[thisGame.earmor[9]], weapons[thisGame.eweapon[9]], thisGame.elevel[9], thisGame.exp[9]);
	printf("Also at the scene are some adventurers looking for trouble:\n");
	printf("0: [%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", thisGame.name, thisGame.hp, armors[thisGame.armor], weapons[thisGame.weapon], thisGame.level, thisGame.xp);
}

// The regular intro
void introReg() {
	char c = 'a';
	printf("What is your name?\n");
	char name2[30];
	scanf("%s", &name2);
	do { scanf("%c", &c); } while (c != '\n');
	printf("\n");
	strcpy(thisGame.name, name2);
}

// The setup of the game for all enemies and main character
void setup() {
	thisGame.hp = 20;
	thisGame.level = 1;
	thisGame.xp = 2000;
	thisGame.ehp[0] = 115;
	thisGame.ehp[1] = 20;
	thisGame.ehp[2] = 20;
	thisGame.ehp[3] = 20;
	thisGame.ehp[4] = 20;
	thisGame.ehp[5] = 20;
	thisGame.ehp[6] = 20;
	thisGame.ehp[7] = 20;
	thisGame.ehp[8] = 20;
	thisGame.ehp[9] = 10;
	int i = 0;
	for (i = 1; i < 9; i++) {
		thisGame.earmor[i] = rand() % 5;
		thisGame.eweapon[i] = rand() % 5;
	}
	thisGame.earmor[0] = 4;
	thisGame.earmor[9] = 0;
	thisGame.eweapon[0] = 4;
	thisGame.eweapon[9] = 0;

	thisGame.elevel[0] = 20;
	thisGame.elevel[1] = 1;
	thisGame.elevel[2] = 1;
	thisGame.elevel[3] = 1;
	thisGame.elevel[4] = 1;
	thisGame.elevel[5] = 1;
	thisGame.elevel[6] = 1;
	thisGame.elevel[7] = 1;
	thisGame.elevel[8] = 1;
	thisGame.elevel[9] = 1;
	thisGame.exp[0] = 1048921;
	thisGame.exp[1] = 2000;
	thisGame.exp[2] = 2000;
	thisGame.exp[3] = 2000;
	thisGame.exp[4] = 2000;
	thisGame.exp[5] = 2000;
	thisGame.exp[6] = 2000;
	thisGame.exp[7] = 2000;
	thisGame.exp[8] = 2000;
	thisGame.exp[9] = 2000;
}

// Basic struct saving
void save() {
	FILE *myFile = fopen("rpg.save", "wb");
	fwrite(&thisGame, sizeof(struct gameState), 1, myFile);
	fclose(myFile);
}

// Show the character's info
void displayChar() {
	printf("\[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", thisGame.name, thisGame.hp, armors[thisGame.armor], weapons[thisGame.weapon], thisGame.level, thisGame.xp);
}

// The fight loop
void fight(int choice) {
	char c = 'a';
	do { scanf("%c", &c); } while (c != '\n');
	while (1) {
		int oneRoll = 0;
		int twoRoll = 0;
		int youHitHim = 0;
		int eHitHim = 0;
		int youDmgHim = 0;
		int eDmgHim = 0;
		oneRoll = rand() % 20 + 1;
		twoRoll = rand() % 20 + 1;
		if (oneRoll >= armorStat[thisGame.earmor[choice]]) {
			youHitHim = 1;
		}
		if (twoRoll >= armorStat[thisGame.armor]) {
			eHitHim = 1;
		}
		if (youHitHim) {
			youDmgHim = getDamage(thisGame.weapon);
			printf("%s hits %s for %d damage (attack roll %d)\n", thisGame.name, enemies[choice], youDmgHim, oneRoll);
			thisGame.ehp[choice] -= youDmgHim;
		}
		else {
			printf("%s misses %s (attack roll %d)\n", thisGame.name, enemies[choice], oneRoll);
		}
		if (eHitHim) {
			eDmgHim = getDamage(thisGame.eweapon[choice]);
			printf("%s hits %s for %d damage (attack roll %d)\n", enemies[choice], thisGame.name, eDmgHim, twoRoll);
			thisGame.hp -= eDmgHim;
		}
		else {
			printf("%s misses %s (attack roll %d)\n", enemies[choice], thisGame.name, twoRoll);
		}
		if (thisGame.hp <= 0 && thisGame.ehp[choice] <= 0) {
			printf("\n");
			printf("Both of you have died!\n");
			printf("Respawning %s ...\n", thisGame.name);
			thisGame.hp = 20 + (thisGame.level - 1) * 5;
			thisGame.xp = mypow(2, thisGame.level) * 1000;
			printf("\[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", thisGame.name, thisGame.hp, armors[thisGame.armor], weapons[thisGame.weapon], thisGame.level, thisGame.xp);
			printf("Respawning %s ...\n", enemies[choice]);
			int ep = 0;
			int elev = 0;
			int exp2 = 0;
			int earm = 0;
			int eweap = 0;
			if (choice == 0) {
				ep = 115;
				elev = 20;
				exp2 = 1048921;
				earm = 4;
				eweap = 4;
			}
			else if (choice == 9) {
				ep = 10;
				elev = 1;
				exp2 = 2000;
				earm = 0;
				eweap = 0;
			}
			else {
				earm = rand() % 5;
				eweap = rand() % 5;
				elev = rand() % thisGame.level + 1;
				exp2 = mypow(2, elev) * 1000;
				ep = 20 + (elev - 1) * 5;
			}
			thisGame.ehp[choice] = ep;
			thisGame.elevel[choice] = elev;
			thisGame.exp[choice] = exp2;
			printf("\[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", enemies[choice], ep, armors[earm], weapons[earm], elev, exp2);
			break;
		}
		else if (thisGame.hp <= 0) {
			printf("\n");
			printf("%s is killed by %s.\n", thisGame.name, enemies[choice]);
			printf("Respawning %s ...\n", thisGame.name);
			thisGame.hp = 20 + (thisGame.level - 1) * 5;
			thisGame.xp = mypow(2, thisGame.level) * 1000;
			printf("\[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", thisGame.name, thisGame.hp, armors[thisGame.armor], weapons[thisGame.weapon], thisGame.level, thisGame.xp);
			break;
		}
		else if (thisGame.ehp[choice] <= 0) {
			printf("\n");
			printf("%s is killed by %s.\n", enemies[choice], thisGame.name);
			printf("Get %s's %s, exchanging %s's current armor %s (y/n)? ", enemies[choice], armors[thisGame.earmor[choice]], thisGame.name, armors[thisGame.armor]);
			char d = 'a';
			scanf("%c", &d);
			do { scanf("%c", &c); } while (c != '\n');
			if (d == 'y') {
				thisGame.armor = thisGame.earmor[choice];
			}
			printf("Get %s's %s, exchanging %s's current weapon %s (y/n)? ", enemies[choice], weapons[thisGame.eweapon[choice]], thisGame.name, weapons[thisGame.weapon]);
			scanf("%c", &d);
			do { scanf("%c", &c); } while (c != '\n');
			if (d == 'y') {
				thisGame.weapon = thisGame.eweapon[choice];
			}
			thisGame.xp += 2000 * thisGame.elevel[choice];

			if (thisGame.xp >= mypow(2, thisGame.level + 1) * 1000) {
				thisGame.level++;
				printf("%s levels up to level %d!\n", thisGame.name, thisGame.level);
			}
			thisGame.hp = 20 + (thisGame.level - 1) * 5;
			printf("\[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", thisGame.name, thisGame.hp, armors[thisGame.armor], weapons[thisGame.weapon], thisGame.level, thisGame.xp);
			printf("Respawning %s ...\n", enemies[choice]);
			int ep = 0;
			int elev = 0;
			int exp2 = 0;
			int earm = 0;
			int eweap = 0;
			if (choice == 0) {
				ep = 115;
				elev = 20;
				exp2 = 1048921;
				earm = 4;
				eweap = 4;
			}
			else if (choice == 9) {
				ep = 10;
				elev = 1;
				exp2 = 2000;
				earm = 0;
				eweap = 0;
			}
			else {
				earm = rand() % 5;
				eweap = rand() % 5;
				elev = rand() % thisGame.level + 1;
				exp2 = mypow(2, elev) * 1000;
				ep = 20 + (elev - 1) * 5;
			}
			thisGame.ehp[choice] = ep;
			thisGame.elevel[choice] = elev;
			thisGame.exp[choice] = exp2;
			printf("\[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", enemies[choice], ep, armors[earm], weapons[earm], elev, exp2);
			break;
		}
	}
}

// Get exponents
int mypow(int a, int b) {
	int result = 1;
	while (1) {
		if (b & 1) {
			result *= a;
		}
		b >>= 1;
		if (!b) {
			break;
		}
		a *= a;
	}
	return result;
}

// Handle sigterm
void sigterm2(int a) {
	save(thisGame);
	exit(1);
}

// Handles sigint
void sigint2(int a) {
	save(thisGame);
	exit(1);
}

// Handle sigrtmin
void sigrtmin2(int a) {
	printf("\nEARTH QUAKE!!!\n\n");
	int i = 0;
	for (i = 0; i < 10; i++) {
		printf("%s suffers -20 damage ", enemies[i]);
		if (thisGame.ehp[i] <= 20) {
			printf("and dies. Respawning ...\n");
			respawn(i);
		}
		else {
			printf("but survives.\n");
			thisGame.ehp[i] -= 20;
		}
	}
	printf("%s suffers -20 damage ", thisGame.name);
	if (thisGame.hp <= 20) {
		printf("and dies. Respawning ...\n");
		respawn(10);
		printf("command >> ");
		fflush(stdout);
	}
	else {
		printf("but survives.\ncommand >> ");
		fflush(stdout);
		thisGame.hp -= 20;
	}
}

// Bring a character back to life
void respawn(int i) {
	if (i != 10) {
		int ep = 0;
		int elev = 0;
		int exp2 = 0;
		int earm = 0;
		int eweap = 0;
		if (i == 0) {
			ep = 115;
			elev = 20;
			exp2 = 1048921;
			earm = 4;
			eweap = 4;
		}
		else if (i == 9) {
			ep = 10;
			elev = 1;
			exp2 = 2000;
			earm = 0;
			eweap = 0;
		}
		else {
			earm = rand() % 5;
			eweap = rand() % 5;
			elev = rand() % thisGame.level + 1;
			exp2 = mypow(2, elev) * 1000;
			ep = 20 + (elev - 1) * 5;
		}
		thisGame.ehp[i] = ep;
		thisGame.elevel[i] = elev;
		thisGame.exp[i] = exp2;
		printf("\[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", enemies[i], ep, armors[earm], weapons[earm], elev, exp2);
	}
	else {
		thisGame.hp = 20 + (thisGame.level - 1) * 5;
		thisGame.xp = mypow(2, thisGame.level) * 1000;
		printf("\[%s: hp=%d, armor=%s, weapon=%s, level=%d, xp=%d]\n", thisGame.name, thisGame.hp, armors[thisGame.armor], weapons[thisGame.weapon], thisGame.level, thisGame.xp);
	}
}
