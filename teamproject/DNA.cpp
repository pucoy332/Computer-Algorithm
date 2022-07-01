/*2017112138 정여준*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <random>
#include <cmath>
using namespace std;

#define DNA_SIZE 1000000
#define MISMATCH 3//mismatch 허용 개수

int GetRand(int min, int max); //random_device를 사용해서 난수 생성
void makeReferenceDNA(string DNA);//Reference D녹NA 시퀀스 생성
char change(); //ShortRead 길이당 0~2개의 다른 문자를 만들기 위해 변환하는 함수
void makeMyDNA(int Length, int Num); //My DNA 시퀀스를 My_DNA(시퀀스의 길이).txt 파일로 저장된다. 
void makeShortRead(string My_DNA, int Length, int Num);//입력받은 문자열에서 랜덤한 위치의 Short Read를 추출하여 파일에 저장하는 함수
void RestoreDNA(int Length, int Num); //ShortRead와  Reference DNA를 사용해서 My_DNA 복구
void misCount(string DNA_str1, string DNA_str2, int Length, int Num);//다른 횟수를 카운트하고 복구율을 출력하는 함수
vector<int> getPi(string p);


vector<int> getPi(string p) {  //prefix == suffix가 될 수 있는 문자열 중 가장 긴 것의 길이를 반환하는 함수
	int m = p.size(), j = 0; //j는 패턴의 현재 비교위치를 나타내는 변수
	vector<int> pi(m, 0); //사이즈 m의 정수형 벡터 선언하고 0으로 초기화
	for (int i = 1; i < m; i++) {
		while (j > 0 && p[i] != p[j]) //j가 0보다 크고 p[i] p[j]가 다를 경우 
			j = pi[j - 1]; //j를 pi[j-1]로 당긴다
		if (p[i] == p[j]) //p[i]와 p[j]가 일치할 떄
			pi[i] = ++j; //j를 1 증가시키고 pi[i]에 대입
	}
	return pi;
}

int GetRand(int min, int max) {
	random_device rn;
	mt19937_64 rnd(rn());
	uniform_int_distribution<int> range(min, max);

	return range(rnd);
}

void makeReferenceDNA(string DNA) 
{ 
	ofstream ofs("Reference_DNA.txt");
	for (int i = 0; i < DNA_SIZE; i++) {
		int randNum = GetRand(0, 3);
		ofs << DNA[randNum];
	}
	ofs.close();
}

char change()
{
	char c;
	int random = GetRand(0, 3);

	if (random == 0)
		c = 'A';
	else if (random == 1)
		c = 'C';
	else if (random == 2)
		c = 'G';
	else
		c = 'T';

	return c;
}

void makeMyDNA(int Length,int Num) 
{	
	ifstream ifs("Reference_DNA.txt");
	ofstream ofs("My_DNA" + to_string(Length) + ".txt");
	string reference_str; //Reference_DNA 시퀀스 문자열을 받아올 문자열 생성	
	string MyDNA_str;
	for (int i = 0; i < DNA_SIZE; i++)
	{
		char c;
		ifs >> c;
		reference_str += c;
	}
	MyDNA_str = reference_str;
	int i = 0;
	while (i < DNA_SIZE)
	{
		int a = GetRand(i, i + Length);
		int b = GetRand(i, i + Length);
		MyDNA_str[a] = change();
		MyDNA_str[b] = change();
		i += Length;
	}


	ofs << MyDNA_str;
	
	ifs.close();
	ofs.close();
}

void makeShortRead(string My_DNA, int Length, int Num) {
	ofstream ofs("ShortRead" + to_string(Length) + ".txt");
	for (int j = 0; j < Num; j++) {
		string str = "";
		int startPoint = GetRand(0, DNA_SIZE - Length);
		for (int i = 0; i < Length; i++) {
			str += My_DNA[startPoint + i];
		}
		ofs << str;
		ofs << endl;
	}
	ofs.close();
}

void RestoreDNA(int Length, int Num) {
	ifstream ifs1("ShortRead" + to_string(Length) + ".txt");
	ifstream ifs2("Reference_DNA.txt");
	ifstream ifs3("My_DNA" + to_string(Length) + ".txt");
	ofstream ofs("Restore_DNA" + to_string(Length) + ".txt");
	
	int miss = 0;
	vector<string> SR;
	for(int i=0; i<Num; i++)
	{
		string shortRead = "";
		getline(ifs1, shortRead);
		SR.push_back(shortRead);
	}
	
	string DNA_str = "";
	for (int i = 0; i < DNA_SIZE; i++)
	{
		char c;
		ifs2 >> c;
		DNA_str += c;
	}
	string My_DNA;
	for (int i = 0; i < DNA_SIZE; i++)
	{
		char c;
		ifs3 >> c;
		My_DNA += c;
	}
		
	string restore_str;
	for (int i = 0; i < DNA_SIZE; i++)
		restore_str += " ";

	vector<bool> flag(DNA_SIZE, false);
	for (int i = 0; i < Num; i++)
	{
		string shortRead = SR[i];
		vector<int> SRkmp = getPi(shortRead);
		int miss = 0;
		int tmp = 0, k = 0;
		for (int j = 0; j < DNA_SIZE - Length; j)
		{
			if (flag[j] == true)
				j = j + Length;
			while (k < Length)
			{
				if (DNA_str[j + k] != shortRead[k])
				{
					miss++;
					if (miss == 1) //처음 miss 발생하는 지점 저장
						tmp = k;
					if (miss < MISMATCH) //miss match 허용 개수보다 작은 경우
						k++; 
					else { //miss >= mism match
						if (miss == MISMATCH && k == Length - 1)
						{
							k++;
							break;
						}
						miss = 0; //허용개수 초과이므로 j를 이동해서 다시 비교 시작 miss 0으로 초기화
						if (tmp == 0) //처음부터 miss match가 발생한 경우
						{
							j++;
							k = 0;
						}
						else {
							j = j + tmp; //처음 mismatch가 발생한 곳을 시작으로 다시 비교한다.
							k = SRkmp[tmp - 1]; //tmp 이전까지 mismatch가 없었으므로 최대 접두부 테이블을 이용해 이동
						}
						break;
					}
				}
				else
					k++;
			}
			if (k == Length && miss <= MISMATCH && flag[j] == false) //ShortRead의 길이만큼 비교했을 때 miss가 MISMATCH 이하인 경우
			{
				flag[j] = true; //방문한 곳으로 체크
				for (int t = 0; t < Length; t++)
				{
					restore_str[j + t] = shortRead[t];
				}
				break;
			}
		}

	}
	
	ofs << restore_str;
	misCount(My_DNA, restore_str, Length, Num); //복구한 시퀀스와 My DNA 시퀀스 비교해서 복구율 구하기
	
	ifs1.close();
	ifs2.close();
	ifs3.close();
	ofs.close();
}

void misCount(string DNA_str1, string DNA_str2, int Length, int Num) {
		cout << "k =  " << Length << "\t" << "n = " << Num << "\t" << endl;
	int count = 0;
	for (int i = 0; i < DNA_SIZE; i++) {
		if (DNA_str1[i] != DNA_str2[i]) {
			count++;
		}
	}
	if (count == 0) cout << "모두 일치합니다." << endl;
	else {
		cout << count << "개 일치하지 않습니다.\n";
		printf("복구율 : %.2f%%\n", (float(DNA_SIZE) - float(count)) / float(DNA_SIZE) * 100);
	}
}

int main() {
	time_t start, end;
	double result;
	/*
	string DNA = "ACGT";
	int k1, k2;
	int n1, n2;
	cout << "길이 k 입력: ";
	cin >> k1;
	cout << "개수 n 입력: ";
	cin >> n1;
	
	
	makeReferenceDNA(DNA);
	makeMyDNA(k1, n1);
	
	ifstream ifs1("My_DNA" + to_string(k1) + ".txt");
	string DNA_str1;
	
	for (int i = 0; i < DNA_SIZE; i++)
	{
		char c;
		ifs1 >> c;
		DNA_str1 += c;
	}
	
	makeShortRead(DNA_str1, k1, n1);
	*/
	start = clock();
	RestoreDNA(30,100000);
	end = clock();
	result = (double)(end - start);
	cout << "실행시간: " << result / CLOCKS_PER_SEC << endl;
	
	//ifs1.close();
	
}
