//****************Function Declarations*******************
void intToChar(int intToChng, char* charToRet);
char* str2Char(string str);
string numToStr(int num);
char* numToChar(int num);
string concat(string s1, string s2);
void tab(int count);
bool startsWith(const char *a, const char *b);
char* substring(const char *string, int position, int length);
int countOccurence(char* string, char splitter);
void split(char* string, char splitter, char splittedArr[][1024]);
void split(char* string, char splitter, char splittedArr[][16384]);
int indexOf(char* string, char of);

//****************Function Definitions*******************
void intToChar(int intToChng, char* charToRet) {
	size_t size = sizeof(charToRet);
	snprintf(charToRet, size, "%d", intToChng);
}

char* str2Char(string str) {
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	return cstr;
}

string numToStr(int num) {
	stringstream ss;
	ss << num;
	return ss.str();
}

char* numToChar(int num) {
	string str = numToStr(num);
	return str2Char(str);
}

string concat(string s1, string s2) {
	stringstream ss;
	ss << s1 << s2;
	return ss.str();
}
void tab(int count) {
	for (int i = 0; i < count; i++) {
		cout << "\t";
	}
}

bool startsWith(const char *a, const char *b) {
	if (strncmp(a, b, strlen(b)) == 0) {
		return 0;
	}
	return 1;
}

char* substring(const char *string, int position, int length) {
	char *pointer;
	int c;

	pointer = (char*) malloc(length + 1);

	if (pointer == NULL) {
		printf("Inside substring : Unable to allocate memory.\n");
		exit( EXIT_FAILURE);
	}

	for (c = 0; c < position - 1; c++) {
		string++;
	}

	for (c = 0; c < length; c++) {
		*(pointer + c) = *string;
		string++;
	}

	*(pointer + c) = '\0';
	return pointer;
}

int countOccurence(char* string, char splitter) {
	int len = strlen(string);
	int count = 0;
	for (int i = 0; i < len; i++) {
		if (string[i] == splitter) {
			count++;
		}
	}
	return count;
}

void split(char* string, char splitter, char splittedArr[][1024]) {
	int len = strlen(string);
	char tmp[len];
	memset(tmp, 0, strlen(tmp));
	int j = 0; //pointer for tmp string
	int k = 0; //pointer for strings in splittedArr
	for (int i = 0; i < len; i++) {
		if (string[i] != splitter) {
			tmp[j] = string[i];
			j++;
		} else {
			tmp[j] = '\0';
			strcpy(splittedArr[k], tmp);
			j = 0;
			memset(tmp, 0, strlen(tmp));
			k++;
		}
	}
	if (string[len - 1] != splitter) { //Adding this check to get the last array
		tmp[j] = '\0';
		strcpy(splittedArr[k], tmp);
	}
}

void split(char* string, char splitter, char splittedArr[][16384]) {
	int len = strlen(string);
	char tmp[len];
	memset(tmp, 0, strlen(tmp));
	int j = 0; //pointer for tmp string
	int k = 0; //pointer for strings in splittedArr
	for (int i = 0; i < len; i++) {
		if (string[i] != splitter) {
			tmp[j] = string[i];
			j++;
		} else {
			tmp[j] = '\0';
			strcpy(splittedArr[k], tmp);
			j = 0;
			memset(tmp, 0, strlen(tmp));
			k++;
		}
	}

	if (string[len - 1] != splitter) { //Adding this check to get the last array
		tmp[j] = '\0';
		strcpy(splittedArr[k], tmp);
	}
}

int indexOf(char* string, char of) {
	int len = strlen(string);
	for (int i = 0; i < len; i++) {
		if (string[i] == of) {
			return i;
		}
	}
	return len - 1;
}

string remLastCharFromStr(string str) {
	char* strChar = str2Char(str);
	strChar[strlen(strChar) - 1] = '\0';

	return strChar;
}
