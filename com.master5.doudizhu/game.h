
#include "stdafx.h"
#include "string"
#ifdef _DEBUG 
#else
#include "cqp.h"
#endif 
#include <time.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <tchar.h>  
#include <regex> 
using namespace std;
static const wstring  cardDest[54] = {
	L"��",L"��",
	L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"10",L"J",L"Q",L"K",L"A",
	L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"10",L"J",L"Q",L"K",L"A",
	L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"10",L"J",L"Q",L"K",L"A",
	L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"10",L"J",L"Q",L"K",L"A"
};

static const wstring flag[15] = { L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"10",L"J",L"Q",L"K",L"A",L"2",L"��",L"��" };

const int STATE_WAIT = 0;
const int STATE_START = 1;
const int STATE_BOSSING = 2;
const int STATE_MULTIPLING = 4;
const int STATE_READYTOGO = 4;
const int STATE_GAMEING = 5;

const wstring CONFIG_PATH = L".\\app\\com.master5.doudizu\\config.ini";
const wstring CONFIG_DIR = L".\\app\\com.master5.doudizu\\";

const int CONIFG_INIT_SCORE = 10000;
const int CONFIG_BOTTOM_SCORE = 1000;

const wregex allotReg(L"�������(\\d+)=(\\d+)");
const wregex numberReg(L"\\d+");

class Util {
public:
	static int AC;
	static void testMsg(bool subType, int64_t playNum, int64_t desknum, const char * str);
	static void sendGroupMsg(int64_t groupid, const char *msg);
	static void sendDiscussMsg(int64_t groupid, const char *msg);
	static void sendPrivateMsg(int64_t groupid, const char *msg);
	static int  findAndRemove(vector<wstring> &dest, wstring str);
	static int  find(vector<wstring> &dest, wstring str);
	static int  findFlag(wstring str);
	static int  desc(int a, int b);
	static int  asc(int a, int b);
	static bool  compareCard(const wstring &carda, const wstring &cardb);
	static void  trim(wstring &s);
	static void  toUpper(wstring &str);
	static void setAC(int32_t ac);
	static string wstring2string(wstring wstr);
	static wstring string2wstring(string str);
	static void mkdir();
};
class Admin {
public:
	static wstring readString();
	static int64_t readAdmin();
	static int64_t readScore(int64_t playerNum);
	static bool isAdmin(int64_t playNum);
	static bool writeAdmin(int64_t playerNum);
	static bool getScore(int64_t playerNum);
	static bool addScore(int64_t playerNum, int score);
	static bool IAmAdmin(int64_t playerNum);
	static bool resetGame(int64_t playNum);
	static bool allotScoreTo(wstring msg, int64_t playNum);
	static bool gameOver(wstring msg, int64_t playNum);
private:
	static bool writeScore(int64_t playerNum, int64_t score);
};

class Player
{
public:
	Player();
	wstringstream msg;
	int64_t number;
	vector<wstring> card;
	bool isReady;
	bool isOpenCard;//����״̬
	bool isSurrender;//Ͷ��״̬
	void sendMsg();
	void listCards();
	void breakLine();
};



class Desk {
public:

	Desk();
	int multiple;
	int turn;
	wstring cards[54];
	int64_t number;
	vector<Player*> players;

	int whoIsWinner;
	int state;
	int lastPlayIndex;//��ǰ˭������
	int currentPlayIndex;//��˭����
	int bossIndex;//˭�ǵ���

	vector<wstring> lastCard;//��λ��ҵ���
	wstring lastCardType;//��λ��ҵ�����
	vector<int> *lastWeights;//��λ��ҵ��Ƶ�Ȩ��

	wstringstream msg;

	void join(int64_t playNum);
	void startGame();
	void exit(int64_t playNum);
	void commandList();

	void shuffle();//ϴ��
	void deal();//����
	//������
	void creataBoss();
	void getBoss(int64_t playerNum);
	void dontBoss(int64_t playerNum);
	//���ӱ�
	void multipleChoice();
	void getMultiple(int64_t playerNum);
	void dontMultiple(int64_t playerNum);
	void sendBossCard();
	void play(int64_t playNum, wstring msg);
	void play(vector<wstring> list, int playIndex);//����
	void discard(int64_t playNum);
	void surrender(int64_t playNum);//Ͷ��
	void openCard(int64_t playNum);//����

	void getPlayerInfo(int64_t playNum);
	void getScore(int64_t playNum);

	void at(int64_t playNum);
	void breakLine();
	int getPlayer(int64_t number);//��qq�Ż����ҵ�����
	void setNextPlayerIndex();//�����¸����Ƶ��������
	void listPlayers(int type);
	bool isCanWin(int cardCount, vector<int> *Weights, wstring type);
	wstring getMycardType(vector<wstring> list, vector<int> *Weights);
	void sendMsg(bool subType);
	void sendPlayerMsg();
	void listCardsOnDesk(Player* player);

};

class Desks {
public:
	vector<Desk*> desks;
	Desk* getOrCreatDesk(int64_t deskNum);
	static bool game(bool subType,int64_t deskNum, int64_t playNum, const char *msg);
	static bool game(int64_t playNum, const char *msg);
	int getDesk(int64_t deskNum);
	void gameOver(int64_t deskNum);
	void listDesks();
};