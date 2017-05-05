
#include "stdafx.h"
#include "string"
#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h>
#include <sstream>
using namespace std;

int ac = 0;
void CQ_sendGroupMsg(int ac, int qun, const char* msg) {
	string aa = msg;
	cout << "Ⱥ����" << aa << endl;
}

void CQ_sendPrivateMsg(int ac, int qun, const char* msg) {
	string aa = msg;
	cout << "˽�ģ�" << aa << endl;
}

static const string  cardDest[54] = {
	"��","��",
	"2","3","4","5","6","7","8","9","10","J","Q","K","A",
	"2","3","4","5","6","7","8","9","10","J","Q","K","A",
	"2","3","4","5","6","7","8","9","10","J","Q","K","A",
	"2","3","4","5","6","7","8","9","10","J","Q","K","A",
};

static const string flag[15] = { "3","4","5","6","7","8","9","10","J","Q","K","A","2", "��","��" };

const int STATE_WAIT = 0;
const int STATE_START = 1;
const int STATE_BOSSING = 2;
const int STATE_GAMEING = 2;

int findAndRemove(vector<string> &dest, string str) {

	for (unsigned i = 0; i < dest.size(); i++) {
		if (dest.at(i) == str) {
			vector<string>::iterator it = dest.begin() + i;
			dest.erase(it);
			return i;
		}
	}
	return -1;
}

int find(vector<string> &dest, string str) {

	for (unsigned i = 0; i < dest.size(); i++) {
		if (dest.at(i) == str) {
			return i;
		}
	}
	return -1;
}

int findFlag(string str)
{

	for (int i = 0; i < 15; i++) {
		if (flag[i] == str) {
			return i;
		}
	}
	return -1;

}

int desc(int a, int b)
{
	return a>b;
}

int asc(int a, int b)
{
	return a<b;
}

bool compareCard(const string &carda, const string &cardb)
{
	return findFlag(carda) < findFlag(cardb);
}


void trim(string &s)
{

	int index = 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != string::npos)
		{
			s.erase(index, 1);
		}
	}

}

void toUpper(string &str) {
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}


class Player
{
public:
	Player();
	stringstream msg;
	int64_t number;
	vector<string> card;
	int32_t socre;
	bool isReady;

	void sendMsg();
	void listCards();
	void breakLine();
};

Player::Player() {
	socre = 5000;
	isReady = false;
}

class Desk {
public:

	Desk();
	string cards[54];
	int64_t number;
	vector<Player*> players;

	int state;
	int lastPlayIndex;//��ǰ˭������
	int nextPlayIndex;//��˭����
	int bossIndex;//˭�ǵ���

	vector<string> lastCard;//��λ��ҵ���
	string lastCardType;//��λ��ҵ�����
	vector<int> *lastWeights;//��λ��ҵ���

	stringstream msg;

	void join(int64_t playNum);
	void startGame();
	static void gameOver(int64_t number);
	void exit(int64_t playNum);
	void commandList();

	void shuffle();//ϴ��
	void deal();//����
	void creataBoss();//������
	void getBoss(int64_t playerNum);
	void dontBoss(int64_t playerNum);
	void sendBossCard();
	void play(int64_t playNum, string msg);
	void play(int64_t playNum, vector<string> list);//����
	void discard(int64_t playNum);

	void at(int64_t playNum);
	void breakLine();
	int getPlayer(int64_t number);//��qq�Ż����ҵ�����
	void setNextPlayerIndex();//�����¸����Ƶ��������
	void listPlayers();
	void listPlayers(int type);
	bool isCanWin(int cardCount, vector<int> *Weights, string type);
	string getMycardType(vector<string> list, vector<int> *Weights);
	void sendMsg();
	void sendPlayerMsg();

};
Desk::Desk() {
	for (int i = 0; i<54; i++) {
		this->cards[i] = cardDest[i];
	}

	this->state = 0;
	this->lastPlayIndex = -1;//��ǰ˭������
	this->nextPlayIndex = -1;//��˭����
	this->bossIndex = -1;//˭�ǵ���

	vector<string> lastCard;//��λ��ҵ���
	this->lastCardType = "";//��λ��ҵ�����
	this->lastWeights = new vector<int>;//��λ��ҵ���
}

class Desks {
public:
	vector<Desk*> desks;
	Desk* getOrCreatDesk(int64_t deskNum);
	void game(int64_t deskNum, int64_t playNum, const char *msg);
	int getDesk(int64_t deskNum);
};


static Desks datas;


void Player::sendMsg()
{
	string tmp = this->msg.str();
	if (tmp.empty()) {
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}
	CQ_sendPrivateMsg(ac, this->number, tmp.data());
	this->msg.str("");
}

void Desk::at(int64_t playNum)
{
	this->msg << "[CQ:at,qq=" << playNum << "]";
}

void Desk::breakLine()
{
	this->msg << "\r\n";
}

int Desk::getPlayer(int64_t number) {
	for (unsigned i = 0; i < players.size(); i++) {
		if (players[i]->number == number) {
			return i;
		}
	}
	return -1;
}


void Desk::listPlayers()
{
	this->msg << "���:";
	this->breakLine();
	this->listPlayers(1);
}

void Desk::listPlayers(int type)
{

	bool hasType = (type & 1) == 1;
	bool hasOwn = ((type >> 1) & 1) == 1;

	for (unsigned i = 0; i < this->players.size(); i++) {
		this->msg << i + 1 << ":";
		if (hasType) {
			this->msg << "[" << (i == this->bossIndex && state == STATE_GAMEING ? "����" : "ũ��") << "]";
		}

		this->msg << "[CQ:at,qq=" << this->players[i]->number << "]";
		if (hasOwn) {
			if (this->bossIndex == this->nextPlayIndex) {//����ǵ���Ӯ��
				this->msg << "[" << (i == this->bossIndex ? "ʤ��" : "ʧ��") << "]";
			}
			else {
				this->msg << "[" << (i == this->bossIndex ? "ʧ��" : "ʤ��") << "]";
			}
		}

		this->breakLine();
	}
}

bool Desk::isCanWin(int cardCount, vector<int> *weights, string type)
{

	if (type == "" || this->lastCardType == "��ը") {
		return false;
	}

	if (this->lastCardType == "") {
		return true;
	}

	if (type == "��ը") {
		return true;
	}
	if (type == "ը��" && type != this->lastCardType) {
		return true;
	}


	if (type == this->lastCardType && cardCount == this->lastCard.size()) {

		for (unsigned i = 0; i < weights->size(); i++) {
			if (weights[i] > this->lastWeights[i]) {
				return true;
			}
		}

	}


	return false;
}



string Desk::getMycardType(vector<string> list, vector<int> *weights)
{
	int cardCount = list.size();
	sort(list.begin(), list.end(), compareCard);

	if (cardCount == 2 && findFlag(list[0]) + findFlag(list[1]) == 27) {//��ը
		return "��ը";
	}

	vector<string> cards;
	vector<int> counts;

	for (unsigned i = 0; i < list.size(); i++) {
		int index = find(cards, list[i]);
		if (index == -1) {
			cards.push_back(list[i]);
			counts.push_back(1);
		}
		else {
			counts[index] = counts[index] + 1;
		}
	}

	int max = counts[0];//��Ŵ�ֵ
	int min = counts[0];//���Сֵ
	int cardGroupCout = cards.size();
	int tmp;
	for (unsigned i = 0; i < counts.size(); i++) {
		tmp = counts[i];
		if (tmp > max) {
			max = tmp;
		}
		if (tmp < min) {
			min = tmp;
		}
	}

	vector<int> tmpCount(counts);
	sort(tmpCount.begin(), tmpCount.end(), desc);
	if (cardCount == 1) {//����
		weights->push_back(findFlag(cards[0]));
		return "����";
	}
	if (cardCount == 2 && max == 2) {//����
		weights->push_back(findFlag(cards[0]));
		return "����";
	}
	if (cardCount == 3 && max == 3) {//����
		weights->push_back(findFlag(cards[0]));
		return "����";
	}
	if (cardCount == 4 && max == 4) {//ը��
		weights->push_back(findFlag(cards[0]));
		return "ը��";
	}

	if (cardCount == 4 && max == 3) {//3��1

		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "3��1";
	}

	if (cardCount == 5 && max == 3 && min == 2) {//3��2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "3��2";
	}

	if (cardCount == 6 && max == 4) {//4��2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "4��2";
	}

	if (cardGroupCout > 2 && max == 2 && min == 2
		&& findFlag(cards[0]) == findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& findFlag(cards[cardGroupCout - 1]) < 13
		) {//����
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "����";
	}

	if (cardGroupCout > 4 && max == 1 && min == 1
		&& findFlag(cards[0]) == findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& findFlag(cards[cardGroupCout - 1]) < 13
		) {//˳��
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return "˳��";
	}

	//�ɻ�
	int  planeCount = 0;
	for (unsigned i = 0; i < counts.size() && counts[i] >= 3; i++, planeCount++);
	if (planeCount>1) {
		string tmp;
		if (cardCount == planeCount * 3) {
			tmp = "�ɻ�";
		}
		else if (cardCount == planeCount * 4) {
			tmp = "����ɻ�";
		}
		else if (cardCount == planeCount * 5 && min == 2) {
			tmp = "˫��ɻ�";
		}

		for (int i = 0; i < planeCount; i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		sort(weights->begin(), weights->end(), desc);

		int weightscount = weights->size();

		if (weights->at(0) - weightscount + 1 != weights->at(weightscount - 1)) {
			return "";
		}

		return tmp;
	}
	return "";
}





void Desk::sendMsg()
{

	string tmp = this->msg.str();
	if (tmp.empty()) {
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}
	CQ_sendGroupMsg(ac, this->number, tmp.data());
	this->msg.str("");
}

void Desk::sendPlayerMsg()
{
	for (unsigned i = 0; i < this->players.size(); i++) {
		players[i]->sendMsg();
	}
}



void Desk::shuffle() {
	srand((unsigned)time(NULL));
	for (unsigned i = 0; i < 54; i++) {
		swap(this->cards[i], this->cards[rand() % 54]);
	}
}

void Desk::creataBoss() {

	state = STATE_BOSSING;

	int index = rand() % 3;

	this->bossIndex = index;
	this->nextPlayIndex = index;
	this->at(this->players[index]->number);
	this->msg << "���Ƿ�Ҫ������";
	this->breakLine();
}

void Desk::getBoss(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_BOSSING && this->nextPlayIndex == index) {

		this->bossIndex = index;
		sendBossCard();
	}
}

void Desk::dontBoss(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_BOSSING && this->nextPlayIndex == index) {

		this->setNextPlayerIndex();

		if (this->nextPlayIndex == this->bossIndex) {
			this->sendBossCard();
		}
		else {
			this->msg << "[CQ:at,qq=" << this->players[index]->number << "] "
				<< "������"
				<< "[CQ:at,qq=" << this->players[nextPlayIndex]->number << "] "
				<< "���Ƿ�Ҫ������";
		}
	}
}

void Desk::sendBossCard()
{
	Player *playerBoss = players[this->bossIndex];

	this->msg << "[CQ:at,qq=" << playerBoss->number << "] "
		<< "�ǵ��������ǣ�"
		<< "[" << this->cards[53] << "]"
		<< "[" << this->cards[52] << "]"
		<< "[" << this->cards[51] << "]"
		<< "�����";
	this->breakLine();

	for (int i = 0; i < 3; i++) {
		playerBoss->card.push_back(cards[53 - i]);
	}
	sort(playerBoss->card.begin(), playerBoss->card.end(), compareCard);

	playerBoss->msg << "���ǵ������յ����ƣ�";
	for (unsigned m = 0; m < playerBoss->card.size(); m++) {
		playerBoss->msg << "[" << playerBoss->card.at(m) << "]";
	}
	playerBoss->breakLine();

	state = STATE_GAMEING;
}

void Desk::play(int64_t playNum, string msg)
{

	int length = msg.length();
	char *msgs = new char[length];
	strncpy(msgs, msg.data(), length);
	vector<string> msglist;

	stringstream data;
	string msgFindTmp;
	for (int i = 2; i < length; i++) {

		data << msgs[i];
		msgFindTmp = msgs;
		int gIndex = msgFindTmp.find("��");
		int wIndex = msgFindTmp.find("��");
		int tenIndex = msgFindTmp.find("10");
		if (i == gIndex || i == wIndex || i == tenIndex) {
			data << msgs[i + 1];
			msgs[i++] = -1;
			msgs[i] = -1;
		}

		msglist.push_back(data.str());
		data.str("");
	}

	this->play(playNum, msglist);

}

void Desk::play(int64_t playNum, vector<string> list)
{

	int playIndex = this->getPlayer(playNum);

	if (playIndex == -1 || playIndex != this->nextPlayIndex || this->state != STATE_GAMEING) {
		return;
	}

	Player *player = this->players[playIndex];
	vector<string> mycardTmp(player->card);

	int cardCount = list.size();

	for (int i = 0; i < cardCount; i++) {
		if (findAndRemove(mycardTmp, list[i]) == -1) {
			this->msg << "���û��������ƣ��᲻���棿";
			return;
		}
	}

	vector<int> *weights = new vector<int>;

	string type = this->getMycardType(list, weights);

	bool isCanWin = this->isCanWin(cardCount, weights, type);

	if (isCanWin) {
		if (mycardTmp.size() == 0) {//Ӯ�ˡ�
			this->msg << "��Ϸ����";
			this->breakLine();
			this->listPlayers(3);

			this->msg << (this->bossIndex == this->nextPlayIndex ? "����Ӯ��" : "ũ��Ӯ��");
			this->gameOver(this->number);
			return;
		}
		player->card = mycardTmp;
		this->lastWeights = weights;
		this->lastCard = list;
		this->lastCardType = type;
		this->lastPlayIndex = this->nextPlayIndex;


		player->listCards();

		if (player->card.size() < 3) {
			this->at(this->lastPlayIndex);
			this->msg << "��ʣ��" << player->card.size() << "����";
			this->breakLine();
		}

		this->msg << "�ϻغϣ�" << this->lastCardType;
		for (unsigned m = 0; m < this->lastCard.size(); m++) {
			this->msg << "[" << this->lastCard.at(m) << "]";
		}
		this->breakLine();
		this->setNextPlayerIndex();
		this->at(this->players[this->nextPlayIndex]->number);
		this->msg << "�����";
		this->breakLine();
	}
	else {
		this->msg << "��Ϲ���ѳ���";
		this->breakLine();
	}
}

void Desk::discard(int64_t playNum)
{
	if (this->nextPlayIndex != this->getPlayer(playNum) || this->state != STATE_GAMEING) {
		return;
	}

	if (this->nextPlayIndex == this->lastPlayIndex) {
		this->msg << "�����������ã��᲻���棬�㲻�ܹ�����";
		return;
	}


	this->msg << "�ϻغϣ�" << this->lastCardType;
	for (unsigned m = 0; m < this->lastCard.size(); m++) {
		this->msg << "[" << this->lastCard.at(m) << "]";
	}
	this->breakLine();
	this->msg << "��λ��ң�����";
	this->breakLine();
	this->setNextPlayerIndex();
	this->at(this->players[this->nextPlayIndex]->number);
	this->msg << "�����";
	this->breakLine();
}

void Desk::gameOver(int64_t number)
{
	int index = datas.getDesk(number);
	if (index == -1) {
		return;
	}
	vector<Desk*>::iterator it = datas.desks.begin() + index;
	datas.desks.erase(it);
	CQ_sendGroupMsg(ac, number, "��Ϸ����");
}

void Desk::exit(int64_t number)
{
	if (this->state == STATE_WAIT) {
		int index = this->getPlayer(number);
		if (index != -1) {
			vector<Player*>::iterator it = this->players.begin() + index;
			this->players.erase(it);
			this->msg << "�˳��ɹ�";
			this->breakLine();
		}

	}
	else {
		this->msg << "��Ϸ�Ѿ���ʼ�����˳�";
		this->breakLine();
	}
}

void Desk::commandList()
{
this->msg << "=    �����б�    =\r\n"
		<< 1 << " " << "������������Ϸ\r\n"
		<< 2 << " " << "�������� ���� ��23456\r\n"
		<< 3 << " " << "��������\r\n"
		<< 4 << " " << "������ | �������Ƿ�������\r\n"
		<< 5 << " " << "��ʼ��Ϸ���Ƿ�ʼ��Ϸ\r\n"
		<< 6 << " " << "�������˳���Ϸ��ֻ����׼������ʹ��\r\n"
		<< 7 << " " << "����б�����ǰ����Ϸ�е������Ϣ\r\n"
		<< 8 << " " << "����������ʾ�Ѿ���������\r\n";
	this->breakLine();
}

void Desk::setNextPlayerIndex()
{
	this->nextPlayIndex = this->nextPlayIndex == 2 ? 0 : this->nextPlayIndex + 1;

	if (this->nextPlayIndex == this->lastPlayIndex) {
		this->lastCard.clear();
		this->lastWeights->clear();
		this->lastCardType = "";
	}


}

void Desk::deal() {
	unsigned i, k, j;

	for (i = k = 0; i < 3; i++) {
		Player *player = players[i];

		for (j = 0; j < 17; j++) {
			player->card.push_back(cards[k++]);
		}

		sort(player->card.begin(), player->card.end(), compareCard);

		for (unsigned m = 0; m < player->card.size(); m++) {
			player->msg << "[" << player->card.at(m) << "]";
		}

		player->breakLine();
	}

}

int Desks::getDesk(int64_t deskNum) {

	for (unsigned i = 0; i < this->desks.size(); i++) {
		if (this->desks[i]->number == deskNum) {
			return i;
		}
	}

	return -1;
}

void Desk::join(int64_t playNum)
{

	int playIndex = this->getPlayer(playNum);

	if (playIndex != -1) {
		this->msg << "[CQ:at,qq=" << playNum << "] �Ѿ�������Ϸ";
		this->breakLine();
		return;
	}

	if (this->players.size() == 3) {
		this->msg << "[CQ:at,qq=" << playNum << "] ��������";
		this->breakLine();
		return;
	}

	Player *player = new Player;
	player->number = playNum;
	this->players.push_back(player);

	this->msg << "[CQ:at,qq=" << playNum << "] "
		<< "����ɹ����������:" << this->players.size() << "��";
	this->breakLine();
	if (this->players.size() == 3) {
		this->msg << "������[��ʼ��Ϸ]";
		this->breakLine();
	}
}

Desk* Desks::getOrCreatDesk(int64_t deskNum) {

	Desk *desk = NULL;
	int deskIndex = getDesk(deskNum);
	if (deskIndex == -1) {//û������
		desk = new Desk;
		desk->number = deskNum;
		desks.push_back(desk);
	}
	else {
		desk = datas.desks[deskIndex];
	}

	return desk;
}
void Desk::startGame() {
	if (this->players.size() == 3 && this->state == STATE_WAIT) {
		this->state = STATE_START;
		this->msg << "��Ϸ��ʼ";
		this->breakLine();

		this->listPlayers();

		this->shuffle();

		this->deal();

		this->creataBoss();
	}
	else {
		this->msg << "û���㹻����һ����Ѿ���ʼ��Ϸ";
		this->breakLine();
		this->listPlayers();
	}
}

void Desks::game(int64_t deskNum, int64_t playNum, const char* msgArray) {

	string msg = msgArray;
	trim(msg);
	toUpper(msg);

	Desk *desk = datas.getOrCreatDesk(deskNum);

	if (msg == "����" || msg == "������Ϸ" || msg == "JOIN") {
		desk->join(playNum);
	}
	else if (msg.find("��") == 0 || msg.find("����") == 0) {//���ƽ׶�
		desk->play(playNum, msg);
	}
	else if (msg == "��" || msg == "����" || msg == "����" || msg == "��Ҫ" || msg == "PASS") {//�������ƽ׶�
		desk->discard(playNum);
	}
	else if (msg == "�˳���Ϸ" || msg == "����" || msg == "����") {//������Ϸ
		desk->exit(playNum);
	}
	else if (msg == "�����б�") {
		desk->commandList();
	}
	else if ((msg == "������Ϸ" || msg == "GAMEOVER") && playNum == 895252155) {//������Ϸ
		desk->gameOver(deskNum);
		return;
	}
	else if (msg == "����б�") {
		desk->listPlayers();
	}
	else if (msg == "��ʼ��Ϸ") {
		desk->startGame();
	}
	else if (msg == "������" || msg == "��") {
		desk->getBoss(playNum);
	}
	else if (msg == "����") {
		desk->dontBoss(playNum);
	}
	else if (msg == "������") {
		desk->msg << "δ�������";
	}

	desk->sendMsg();
	desk->sendPlayerMsg();
}

void Player::listCards()
{
	for (unsigned m = 0; m < this->card.size(); m++) {
		this->msg << "[" << this->card.at(m) << "]";
	}
	this->breakLine();

}

void Player::breakLine()
{
	this->msg << "\r\n";
}




int main() {


	string info = "���";
	int64_t desknum = 10000000;
	int64_t playNum = 111111;
	datas.game(desknum, playNum, "����б�");

	for (int i = 0; i < 3; i++) {
		datas.game(desknum, playNum + i, "����");
	}

	datas.game(desknum, playNum, "��ʼ��Ϸ");

	while (true) {
		char str[30];
		cin.getline(str, 30);

		int index = datas.desks[datas.getDesk(desknum)]->nextPlayIndex;
		datas.game(desknum, playNum + index, str);

	}

	system("pause");
	return 0;
};
