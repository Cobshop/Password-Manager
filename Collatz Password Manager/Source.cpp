#include <iostream>
#include <sstream>
#include <chrono>
#include <string>
#include <vector>
#include <stdint.h>
#include <random>
#include <fstream>
#include <algorithm>

using namespace std;
using std::vector;

vector<char> gletters = {};
vector<char> gcharacters = {};

auto Create_Character_Set = [](string s) {
	vector<char> characters(s.begin(), s.end());
	return characters;
};

class passwordManager {
public:
	passwordManager();
	virtual ~passwordManager();
	int Collatz(int x, int n);
	virtual void InputMenu();
	virtual void AccessFile() = 0;
	virtual inline void GenerateEpassword();

protected:
	string Username, Password;
	string ePassword;
	const string file_name;
};

passwordManager::passwordManager() : file_name("password.txt"){

}

passwordManager::~passwordManager() {

}

void passwordManager::InputMenu() {

}

int passwordManager::Collatz(int x, int n) {
	if (x != 1) {
		if (x % 2 == 0)
			n = Collatz(x / 2, n);
		else
			n = Collatz(3 * x + 1, n);

		n++;
	}
	return n;
}

inline void passwordManager::GenerateEpassword() {
	int offset = 0;
	for (auto& i : Password) {
		int stoppingTime = Collatz(unsigned char(i) + offset, 0);
		ePassword.append(to_string(stoppingTime));
		offset = stoppingTime;
	}
}

class create : public passwordManager {
public:
	create();
	~create();
	void InputMenu();
	void AccessFile();
};

create::create() {

}

create::~create() {

}

void create::InputMenu() {
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << "Input a Username\n";
	getline(cin, Username);
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << "Input a Password\n";
	getline(cin, Password);

	GenerateEpassword();

	cout << "Username and Password saved\n";
}

void create::AccessFile() {
	ofstream ost{ file_name, ios::app};
	ost << Username << " " << ePassword << "\n";
}

class checkDetails : public passwordManager {
public:
	checkDetails();
	~checkDetails();
	void InputMenu();
	void AccessFile();
protected:
	vector<string>* Usernames;
	vector<string>* Passwords;
};

checkDetails::checkDetails() : Usernames(new vector<string>), Passwords(new vector<string>) {

}

checkDetails::~checkDetails() {
	delete Usernames;
	delete Passwords;
}

void checkDetails::InputMenu() {
	int tries = 3;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << "Input a Username\n";
	getline(cin, Username);
	
	for (size_t i = 0; i < Usernames->size(); ++i) {
		if ((*Usernames)[i] == Username) {
			while (tries > 0) {
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << "Input a Password\n";
				getline(cin, Password);
				GenerateEpassword();
				if (ePassword == (*Passwords)[i]) {
					cout << "Success!\n";
					return;
				}
				else {
					cout << "Password Incorrect\n";
					tries--;
					cout << "You have " << tries << " attempts left\n";
					ePassword.clear();
				}
			}
			cout << "You have run out of attempts\n";
			return;
		}
	}
	cout << "Invalid Username\n";
}

void checkDetails::AccessFile() {
	ifstream data_file;
	string temp;

	data_file.open(file_name.c_str());

	if (data_file.fail())
		throw invalid_argument("no file exists " + file_name);

	for (temp; getline(data_file, temp);) {
		Usernames->push_back(temp.substr(0, temp.find_first_of(" ")));
		Passwords->push_back(temp.substr(temp.find_first_of(" ") + 1));
	}

	data_file.close();
}

class passwordGeneration : public passwordManager {
public:
	passwordGeneration();
	~passwordGeneration();
	vector<char> GenerateCharacters();
	vector<char> GenerateCharacters2();
	void GeneratePasswords(vector<char> arr, bool repeat);
	void AccessFile();
};

passwordGeneration::passwordGeneration() {

}

passwordGeneration::~passwordGeneration() {

}

vector<char> passwordGeneration::GenerateCharacters() {
	vector<char> letters;
	vector<char> alphabet = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
						'i', 'j', 'k','l', 'm', 'n', 'o', 'p',
						'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
	for (int i = 0; i < 10; i++) {
		letters.push_back(alphabet[rand()%(alphabet.size())]);
		alphabet.erase(remove(alphabet.begin(), alphabet.end(), letters[i]));
		gletters.push_back(letters[i]);
	}
	return letters;
}

vector<char> passwordGeneration::GenerateCharacters2() {
	string s("!#$ % &'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~⌂ÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜø£Ø×ƒáíóúñÑªº¿®¬½¼¡«»");
	gcharacters = Create_Character_Set(s);
	return Create_Character_Set(s);
}

void passwordGeneration::GeneratePasswords(vector<char> arr, bool repeat) {
	int n = 0;
	while (n <= 10000) {
		if(!repeat)
			random_shuffle(arr.begin(), arr.end());

		for (int i = 0; i < (int(n / 100) + 1); i++) {
			if (repeat)
				Password.push_back(arr[rand() % 10]);
			else
				Password.push_back(arr[i]);
		}

		GenerateEpassword();
		AccessFile();
		ePassword.clear();
		Password.clear();
		n++;
	}
}

void passwordGeneration::AccessFile() {
	ofstream ost{ "passwordtest.txt", ios::app };
	ost << ePassword << "\n";
}

class passwordAnalysis : public passwordManager {
public: 
	passwordAnalysis();
	~passwordAnalysis();
	void Crack();
	void Crack2(vector<vector<int>> collatzNumbers, bool decrypt);
	void Decrypt();
	void passwordRunThrough(int startPoint);
	void AccessFile();
	vector<vector<int>> fillArray(vector<vector<int>> collatzNumbers);
protected:
	bool stop;
	int percentage;
	vector<double> Times;
	vector<unsigned int> eLetters;
	vector<char> Letters;
	vector<string>* ePasswords;
};

passwordAnalysis::passwordAnalysis() : ePasswords(new vector<string>), Letters(gletters), percentage(0), stop(false) {

}

passwordAnalysis::~passwordAnalysis() {
	delete ePasswords;
}

vector<vector<int>> passwordAnalysis::fillArray(vector<vector<int>> collatzNumbers) {
	collatzNumbers[6] = { 10, 64 };
	collatzNumbers[7] = { 3, 20, 21, 128 };
	collatzNumbers[8] = { 6, 40, 42, 256 };
	collatzNumbers[9] = { 12, 13, 80, 84, 85 };
	collatzNumbers[10] = { 24, 26, 160, 168, 170 };
	collatzNumbers[11] = { 48, 52, 53, 320, 336, 340, 341 };
	collatzNumbers[12] = { 17, 96, 104, 106, 113 };
	collatzNumbers[13] = { 34, 35, 192, 208, 212, 213, 226, 227 };
	collatzNumbers[14] = { 11, 68, 69, 70, 75, 384 };
	collatzNumbers[15] = { 22, 23, 136, 138, 140, 141, 150, 151 };
	collatzNumbers[16] = { 7, 44, 45, 46, 272, 276, 277, 280, 282, 300, 301, 302 };
	collatzNumbers[17] = { 14, 15, 88, 90, 92, 93 };
	collatzNumbers[18] = { 28, 29, 30, 176, 180, 181, 184, 186, 201 };
	collatzNumbers[19] = { 9, 56, 58, 60, 61, 352, 360, 362, 368, 369, 372, 373 };
	collatzNumbers[20] = { 18, 19, 112, 116, 117, 120, 122 };
	collatzNumbers[21] = { 36, 37, 38, 224, 232, 234, 240, 241, 244, 245, 267 };
	collatzNumbers[22] = { 72, 74, 76, 77, 81 };
	collatzNumbers[23] = { 25, 144, 148, 149, 152, 154, 162, 163 };
	collatzNumbers[24] = { 49, 50, 51, 288, 296, 298, 304, 308, 309, 321, 324, 325, 326, 331 };
	collatzNumbers[25] = { 98, 99, 100, 101, 102 };
	collatzNumbers[26] = { 33, 196, 197, 198, 200, 202, 204, 205, 217 };
	collatzNumbers[27] = { 65, 66, 67, 392, 394, 396, 397 };
	collatzNumbers[28] = { 130, 131, 132, 133, 134 };
	collatzNumbers[29] = { 43, 260, 261, 262, 264, 266, 268, 269, 273, 289 };
	collatzNumbers[30] = { 86, 87, 89 };
	collatzNumbers[31] = { 172, 173, 174, 177, 178, 179 };
	collatzNumbers[32] = { 57, 59 };
	collatzNumbers[33] = { 114, 115, 118, 119 };
	collatzNumbers[34] = { 39, 228, 229, 230, 236, 237, 238 };
	collatzNumbers[35] = { 78, 79 };
	collatzNumbers[36] = { 153, 156, 157, 158 };
	collatzNumbers[37] = { 305, 306, 307, 312, 314, 315, 316, 317 };
	collatzNumbers[38] = { 105 };
	collatzNumbers[39] = { 203, 209, 210, 211 };
	collatzNumbers[41] = { 135, 139 };
	collatzNumbers[42] = { 270, 271, 278, 279, 281, 287, 303 };
	collatzNumbers[44] = { 185, 187, 191 };
	collatzNumbers[45] = { 361, 363, 367, 370, 371, 374, 375, 382, 383 };
	collatzNumbers[46] = { 123, 127 };
	collatzNumbers[47] = { 246, 247, 249, 254, 255 };
	collatzNumbers[49] = { 169 };
	collatzNumbers[50] = { 329, 338, 339, 359 };
	collatzNumbers[52] = { 219, 225, 239 };
	collatzNumbers[54] = { 159 };
	collatzNumbers[55] = { 295, 318, 319 };
	collatzNumbers[58] = { 379, 393 };
	collatzNumbers[60] = { 283 };
	collatzNumbers[63] = { 377 };
	collatzNumbers[65] = { 251 };
	collatzNumbers[67] = { 167 };
	collatzNumbers[68] = { 334, 335 };
	collatzNumbers[69] = { 111 };
	collatzNumbers[70] = { 222, 223 };
	collatzNumbers[73] = { 297 };
	collatzNumbers[76] = { 395 };
	collatzNumbers[78] = { 263 };
	collatzNumbers[80] = { 175 };
	collatzNumbers[81] = { 350, 351 };
	collatzNumbers[83] = { 233 };
	collatzNumbers[85] = { 155 };
	collatzNumbers[86] = { 310, 311 };
	collatzNumbers[87] = { 103 };
	collatzNumbers[88] = { 206, 207 };
	collatzNumbers[90] = { 137 };
	collatzNumbers[91] = { 274, 275 };
	collatzNumbers[92] = { 91 };
	collatzNumbers[93] = { 182, 183 };
	collatzNumbers[94] = { 364, 365, 366 };
	collatzNumbers[95] = { 121 };
	collatzNumbers[96] = { 242, 243 };
	collatzNumbers[98] = { 161 };
	collatzNumbers[99] = { 322, 323 };
	collatzNumbers[100] = { 107 };
	collatzNumbers[101] = { 214, 215 };
	collatzNumbers[102] = { 71 };
	collatzNumbers[103] = { 142, 143 };
	collatzNumbers[104] = { 47 };
	collatzNumbers[105] = { 94, 95 };
	collatzNumbers[106] = { 31 };
	collatzNumbers[107] = { 62, 63, 376, 378, 380, 381 };
	collatzNumbers[108] = { 124, 125, 126 };
	collatzNumbers[109] = { 41 };
	collatzNumbers[110] = { 82, 83 };
	collatzNumbers[111] = { 27 };
	collatzNumbers[112] = { 54, 55 };
	collatzNumbers[113] = { 108, 109, 110 };
	collatzNumbers[114] = { 216, 218, 220, 221 };
	collatzNumbers[115] = { 73 };
	collatzNumbers[116] = { 145, 146, 147 };
	collatzNumbers[117] = { 290, 291, 292, 293, 294, 299 };
	collatzNumbers[118] = { 97 };
	collatzNumbers[119] = { 193, 194, 195, 199 };
	collatzNumbers[120] = { 386, 387, 388, 389, 390, 391, 398, 399 };
	collatzNumbers[121] = { 129 };
	collatzNumbers[122] = { 257, 258, 259, 265 };
	collatzNumbers[124] = { 171 };
	collatzNumbers[125] = { 342, 343, 345, 347, 353 };
	collatzNumbers[127] = { 231, 235 };
	collatzNumbers[130] = { 313 };
	collatzNumbers[143] = { 327 };
	return collatzNumbers;

}

void passwordAnalysis::Crack() {
	auto tBegin = chrono::high_resolution_clock::now();
	ifstream data_file;
	string temp;

	data_file.open("collatz.txt");

	if (data_file.fail())
		throw invalid_argument("no file exists " + file_name);

	string possNumbers;
	unsigned int n = 0;
	int j = 1;
	bool back = false;
	bool exit = false;
	unsigned int offset = 0;
	unsigned int eLetter = 0;
	char Letter = 0;
	vector<int> allJ;
	vector<int> vPossNumbers;
	while (n < ePassword.length()) {
		j = 1;
		while (j <= 3) {
			for (temp; getline(data_file, temp);) {
				if (ePassword.substr(n, j) == temp.substr(0, temp.find_first_of(" "))) {
					possNumbers = temp.substr(temp.find_first_of(" ") + 1);
					stringstream ssPossNumbers(possNumbers);
					while (ssPossNumbers >> eLetter)
						vPossNumbers.push_back(eLetter);
					for (unsigned int i = 0; i < vPossNumbers.size(); i++) {
						if (vPossNumbers[i] - offset < 255) {
							for (unsigned int x = 0; x < Letters.size(); x++) {
								if (char(vPossNumbers[i] - offset) == Letters[x]) {
									Letter = char(vPossNumbers[i] - offset);
									offset = stoi(temp.substr(0, temp.find_first_of(" ")));
									break;
								}
							}
							if (Letter != 0) {
								break;
							}
						}
					}

					vPossNumbers.clear();
					
					if (Letter != 0) {
						Password.push_back(Letter);
						Letter = 0;
					}
					else
						break;


					back = true;
					break;
				}
			}
			data_file.clear();
			data_file.seekg(0);
			if (back) {
				break;
			}
			j++;
		}
		back = false;
		if ((j >= 4) && (exit == false)) {
			if (Password.length() > 2) {
				Password.pop_back();
				j = 3;
				n = n - 2;
				offset = stoi(ePassword.substr(n, allJ[Password.length() - 1]));
			}
			exit = true;
		}
		else {
			allJ.push_back(j);
			n = n + j;
			j = 1;
		}
	}
	data_file.close();
	auto tEnd = chrono::high_resolution_clock::now();
	Times.push_back(chrono::duration<double, milli>(tEnd - tBegin).count());
	offset = 0;
}

void passwordAnalysis::Crack2(vector<vector<int>> collatzNumbers, bool decrypt) {
	auto tBegin = chrono::high_resolution_clock::now();
	unsigned int n = 0;
	int j = 1;
	bool back = false;
	bool exit = false;
	vector<int> allJ;
	unsigned int offset = 0;
	unsigned int eLetter = 0;
	unsigned int eNumber = 0;
	char Letter = 0;
	vector<int> vPossNumbers;
	while (n < ePassword.length()) {
		while (j <= 3) {
			eNumber = stoi(ePassword.substr(n, j));
			if ((eNumber) < 144) {
				vPossNumbers = collatzNumbers[eNumber];
				for (unsigned int i = 0; i < vPossNumbers.size(); i++) {
					if (vPossNumbers[i] - offset < 256) {
						for (unsigned int x = 0; x < Letters.size(); x++) {
							if (vPossNumbers[i] - offset == int(Letters[x])) {
								Letter = Letters[x];
								if(decrypt)
    									cout << Letter;
							}
						}
					}
				}
				
				
				vPossNumbers.clear();

				if (Letter != 0) {
					offset = eNumber;
					Password.push_back(Letter);
					Letter = 0;
					break;
				}	
			}
			j++;
		}
		if (decrypt)
			cout << "\n";
		if ((j >= 4) && (exit == false)) {
			if (Password.length() > 2) {
				Password.pop_back();
				j = 3;
				n = n - 2;
				offset = stoi(ePassword.substr(n, allJ[Password.length()-1]));
			}
			exit = true;
		}
		else {
			allJ.push_back(j);
			n = n + j;
			j = 1;
		}
	}
	auto tEnd = chrono::high_resolution_clock::now();
	Times.push_back(chrono::duration<double, milli>(tEnd - tBegin).count());
	offset = 0;
}

void passwordAnalysis::passwordRunThrough(int startPoint) {
	vector<vector<int>> collatzNumbers(144);
	collatzNumbers = fillArray(collatzNumbers);
	bool repeat = true;
	if (startPoint >= 10000) {
		Letters = gcharacters;
		repeat = false;
	}
	for (unsigned int i = 0; i < 10000; i++) {
		ePassword = (*ePasswords)[startPoint + i];

		//Crack();
		Crack2(collatzNumbers, false);
		if (Password.length() >= (int(i / 100) + 1))
			percentage++;
		if (Times.size() >= 100) {
			double sum = 0;
			for (int j = 0; j < 100; j++)
				sum = sum + Times[j];
			cout << "Average Time: " << sum / 100 << "ms\n";
			cout << "Percentage of Passwords cracked: " << percentage << "%\n";
			percentage = 0;
			Times.clear();
		}
		//cout << Password << "\n";
		Password.clear();
	}
}

void passwordAnalysis::AccessFile() {
	ifstream data_file;
	string temp;

	data_file.open("passwordtest.txt");

	if (data_file.fail())
		throw invalid_argument("no file exists " + file_name);

	for (temp; getline(data_file, temp);) {
		ePasswords->push_back(temp);
	}

	data_file.close();
}

void::passwordAnalysis::Decrypt() {
	ePassword = "27322810313331033910211452912207344136146925461033281533271031012815108114101";
	string s = "ABCDEFGHIJKLMNOPQRSTUWXZabcdefhiklmnopqrstuvwxz ";
	vector<char> characters(s.begin(), s.end());
	Letters = characters;
	vector<vector<int>> collatzNumbers(144);
	collatzNumbers = fillArray(collatzNumbers);
	Crack2(collatzNumbers, true);
	cout << "Sentence decrypted is: " << Password << "\n";
	Password.clear();
}

void Create_Username_Password() {
	create* Account = new create();
	Account->InputMenu();
	Account->AccessFile();
	delete Account;
}

void Check_Username_Password() {
	checkDetails* SignIn = new checkDetails();
	try {
		SignIn->AccessFile();
	}
	catch (const invalid_argument& iae) {
		cout << "unable to read data: " << iae.what() << "\n";
		exit(1);
	}
	SignIn->InputMenu();
	delete SignIn;
}

void Generate_Password_Analysis() {
	passwordGeneration* Generate = new passwordGeneration();
	Generate->GeneratePasswords(Generate->GenerateCharacters(), true);
	Generate->GeneratePasswords(Generate->GenerateCharacters2(), false);
	delete Generate;
}

void Analyse_Generated_Passwords() {
	passwordAnalysis* Analysis = new passwordAnalysis();
	try {
		Analysis->AccessFile();
	}
	catch (const invalid_argument& iae) {
		cout << "unable to read data: " << iae.what() << "\n";
		exit(1);
	}
	Analysis->passwordRunThrough(0);
	delete Analysis;
}

void Decrypt_Sentence() {
	passwordAnalysis* Analysis = new passwordAnalysis();
	Analysis->Decrypt();
	delete Analysis;
}

int main() {
	srand(time(NULL));
	int x = -1;
	bool menu = true;
	while (menu == true) {
		cout << "Input number corresponding to the option you wish to choose\n";
		cout << "1: Create Username/Password\n";
		cout << "2: Check Username and Password\n";
		cout << "3: Generate Password Strength Analysis File\n";
		cout << "4: Analyse Password Strength Analysis File\n";
		cout << "5: Decrypt given sentence\n";
		cout << "6: Quit Program\n";
		cin >> x;
		if (x == 1) {
			void (*CreatePtr)();
			CreatePtr = &Create_Username_Password;
			CreatePtr();
		}
		else if (x == 2) {
			Check_Username_Password();
		}
		else if (x == 3) {
			Generate_Password_Analysis();
		}
		else if (x == 4) {
			Analyse_Generated_Passwords();
		}
		else if (x == 5) {
			Decrypt_Sentence();
		}
		else if (x == 6) {
			menu = false;
		}
		else {
			cout << "Invalid input\n";
			menu = false;
		}	
	}
	return 0;
}