#include <ctime>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <cstring>

#include <vector>
#include <algorithm>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;

bool cmdOptionExists(char** begin, char** end, const string& option);
char* getCmdOption(char ** begin, char ** end, const string & option);
bool compareFileNamesNumeric(const string& s1, const string& s2);
string ExtractDirectory(const string path);
string ExtractFileName(const string path);
string getExtension(const string path);
bool getFilenameWithoutExtension(const string path);
string getPrefix(const string path);
string getSuffix(const string path);
int getDirbin(string dir, vector<string> &files,  vector<string> &darkfiles, string &logfile);
int getDirtiff(string dir, vector<string> &files, string filenameprefix);

