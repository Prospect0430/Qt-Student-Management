#pragma once
#include <string>
#include <map>
using namespace std;
class Student
{
public:
		Student();
		void setName(string name);
		string getName();
		void setId(string id);
		string getId();
		void setGender(string gender);
		string getGender();
		void addScore(string courseName, float score);
		float getScore(string courseName);
		map<string, float> getAllScores();
		void setMajor(const string& major);
		void setaddress(const string& address);
		void setBorndate(const string& borndate);
		string getMajor();
		string getaddress();
		string getBorndate();
		~Student() {}

private:
	string name;
	string id;
	string gender;
	map<string, float> score;
	string major;
	string address;
	string borndate;
};

