#include"Student.h"
Student::Student() {
	id = "0";
}


void Student::setName(string name) {
	this->name = name;
}
string Student::getName() {
	return name;
}


void Student::setId(string id) {
	this->id = id;
}
string Student::getId() {
	return id;
}


void Student::setGender(string gender) {
	this->gender = gender;
}
string Student::getGender() {
	return gender;
}


void Student::addScore(string courseName, float score) {
	this->score[courseName] = score;
}
float Student::getScore(string courseName) {
	auto it = score.find(courseName);
	if (it != score.end())
	{
		return it->second;
	}
	else
	{
		return -1.0;
	}
}


map<string, float> Student::getAllScores() {
	return score;
}


void Student::setMajor(const string& major)
{
	this->major= major;
}
string Student::getMajor()
{
	return major;
}


void Student::setaddress(const string& address)
{
	this->address = address;
}
string Student::getaddress()
{
	return address;
}


void Student::setBorndate(const string& borndate)
{
	this->borndate = borndate;
}
string Student::getBorndate()
{
	return borndate;
}
