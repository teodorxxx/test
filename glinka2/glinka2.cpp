#include "glinka2.h"
/*
 * glinka.cpp
 *
 *  Created on: 28-04-2016
 *      Author: teodor
 */

#include "glinka2.h"
#include "mainwindow/mainwindow.h"

Test::Test()
{
	m_TechnicalData.m_RatedVoltage = 0.0;
	m_TechnicalData.m_TestVoltage = 0.0;
	m_TechnicalData.m_MaxVoltage = 0.0;
	m_TechnicalData.m_ResistanceAfter60s = 0.0;
	m_TechnicalData.m_ResistanceAfter15s = 0.0;
	m_TechnicalData.m_TimeShortCircuit = 0.0;
	m_TechnicalData.m_TimeReconstruction = 0.0;
}
Test::Test(const Test &ts)
{
	m_TechnicalData.m_RatedVoltage = ts.m_TechnicalData.m_RatedVoltage;
	m_TechnicalData.m_TestVoltage = ts.m_TechnicalData.m_TestVoltage;
	m_TechnicalData.m_MaxVoltage = ts.m_TechnicalData.m_MaxVoltage;
	m_TechnicalData.m_ResistanceAfter60s = ts.m_TechnicalData.m_ResistanceAfter60s;
	m_TechnicalData.m_ResistanceAfter15s = ts.m_TechnicalData.m_ResistanceAfter15s;
	m_TechnicalData.m_TimeShortCircuit = ts.m_TechnicalData.m_TimeShortCircuit;
	m_TechnicalData.m_TimeReconstruction = ts.m_TechnicalData.m_TimeReconstruction;
	m_TechnicalData.m_PairMinVoltageTime = ts.m_TechnicalData.m_PairMinVoltageTime;
	m_TechnicalData.m_PairMaxVoltageTime = ts.m_TechnicalData.m_PairMaxVoltageTime;
	m_MMGlinkaVoltageTime = ts.m_MMGlinkaVoltageTime;
}
Test::Test(double rV, double tV, double mV, double r60, double r15, double tSC, double tR, std::multimap<double, double> &gl,
		   std::pair<double, double> &pMaxV, std::pair<double, double> &pMinV)
{
	m_TechnicalData.m_RatedVoltage = rV;
	m_TechnicalData.m_TestVoltage = tV;
	m_TechnicalData.m_MaxVoltage = mV;
	m_TechnicalData.m_ResistanceAfter60s = r60;
	m_TechnicalData.m_ResistanceAfter15s = r15;
	m_TechnicalData.m_TimeShortCircuit = tSC;
	m_TechnicalData.m_TimeReconstruction = tR;
	m_TechnicalData.m_PairMinVoltageTime = pMinV;
	m_TechnicalData.m_PairMaxVoltageTime = pMaxV;
	m_MMGlinkaVoltageTime = gl;
}
void Test::reconstruction(const std::string &name)
{
	using namespace std;

	ifstream inFile;
	inFile.open(name, ios_base::in);

	if (!inFile.is_open())
	{
		cerr << "Cannot open file: " << name << endl;
		exit(EXIT_FAILURE);
	}

	pair<double, double> pairTempGlinkaVoltageTime;
	//pair<double, double> minVoltageAndTime;

	bool min_get = false;
	while (inFile >> pairTempGlinkaVoltageTime.second && inFile >> pairTempGlinkaVoltageTime.first)
	{
		if (pairTempGlinkaVoltageTime.first >= 0.09 && !min_get)
		{
			m_TechnicalData.m_PairMinVoltageTime = pairTempGlinkaVoltageTime;
			min_get = true;
		}
		m_MMGlinkaVoltageTime.insert(pairTempGlinkaVoltageTime);
	}
	inFile.close();

	multimap<double, double>::reverse_iterator maxVoltageAndTime=m_MMGlinkaVoltageTime.rbegin();
	m_TechnicalData.m_PairMaxVoltageTime.first = maxVoltageAndTime->first;
	m_TechnicalData.m_PairMaxVoltageTime.second = maxVoltageAndTime->second;
	m_TechnicalData.m_TimeReconstruction=maxVoltageAndTime->second-m_TechnicalData.m_PairMinVoltageTime.second;
	m_TechnicalData.m_MaxVoltage=maxVoltageAndTime->first;
}
void Test::restoreTestInitialValue()
{
	std::multimap<double, double>tempGlinkaVoltageTime(m_MMGlinkaVoltageTime);
	m_MMGlinkaVoltageTime.clear();

	bool min_get = false;
	for (auto x:tempGlinkaVoltageTime)
	{
		if (x.first >= 0.09 && !min_get)
		{
			m_TechnicalData.m_PairMinVoltageTime = x;
			min_get = true;
		}
		m_MMGlinkaVoltageTime.insert(x);
	}

	std::multimap<double, double>::reverse_iterator maxVoltageAndTime=m_MMGlinkaVoltageTime.rbegin();
	m_TechnicalData.m_PairMaxVoltageTime.first = maxVoltageAndTime->first;
	m_TechnicalData.m_PairMaxVoltageTime.second = maxVoltageAndTime->second;
	m_TechnicalData.m_TimeReconstruction=maxVoltageAndTime->second-m_TechnicalData.m_PairMinVoltageTime.second;
	m_TechnicalData.m_MaxVoltage=maxVoltageAndTime->first;
}

void Test::setTest()
{
	using std::cout;
	using std::cin;
	std::string name;
	cout << "Podaj nazwę pliku do przetworzenia: ";
	getline(cin, name);
	cout << "Napięcie znamonowe [V]: ";
	cin >> m_TechnicalData.m_RatedVoltage;
	cout << "Napięcie probiercze [V]: ";
	cin >> m_TechnicalData.m_TestVoltage;
	cout << "Rezystancja po 60 sekundach [kOhm]: ";
	cin >> m_TechnicalData.m_ResistanceAfter60s;
	cout << "Rezystancja po 15 sekundach [kOhm]: ";
	cin >> m_TechnicalData.m_ResistanceAfter15s;
	cout << "Czas zwarcia [s]: ";
	cin >> m_TechnicalData.m_TimeShortCircuit;
}
void Test::showTest() const
{
	using std::cout;
	using std::endl;
	using std::fixed;
	using std::setprecision;
	using std::left;
	using std::right;
	using std::setw;
	const int wt=50;
	const int wt2=12;

	cout  << fixed << setprecision(2);
	cout <<setw(wt)<<left<< "Napiecie znamonowe: " << setw(wt2)<<right
			<< m_TechnicalData.m_RatedVoltage<< " V" << endl;
	cout <<setw(wt)<<left<< "Napiecie probiercze: " << setw(wt2)<<right
			<< m_TechnicalData.m_TestVoltage<< " V" << endl;
	cout <<setw(wt)<<left<< "Maksymalne napiecie odbudowy: " << setw(wt2)<<right
			<< m_TechnicalData.m_MaxVoltage << " V" << endl;
	cout <<setw(wt)<<left<< "Rezystancja po 60 sekundach: " << setw(wt2)<<right
			<< m_TechnicalData.m_ResistanceAfter60s << " kOhm" << endl;
	cout <<setw(wt)<<left<< "Rezystancja 15 sekundach: " << setw(wt2)<<right
			<< m_TechnicalData.m_ResistanceAfter15s << " kOhm" << endl;
	//cout<<"Czas zwarcia: "<<fixed<<setprecision(2)<<m_TimeShortCircuit<<" s"<<endl;
	//cout<<"Czas odbudowy napięcia: "<<fixed<<setprecision(2)<<m_TimeReconstruction<<" s"<<endl;
}
void Test::resetTest()
{
	m_TechnicalData.m_RatedVoltage = 0.0;
	m_TechnicalData.m_TestVoltage = 0.0;
	m_TechnicalData.m_MaxVoltage = 0.0;
	m_TechnicalData.m_ResistanceAfter60s = 0.0;
	m_TechnicalData.m_ResistanceAfter15s = 0.0;
	m_TechnicalData.m_TimeShortCircuit = 0.0;
	m_TechnicalData.m_TimeReconstruction = 0.0;
	m_TechnicalData.m_PairMinVoltageTime.first = 0.0;
	m_TechnicalData.m_PairMinVoltageTime.second = 0.0;
	m_TechnicalData.m_PairMaxVoltageTime.first = 0.0;
	m_TechnicalData.m_PairMaxVoltageTime.second = 0.0;
	m_MMGlinkaVoltageTime.clear();
}
void Test::writeTest(const std::string &name)const
{
	using namespace std;

	ofstream outFile;
	outFile.open(name, ios_base::out | ios_base::app | ios_base::binary);

	if (!outFile.is_open())
	{
		cerr << "Cannot open file: " << name << endl;
		exit(EXIT_FAILURE);
	}

	int sizeStruct = sizeof m_TechnicalData;
	outFile.write((char *) &sizeStruct, sizeof sizeStruct);
	outFile.write((char *) &m_TechnicalData, sizeStruct);

	int sizeMultimap = m_MMGlinkaVoltageTime.size();
	outFile.write((char *) &sizeMultimap, sizeof sizeMultimap);
	for (auto x:m_MMGlinkaVoltageTime)
		outFile.write((char *) &x, sizeof x);
	outFile.close();
}
std::streampos Test::getTest(const std::string &name, std::streampos place)
{
	using namespace std;

	ifstream inFile;
	inFile.open(name, ios_base::in | ios_base::binary);

	if (!inFile.is_open())
	{
		cerr << "Cannot open file: " << name << endl;
		exit(EXIT_FAILURE);
	}

	inFile.seekg(place);
	int sizeStruct;
	inFile.read((char *) &sizeStruct, sizeof sizeStruct);
	inFile.read((char *) &m_TechnicalData, sizeStruct);

	int sizeMultimap;
	inFile.read((char *) &sizeMultimap, sizeof sizeMultimap);
	pair<double, double>pairTempGlinkaVoltageTime;
	for (int i=0; i<sizeMultimap; ++i)
	{
		inFile.read((char *) &pairTempGlinkaVoltageTime, sizeof pairTempGlinkaVoltageTime);
		m_MMGlinkaVoltageTime.insert(pairTempGlinkaVoltageTime);
	}
	place = inFile.tellg();
	inFile.close();
	return place;
}

std::map<int, int> RatingInsulation::s_CriteriaResistance60sDivTestVoltageSN = {
		{ 50, 5 },
		{ 20, 4 },
		{ 10, 3 },
		{ 3, 0 }
};
std::map<int, int> RatingInsulation::s_CriteriaResistance60sDivTestVoltagenN =
{
		{ 50, 5 },
		{ 20, 4 },
		{ 10, 3 },
		{ 3, 2 },
		{ 2, 1 },
		{ 1, 0 }
};
std::map<int, int> RatingInsulation::s_CriteriaTimeReconstructionSN =
{
		{ 240, 5 },
		{ 120, 4 },
		{ 30, 3 },
		{ 10, 2 },
		{ 0, 1 }
};
std::map<int, int> RatingInsulation::s_CriteriaTimeReconstructionnN =
{
		{ 120, 5 },
		{ 60, 4 },
		{ 15, 3 },
		{ 5, 2 },
		{ 0, 0 }
};
std::map<int, int> RatingInsulation::s_CriteriaTimeShortCircuitSN =
{
		{ 30, 5 },
		{ 1, 2 },
		{0, 0 }
};
std::map<int, int> RatingInsulation::s_CriteriaTimeShortCircuitnN =
{
		{ 10, 5 },
		{ 1, 2 },
		{ 0, 1 }
};
std::map<double, int> RatingInsulation::s_CriteriaMaxVoltageDivTestVoltage =
{
		{ 0.106, 5 },
		{ 0.100, 4 },
		{ 0.050, 3 },
		{ 0.010, 1 },
		{ 0.000, 0 }
};
std::map<double, int> RatingInsulation::s_CriteriaResistance60DivResistance15sSN =
{
		{ 1.5, 5 },
		{ 1.2,4 },
		{ 1.0, 3 },
};
std::map<double, int> RatingInsulation::s_CriteriaResistance60DivResistance15snN =
{
		{ 1.3, 5 },
		{ 1.1,4 },
		{ 1.0, 3 },
};
RatingInsulation::RatingInsulation() :Test()
{
	m_Rating.m_Resistance60sDivTestVoltage = 0.0;
	m_Rating.m_MaxVoltageDivTestVoltage = 0.0;
	m_Rating.m_Resistance60sDivResistance15s = 0.0;
	m_Rating.m_RateResistance60sDivTestVoltage = 0;
	m_Rating.m_RateTimeShortCircuit = 0;
	m_Rating.m_RateTimeReconstruction = 0;
	m_Rating.m_RateMaxVoltageDivTestVoltage = 0;
	m_Rating.m_RateResistance60DivResistance15s = 0;
	m_Rating.m_RateTotal = 0.0;
}
RatingInsulation::RatingInsulation(const Test &ts) :Test(ts)
{
	m_Rating.m_Resistance60sDivTestVoltage = returnsResistanceAfter60s() *1000/ returnsTestVoltage();
	m_Rating.m_MaxVoltageDivTestVoltage = returnsMaxVoltage() / returnsTestVoltage();
	m_Rating.m_Resistance60sDivResistance15s = returnsResistanceAfter60s() / returnsResistanceAfter15s();
	m_Rating.m_RateResistance60sDivTestVoltage = 0;
	m_Rating.m_RateTimeShortCircuit = 0;
	m_Rating.m_RateTimeReconstruction = 0;
	m_Rating.m_RateMaxVoltageDivTestVoltage = 0;
	m_Rating.m_RateResistance60DivResistance15s = 0;
	m_Rating.m_RateTotal = 0.0;
}
RatingInsulation::RatingInsulation(const Test &ts, double r60DivTV, double mVdivTV, double r60Divr15):Test(ts)
{
	m_Rating.m_Resistance60sDivTestVoltage = r60DivTV;
	m_Rating.m_MaxVoltageDivTestVoltage = mVdivTV;
	m_Rating.m_Resistance60sDivResistance15s = r60Divr15;
}
RatingInsulation::RatingInsulation(RatingInsulation &rI):Test(rI.returnsTest())
{
	*this = rI;
}
int RatingInsulation::rateResistance60sDivTestVoltage()
{
	if (Test::returnsRatedVoltage() >= SN) {
		if (m_Rating.m_Resistance60sDivTestVoltage >= 50)
			m_Rating.m_RateResistance60sDivTestVoltage = s_CriteriaResistance60sDivTestVoltageSN[50];
		else if (m_Rating.m_Resistance60sDivTestVoltage < 50	&& m_Rating.m_Resistance60sDivTestVoltage >= 20)
			m_Rating.m_RateResistance60sDivTestVoltage = s_CriteriaResistance60sDivTestVoltageSN[20];
		else if (m_Rating.m_Resistance60sDivTestVoltage < 20	&& m_Rating.m_Resistance60sDivTestVoltage >= 10 && returnsTimeShortCircuit() == 30)
			m_Rating.m_RateResistance60sDivTestVoltage = s_CriteriaResistance60sDivTestVoltageSN[10];
		else if (m_Rating.m_Resistance60sDivTestVoltage < 20	&& m_Rating.m_Resistance60sDivTestVoltage >= 10 && returnsTimeShortCircuit() == 1)
			m_Rating.m_RateResistance60sDivTestVoltage = s_CriteriaResistance60sDivTestVoltageSN[10] - 1;
		else if (m_Rating.m_Resistance60sDivTestVoltage < 3)
			m_Rating.m_RateResistance60sDivTestVoltage = s_CriteriaResistance60sDivTestVoltageSN[1];
	}
	else
	{
		auto temp=find_if(s_CriteriaResistance60sDivTestVoltagenN.rbegin(), s_CriteriaResistance60sDivTestVoltagenN.rend(),
				[=](std::pair<const int, int>  i){return i.first<=m_Rating.m_Resistance60sDivTestVoltage;});
		m_Rating.m_RateResistance60sDivTestVoltage=temp->second;
	}
	return m_Rating.m_RateResistance60sDivTestVoltage;
}
int RatingInsulation::rateTimeReconstruction()
{
	if (Test::returnsRatedVoltage() >= SN)
	{
		if (Test::returnsTimeReconstruction() >= 240)
			m_Rating.m_RateTimeReconstruction = s_CriteriaTimeReconstructionSN[240];
		else if (Test::returnsTimeReconstruction() < 240 && Test::returnsTimeReconstruction() >= 120)
			m_Rating.m_RateTimeReconstruction = s_CriteriaTimeReconstructionSN[120];
		else if (Test::returnsTimeReconstruction() < 120 && Test::returnsTimeReconstruction() >= 30)
			m_Rating.m_RateTimeReconstruction = s_CriteriaTimeReconstructionSN[30];
		else if (Test::returnsTimeReconstruction() < 120 && Test::returnsTimeReconstruction() >= 30)
			m_Rating.m_RateTimeReconstruction = s_CriteriaTimeReconstructionSN[30];
		else if (Test::returnsTimeReconstruction() < 30 && Test::returnsTimeReconstruction() >= 10)
			m_Rating.m_RateTimeReconstruction = s_CriteriaTimeReconstructionSN[10];
		else if (Test::returnsTimeReconstruction() == 0 && Test::returnsTimeShortCircuit() == 1)
			m_Rating.m_RateTimeReconstruction = s_CriteriaTimeReconstructionSN[0];
		else if (Test::returnsTimeReconstruction() == 0 && Test::returnsTimeShortCircuit() == 0)
			m_Rating.m_RateTimeReconstruction = s_CriteriaTimeReconstructionSN[0] - 1;
	}
	else
	{
		auto temp = find_if(s_CriteriaTimeReconstructionnN.rbegin(), s_CriteriaTimeReconstructionnN.rend(),
				[=](std::pair<const int, int>  i){return i.first<=Test::returnsTimeReconstruction();});
		m_Rating.m_RateTimeReconstruction = temp->second;
	}
	return m_Rating.m_RateTimeReconstruction;
}
int RatingInsulation::rateTimeShortCircuit()
{
	if (Test::returnsRatedVoltage() >= SN)
	{
		if (Test::returnsTimeShortCircuit() == 30 && Test::returnsTimeReconstruction() >= 240)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitSN[30];
		else if (Test::returnsTimeShortCircuit() == 30 && Test::returnsTimeReconstruction() < 240	&& returnsTimeReconstruction() >= 120)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitSN[30] - 1;
		else if (Test::returnsTimeShortCircuit() == 30 && Test::returnsTimeReconstruction() < 120	&& returnsTimeReconstruction() >= 30)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitSN[30] - 2;
		else if (Test::returnsTimeShortCircuit() == 1 && Test::returnsTimeReconstruction() < 30 && returnsTimeReconstruction() >= 10)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitSN[1];
		else if (Test::returnsTimeShortCircuit() == 1 && Test::returnsTimeReconstruction() == 0)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitSN[1] - 1;
		else if (Test::returnsTimeShortCircuit() == 0)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitSN[0];
	}
	else
	{
		if (Test::returnsTimeShortCircuit() == 10 && Test::returnsTimeReconstruction() >= 120)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitnN[10];
		else if (Test::returnsTimeShortCircuit() == 10 && Test::returnsTimeReconstruction() < 120	&& Test::returnsTimeReconstruction() >= 60)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitnN[10] - 1;
		else if (Test::returnsTimeShortCircuit() == 10 && Test::returnsTimeReconstruction() < 60)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitnN[10] - 2;
		else if (Test::returnsTimeShortCircuit() == 1 && Test::returnsTimeReconstruction() < 15 && Test::returnsTimeReconstruction() >= 5)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitnN[1];
		else if (Test::returnsTimeShortCircuit() == 0)
			m_Rating.m_RateTimeShortCircuit = s_CriteriaTimeShortCircuitnN[0];
	}
	return m_Rating.m_RateTimeShortCircuit;
}
int RatingInsulation::rateMaxVoltageDivTestVoltage()
{
	auto temp=find_if(s_CriteriaMaxVoltageDivTestVoltage.rbegin(), s_CriteriaMaxVoltageDivTestVoltage.rend(),
			[=](std::pair<const double, int>  i){return i.first<=m_Rating.m_MaxVoltageDivTestVoltage;});
	return m_Rating.m_RateMaxVoltageDivTestVoltage=temp->second;
}
int RatingInsulation::rateResistance60DivResistance15s()
{
	if (Test::returnsRatedVoltage() >= SN)
	{
		if (m_Rating.m_Resistance60sDivResistance15s >= 1.5)
			m_Rating.m_RateResistance60DivResistance15s = s_CriteriaResistance60DivResistance15sSN[1.5];
		else if (m_Rating.m_Resistance60sDivResistance15s < 1.5 && m_Rating.m_Resistance60sDivResistance15s >= 1.2)
			m_Rating.m_RateResistance60DivResistance15s = s_CriteriaResistance60DivResistance15sSN[1.2];
		else if (m_Rating.m_Resistance60sDivResistance15s < 1.2 && m_Rating.m_Resistance60sDivResistance15s > 1.0)
			m_Rating.m_RateResistance60DivResistance15s = s_CriteriaResistance60DivResistance15sSN[1.0];
		else if (m_Rating.m_Resistance60sDivResistance15s == 1.0 && returnsTimeReconstruction() < 30 && returnsTimeReconstruction() >= 10)
			m_Rating.m_RateResistance60DivResistance15s = s_CriteriaResistance60DivResistance15sSN[1.0] - 1;
		else if (m_Rating.m_Resistance60sDivResistance15s == 1.0 && returnsTimeReconstruction() == 0)
			m_Rating.m_RateResistance60DivResistance15s = 0;
	}
	else
	{
		if (m_Rating.m_Resistance60sDivResistance15s >= 1.3)
			m_Rating.m_RateResistance60DivResistance15s = s_CriteriaResistance60DivResistance15snN[1.3];
		else if (m_Rating.m_Resistance60sDivResistance15s < 1.5 && m_Rating.m_Resistance60sDivResistance15s >= 1.1)
			m_Rating.m_RateResistance60DivResistance15s = s_CriteriaResistance60DivResistance15snN[1.1];
		else if (m_Rating.m_Resistance60sDivResistance15s < 1.1 && m_Rating.m_Resistance60sDivResistance15s > 1.0)
			m_Rating.m_RateResistance60DivResistance15s = s_CriteriaResistance60DivResistance15snN[1.0];
		else if (m_Rating.m_Resistance60sDivResistance15s == 1.0 && rateTimeShortCircuit()==0)
			m_Rating.m_RateResistance60DivResistance15s = 0;
	}
	return m_Rating.m_RateResistance60DivResistance15s;
}
double RatingInsulation::rateTotal()
{
	m_Rating.m_RateTotal = 0.0;

	m_Rating.m_Resistance60sDivTestVoltage = returnsResistanceAfter60s() *1000/ returnsTestVoltage();
	m_Rating.m_MaxVoltageDivTestVoltage = returnsMaxVoltage() / returnsTestVoltage();
	m_Rating.m_Resistance60sDivResistance15s = returnsResistanceAfter60s() / returnsResistanceAfter15s();

	m_Rating.m_RateTotal += rateResistance60sDivTestVoltage();
	m_Rating.m_RateTotal += rateTimeReconstruction();
	m_Rating.m_RateTotal += rateTimeShortCircuit();
	m_Rating.m_RateTotal += rateMaxVoltageDivTestVoltage();
	m_Rating.m_RateTotal += rateResistance60DivResistance15s();
	m_Rating.m_RateTotal /= 5;

	return m_Rating.m_RateTotal;
}
void RatingInsulation::showRate() const
{
	using std::cout;
	using std::endl;
	using std::fixed;
	using std::setprecision;
	using std::setw;
	using std::left;
	using std::right;
	using std::setfill;
	const int wt = 50;
	const int wt2 = 12;
	const int wt3= 15;

	Test::showTest();
	cout  << fixed << setprecision(2);
	cout  << setw(wt) << left
			<< "Rezystancja po 60s przez napiecie probiercze: " << right
			<< setw(wt2) << m_Rating.m_Resistance60sDivTestVoltage<<" kOhm/V"
			<<setw(7)<< " Ocena: " << m_Rating.m_RateResistance60sDivTestVoltage << endl;
	cout  << setw(wt) << left << "Czas zwarcia: " << right
			<< setw(wt2) << Test::returnsTimeShortCircuit() <<setw(7)<<left<<" s "
			<<right<< " Ocena: " << m_Rating.m_RateTimeShortCircuit << endl;
	cout  << setw(wt) << left
			<< "Napiecie odbudowy przez napiecie probiercze: " << right
			<< setw(wt2) << m_Rating.m_MaxVoltageDivTestVoltage
			<<setw(wt3)<< " Ocena: " << m_Rating.m_RateMaxVoltageDivTestVoltage << endl;
	cout << setw(wt) << left << "Czas odbudowy napiecia: "
			<< right  << setw(wt2) << Test::returnsTimeReconstruction()<<setw(7)<<left<<" s "
			<<right<< " Ocena: "  << m_Rating.m_RateTimeReconstruction << endl;
	cout << setw(wt) << left
			<< "Rezystancja po 60s przez rezystancje po 15s: " << right
			<< setw(wt2) << m_Rating.m_Resistance60sDivResistance15s
			<<setw(wt3)<< " Ocena: "<< m_Rating.m_RateResistance60DivResistance15s << endl;
	cout << setw(wt) << left << "Ocena koncowa: " << right
			 << setw(wt2) << m_Rating.m_RateTotal << endl;
}
Test & RatingInsulation::returnsTest()
{
	return (Test &)*this;
}
void RatingInsulation::resetRate()
{
	Test::resetTest();
	m_Rating.m_Resistance60sDivTestVoltage = 0.0;
	m_Rating.m_MaxVoltageDivTestVoltage = 0.0;
	m_Rating.m_Resistance60sDivResistance15s = 0.0;
	m_Rating.m_RateResistance60sDivTestVoltage = 0;
	m_Rating.m_RateTimeShortCircuit = 0;
	m_Rating.m_RateTimeReconstruction = 0;
	m_Rating.m_RateMaxVoltageDivTestVoltage = 0;
	m_Rating.m_RateResistance60DivResistance15s = 0;
	m_Rating.m_RateTotal = 0.0;
}
void RatingInsulation::writeRatingInsulation(const std::string &name)const
{
	using namespace std;

	Test::writeTest(name);

	ofstream outFile;
	outFile.open(name, ios_base::out | ios_base::app | ios_base::binary);
	if (!outFile.is_open())
	{
		cerr << "Cannot open file: " << name << endl;
		exit(EXIT_FAILURE);
	}

	int sizeStruct = sizeof m_Rating;
	outFile.write((char *) &sizeStruct, sizeof sizeStruct);
	outFile.write((char *) &m_Rating, sizeStruct);

	outFile.close();
}
std::streampos RatingInsulation::getRatingInsulation(const std::string &name, std::streampos p)
{
	using namespace std;

	streampos place = p;

	place = Test::getTest(name, p);

	ifstream inFile;
	inFile.open(name, ios_base::in | ios_base::binary);

	if (!inFile.is_open())
	{
		cerr << "Cannot open file: " << name << endl;
		exit(EXIT_FAILURE);
	}
	inFile.seekg(place);
	int sizeStruct;
	inFile.read((char *) &sizeStruct, sizeof sizeStruct);
	inFile.read((char *) &m_Rating, sizeStruct);

	place = inFile.tellg();
	inFile.close();
	return place;
}
void RatingInsulation::restoreRateInitialValue()
{
	Test::restoreTestInitialValue();

	m_Rating.m_Resistance60sDivTestVoltage = 0.0;
	m_Rating.m_MaxVoltageDivTestVoltage = 0.0;
	m_Rating.m_Resistance60sDivResistance15s = 0.0;
	m_Rating.m_RateResistance60sDivTestVoltage = 0;
	m_Rating.m_RateTimeShortCircuit = 0;
	m_Rating.m_RateTimeReconstruction = 0;
	m_Rating.m_RateMaxVoltageDivTestVoltage = 0;
	m_Rating.m_RateResistance60DivResistance15s = 0;
	m_Rating.m_RateTotal = 0.0;
}
void RatingInsulation::resetPlotRate()
{
	m_Rating.m_Resistance60sDivTestVoltage = 0.0;
	m_Rating.m_MaxVoltageDivTestVoltage = 0.0;
	m_Rating.m_Resistance60sDivResistance15s = 0.0;
	m_Rating.m_RateResistance60sDivTestVoltage = 0;
	m_Rating.m_RateTimeShortCircuit = 0;
	m_Rating.m_RateTimeReconstruction = 0;
	m_Rating.m_RateMaxVoltageDivTestVoltage = 0;
	m_Rating.m_RateResistance60DivResistance15s = 0;
	m_Rating.m_RateTotal = 0.0;
}
