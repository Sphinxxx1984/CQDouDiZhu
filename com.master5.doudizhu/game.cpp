#include "stdafx.h"
#include "game.h"

using namespace std;

int Util::AC = 0;

static Desks casino;


void Util::testMsg(bool subType, int64_t desknum, int64_t playNum, const char * str) {
	int index = casino.desks[0]->currentPlayIndex;
	casino.game(subType, desknum, playNum + index, str);
}


void Util::sendGroupMsg(int64_t groupid, const char *msg) {
#ifdef _DEBUG  
	string aa = msg;
	cout << "Ⱥ����" << aa << endl;
#else
	CQ_sendGroupMsg(Util::AC, groupid, msg);
#endif
}


void Util::sendDiscussMsg(int64_t groupid, const char *msg) {
#ifdef _DEBUG  
	string aa = msg;
	cout << "Ⱥ����" << aa << endl;
#else
	CQ_sendDiscussMsg(Util::AC, groupid, msg);
#endif
}

void Util::sendPrivateMsg(int64_t number, const char* msg) {
#ifdef _DEBUG  
	string aa = msg;
	cout << "˽��" << number << "��" << aa << endl;
#else
	CQ_sendPrivateMsg(Util::AC, number, msg);
#endif
}


//��stringת����wstring  
wstring Util::string2wstring(string str)
{
	wstring result;
	//��ȡ��������С��������ռ䣬��������С���ַ�����  
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	//���ֽڱ���ת���ɿ��ֽڱ���  
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
	buffer[len] = '\0';             //����ַ�����β  
									//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}

void Util::mkdir()
{
	CreateDirectory(CONFIG_DIR.c_str(), NULL);
}

//��wstringת����string  
string Util::wstring2string(wstring wstr)
{
	string result;
	//��ȡ��������С��������ռ䣬��������С�°��ֽڼ����  
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	//���ֽڱ���ת���ɶ��ֽڱ���  
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	//ɾ��������������ֵ  
	result.append(buffer);
	delete[] buffer;
	return result;
}

int Util::findAndRemove(vector<wstring> &dest, wstring str) {

	for (unsigned i = 0; i < dest.size(); i++) {
		if (dest.at(i) == str) {
			vector<wstring>::iterator it = dest.begin() + i;
			dest.erase(it);
			return i;
		}
	}
	return -1;
}

int Util::find(vector<wstring> &dest, wstring str) {

	for (unsigned i = 0; i < dest.size(); i++) {
		if (dest.at(i) == str) {
			return i;
		}
	}
	return -1;
}

int Util::findFlag(wstring str)
{

	for (int i = 0; i < 15; i++) {
		if (flag[i] == str) {
			return i;
		}
	}
	return -1;

}

int Util::desc(int a, int b)
{
	return a > b;
}

int Util::asc(int a, int b)
{
	return a < b;
}

bool Util::compareCard(const wstring &carda, const wstring &cardb)
{
	return findFlag(carda) < findFlag(cardb);
}


void Util::trim(wstring &s)
{

	int index = 0;
	if (!s.empty())
	{
		while ((index = s.find(' ', index)) != wstring::npos)
		{
			s.erase(index, 1);
		}
	}

}

void Util::toUpper(wstring &str) {
	transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void Util::setAC(int32_t ac)
{
	Util::AC = ac;
}


Desk::Desk() {
	for (int i = 0; i < 54; i++) {
		this->cards[i] = cardDest[i];
	}

	this->state = 0;
	this->lastPlayIndex = -1;//��ǰ˭������
	this->currentPlayIndex = -1;//��˭����
	this->bossIndex = -1;//˭�ǵ���

	vector<wstring> lastCard;//��λ��ҵ���
	this->lastCardType = L"";//��λ��ҵ�����
	this->lastWeights = new vector<int>;//��λ��ҵ���

	this->whoIsWinner = 0;
	this->multiple = 1;
	this->turn = 0;

}

Player::Player() {
	this->isReady = false;
	this->isOpenCard = false;
	this->isSurrender = false;
}
void Player::sendMsg()
{
	wstring tmp = this->msg.str();
	if (tmp.empty()) {
		this->msg.str(L"");
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}

	Util::sendPrivateMsg(this->number, Util::wstring2string(tmp).data());
	this->msg.str(L"");
}

void Desk::at(int64_t playNum)
{
	this->msg << L"[CQ:at,qq=" << playNum << L"]";
}

void Desk::breakLine()
{
	this->msg << L"\r\n";
}

int Desk::getPlayer(int64_t number) {
	for (unsigned i = 0; i < players.size(); i++) {
		if (players[i]->number == number) {
			return i;
		}
	}
	return -1;
}

void Desk::listPlayers(int type)
{

	bool hasType = (type & 1) == 1;
	bool hasWin = ((type >> 1) & 1) == 1;

	int score = CONFIG_BOTTOM_SCORE* this->multiple;
	int halfScore = score / 2;
	this->msg << L"���ֱ�����" << this->multiple << L"x";
	this->breakLine();
	this->msg << L"���ƴ���(���������)��" << this->turn;
	this->breakLine();

	for (unsigned i = 0; i < this->players.size(); i++) {
		this->msg << i + 1 << L":";
		if (hasType) {
			this->msg << L"[" << (i == this->bossIndex && state == STATE_GAMEING ? L"����" : L"ũ��") << L"]";
		}

		this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]";
		if (hasWin) {
			if (this->whoIsWinner == 2) {//�����ũ��Ӯ��
				if (i == this->bossIndex) {
					this->msg << L"[" << L"ʧ��-" << score << L"��]";
					Admin::addScore(this->players[i]->number, -score);
				}
				else if (this->players[i]->isSurrender) {
					this->msg << L"[" << L"����+" << 0 << L"��]";
				}
				else {
					this->msg << L"[" << L"ʤ��+" << halfScore << L"��]";
					Admin::addScore(this->players[i]->number, halfScore);
				}
			}
			else {
				if (i == this->bossIndex) {
					this->msg << L"[" << L"ʤ��+" << score << L"��]";
					Admin::addScore(this->players[i]->number, score);
				}
				else if (this->players[i]->isSurrender) {
					this->msg << L"[" << L"����-" << score << L"��]";
					Admin::addScore(this->players[i]->number, -score);
				}
				else {
					this->msg << L"[" << L"ʧ��-" << halfScore << L"��]";
					Admin::addScore(this->players[i]->number, -halfScore);
				}
			}
		}

		this->breakLine();
	}
}

bool Desk::isCanWin(int cardCount, vector<int> *weights, wstring type)
{

	if (type == L"" || this->lastCardType == L"��ը") {
		return false;
	}

	if (this->lastCardType == L"") {
		return true;
	}

	if (type == L"��ը") {
		return true;
	}
	if (type == L"ը��" && type != this->lastCardType) {
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



wstring Desk::getMycardType(vector<wstring> list, vector<int> *weights)
{
	int cardCount = list.size();
	sort(list.begin(), list.end(), Util::compareCard);

	if (cardCount == 2 && Util::findFlag(list[0]) + Util::findFlag(list[1]) == 27) {//��ը
		return L"��ը";
	}

	vector<wstring> cards;
	vector<int> counts;

	for (unsigned i = 0; i < list.size(); i++) {
		int index = Util::find(cards, list[i]);
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
	sort(tmpCount.begin(), tmpCount.end(), Util::desc);
	if (cardCount == 1) {//����
		weights->push_back(Util::findFlag(cards[0]));
		return L"����";
	}
	if (cardCount == 2 && max == 2) {//����
		weights->push_back(Util::findFlag(cards[0]));
		return L"����";
	}
	if (cardCount == 3 && max == 3) {//����
		weights->push_back(Util::findFlag(cards[0]));
		return L"����";
	}
	if (cardCount == 4 && max == 4) {//ը��
		weights->push_back(Util::findFlag(cards[0]));
		return L"ը��";
	}

	if (cardCount == 4 && max == 3) {//3��1

		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"3��1";
	}

	if (cardCount == 5 && max == 3 && min == 2) {//3��2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"3��2";
	}

	if (cardCount == 6 && max == 4) {//4��2
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"4��2";
	}

	if (cardGroupCout > 2 && max == 2 && min == 2
		&& Util::findFlag(cards[0]) == Util::findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& Util::findFlag(cards[cardGroupCout - 1]) < 13
		) {//����
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"����";
	}

	if (cardGroupCout > 4 && max == 1 && min == 1
		&& Util::findFlag(cards[0]) == Util::findFlag(cards[cardGroupCout - 1]) - cardGroupCout + 1
		&& Util::findFlag(cards[cardGroupCout - 1]) < 13
		) {//˳��
		for (unsigned i = 0; i < tmpCount.size(); i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		return L"˳��";
	}

	//�ɻ�
	int  planeCount = 0;
	for (unsigned i = 0; i < counts.size(); i++) {
		if (counts[i] >= 3) {
			planeCount++;
		}
	}
	if (planeCount > 1) {
		wstring tmp;
		if (cardCount == planeCount * 3) {
			tmp = L"�ɻ�";
		}
		else if (cardCount == planeCount * 4) {
			tmp = L"�ɻ������";
		}
		else if (cardCount == planeCount * 5 && min == 2) {
			tmp = L"�ɻ���˫���";
		}

		for (int i = 0; i < planeCount; i++) {
			int tmp = tmpCount[i];
			for (unsigned m = 0; m < counts.size(); m++) {
				if (counts[m] == tmp) {
					weights->push_back(Util::findFlag(cards[m]));
					counts[m] = -1;
				}
			}
		}

		sort(weights->begin(), weights->end(), Util::desc);

		int weightscount = weights->size();

		if (weights->at(0) - weightscount + 1 != weights->at(weightscount - 1)) {
			return L"";
		}

		return tmp;
	}
	return L"";
}

void Desk::sendMsg(bool subType)
{
	wstring tmp = this->msg.str();
	if (tmp.empty()) {
		return;
	}
	int length = tmp.length();
	if (tmp[length - 2] == '\r' && tmp[length - 1] == '\n') {
		tmp = tmp.substr(0, length - 2);
	}
	if (subType) {
		Util::sendGroupMsg(this->number, Util::wstring2string(tmp).data());
	}
	else {
		Util::sendDiscussMsg(this->number, Util::wstring2string(tmp).data());
	}

	this->msg.str(L"");
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

	this->msg << L"����������������ڡ�";
	this->breakLine();
	this->msg << L"---------------";
	this->breakLine();

	int index = rand() % 3;

	this->bossIndex = index;
	this->currentPlayIndex = index;
	this->at(this->players[index]->number);
	this->breakLine();
	this->msg << L"���Ƿ�Ҫ��������";
	this->breakLine();
	this->msg << L"����[��(����)]��[����(����)]���ش�";
	this->breakLine();
}

void Desk::getBoss(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_BOSSING && this->currentPlayIndex == index) {

		this->bossIndex = index;
		this->currentPlayIndex = index;
		this->lastPlayIndex = index;
		sendBossCard();
	}

	//����ӱ�����
	this->state = STATE_MULTIPLING;
	this->multipleChoice();
}

void Desk::dontBoss(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_BOSSING && this->currentPlayIndex == index) {

		this->setNextPlayerIndex();

		if (this->currentPlayIndex == this->bossIndex) {
			this->sendBossCard();
		}
		else {
			this->msg << L"[CQ:at,qq=" << this->players[index]->number << L"] "
				<< L"����������";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
			this->msg << L"[CQ:at,qq=" << this->players[currentPlayIndex]->number << L"] "
				<< L"���Ƿ�Ҫ��������";
		}
	}
}

void Desk::sendBossCard()
{
	Player *playerBoss = players[this->bossIndex];

	this->msg << L"[CQ:at,qq=" << playerBoss->number << L"] "
		<< L"�ǵ����������ǣ�";
	this->breakLine();
	this->msg << L"[" << this->cards[53] << L"]"
		<< L"[" << this->cards[52] << L"]"
		<< L"[" << this->cards[51] << L"]��";
	this->breakLine();
	this->msg << L"---------------";
	this->breakLine();

	for (int i = 0; i < 3; i++) {
		playerBoss->card.push_back(cards[53 - i]);
	}
	sort(playerBoss->card.begin(), playerBoss->card.end(), Util::compareCard);

	playerBoss->msg << L"���ǵ������յ����ƣ�";
	playerBoss->breakLine();
	for (unsigned m = 0; m < playerBoss->card.size(); m++) {
		playerBoss->msg << L"[" << playerBoss->card.at(m) << L"]";
	}
	playerBoss->breakLine();

	//�����״̬�����ƶ����˼ӱ�(dontMultiple)�����һ����������

}

void Desk::multipleChoice() {
	this->currentPlayIndex = this->bossIndex;

	this->msg << L"���������ڽ������������ӱ����ڡ�";
	this->breakLine();
	this->msg << L"---------------";
	this->breakLine();
	this->at(this->players[this->bossIndex]->number);
	this->breakLine();
	this->msg << L"���Ƿ�Ҫ�ӱ���";
	this->breakLine();
	this->msg << L"����[��(��)]��[����(��)]���ش�";
	this->breakLine();
}

void Desk::getMultiple(int64_t playerNum)
{
	this->multiple++;
	
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_MULTIPLING && this->currentPlayIndex == index) {

		this->setNextPlayerIndex();

		if (this->currentPlayIndex == this->bossIndex && bossHasMultipled) {
			this->msg << L"[CQ:at,qq=" << this->players[index]->number << L"] "
				<< L"Ҫ�ӱ���";
			this->breakLine();
			this->msg << L"��ǰ���ֱ�����" << this->multiple << L"x";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();

			this->state = STATE_READYTOGO;

			this->msg << L"�ӱ����ڽ�������������ʽ��ʼ��";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
			this->msg << L"��" << this->turn + 1 << L"�غϣ���ǰʣ����������";
			this->breakLine();
			for (unsigned i = 0; i < this->players.size(); i++) {
				this->msg << i + 1 << L":";
				this->msg << L"[" << (i == this->bossIndex ? L"����" : L"ũ��") << L"]"; //����ɾ������&& state == STATE_GAMEING 
				this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]";
				this->msg << L"��" << static_cast<int>(this->players[i]->card.size());
				this->breakLine();
			}
			this->breakLine();
			this->msg << L"�����" << L"[CQ:at, qq = " << this->players[this->bossIndex]->number << L"]�ȳ��ơ�";
			this->breakLine();
		}
		else {
			bossHasMultipled = true;

			this->msg << L"[CQ:at,qq=" << this->players[index]->number << L"] "
				<< L"Ҫ�ӱ���";
			this->breakLine();
			this->msg << L"��ǰ���ֱ�����" << this->multiple << L"x";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
			this->msg << L"[CQ:at,qq=" << this->players[currentPlayIndex]->number << L"] "
				<< L"���Ƿ�Ҫ�ӱ���";
			this->breakLine();
			this->msg << L"����[��(��)]��[����(��)]���ش�";
			this->breakLine();
		}
	}
}

void Desk::dontMultiple(int64_t playerNum)
{
	int index = this->getPlayer(playerNum);
	if (this->state == STATE_MULTIPLING && this->currentPlayIndex == index) {

		this->setNextPlayerIndex();

		if (this->currentPlayIndex == this->bossIndex && bossHasMultipled) {
			this->msg << L"[CQ:at,qq=" << this->players[index]->number << L"] "
				<< L"��Ҫ�ӱ���";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();

			this->state = STATE_READYTOGO;

			this->msg << L"�ӱ����ڽ�������������ʽ��ʼ��";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
			this->msg << L"��" << this->turn + 1 << L"�غϣ���ǰʣ����������";
			this->breakLine();
			for (unsigned i = 0; i < this->players.size(); i++) {
				this->msg << i + 1 << L":";
				this->msg << L"[" << (i == this->bossIndex ? L"����" : L"ũ��") << L"]"; //����ɾ������&& state == STATE_GAMEING 
				this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]";
				this->msg << L"��" << static_cast<int>(this->players[i]->card.size());
				this->breakLine();
			}
			this->breakLine();
			this->msg << L"�����" << L"[CQ:at, qq = " << this->players[this->bossIndex]->number << L"]�ȳ��ơ�";
			this->breakLine();
		}
		else {
			bossHasMultipled = true;

			this->msg << L"[CQ:at,qq=" << this->players[index]->number << L"] "
				<< L"��Ҫ�ӱ���";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
			this->msg << L"[CQ:at,qq=" << this->players[currentPlayIndex]->number << L"] "
				<< L"���Ƿ�Ҫ�ӱ���";
			this->breakLine();
			this->msg << L"����[��(��)]��[����(��)]���ش�";
			this->breakLine();
		}
	}
}

void Desk::play(int64_t playNum, wstring msg)
{
	int playIndex = this->getPlayer(playNum);
	int length = msg.length();

	if (playIndex == -1 || playIndex != this->currentPlayIndex
		|| (!(this->state == STATE_GAMEING && this->turn > 0)
			&& !(this->state == STATE_READYTOGO && this->turn == 0))
		|| length < 2) {
		return;
	}

	vector<wstring> msglist;

	for (int i = 1; i < length; i++) {
		wstring tmp = msg.substr(i, 1);
		if (tmp == L"1") {
			tmp = msg.substr(i, 2);
			i++;
		}
		msglist.push_back(tmp);
	}

	this->play(msglist, playIndex);

}

void Desk::play(vector<wstring> list, int playIndex)
{

	Player *player = this->players[playIndex];
	vector<wstring> mycardTmp(player->card);

	int cardCount = list.size();

	for (int i = 0; i < cardCount; i++) {
		if (Util::findAndRemove(mycardTmp, list[i]) == -1) {
			this->msg << L"[CQ:at,qq=" << this->players[currentPlayIndex]->number << L"] ";
			this->breakLine();
			this->msg << L"�涪�ˣ����û����Ҫ�����ƣ��᲻���棿";
			return;
		}
	}

	vector<int> *weights = new vector<int>;

	wstring type = this->getMycardType(list, weights);

	bool isCanWin = this->isCanWin(cardCount, weights, type);

	if (isCanWin) {

		if (this->turn == 0) {
			this->state = STATE_GAMEING;
		}

		player->card = mycardTmp;
		this->lastWeights = weights;
		this->lastCard = list;
		this->lastCardType = type;
		this->lastPlayIndex = this->currentPlayIndex;
		this->turn++;


		//�������
		if (type == L"��ը") {
			this->multiple += 2;

			this->msg << L"�����ը�����ֱ���+2";
			this->breakLine();
			this->msg << L"��ǰ���ֱ�����" << this->multiple << L"x";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
		}
		else if (type == L"ը��") {
			this->multiple += 1;

			this->msg << L"���ը�������ֱ���+1";
			this->breakLine();
			this->msg << L"��ǰ���ֱ�����" << this->multiple << L"x";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
		}

		if (mycardTmp.size() == 0) {//Ӯ�ˡ�
			this->whoIsWinner = this->bossIndex == this->currentPlayIndex ? 1 : 2;

			this->msg << L"---------------";
			this->breakLine();
			this->msg << L"��Ϸ����";
			this->breakLine();
			this->listPlayers(3);

			this->msg << (this->whoIsWinner == 1 ? L"����Ӯ��" : L"ũ��Ӯ��");
			casino.gameOver(this->number);
			return;
		}

		player->listCards();

		if (player->isOpenCard) {
			this->at(player->number);
			this->msg << L"���ƣ�";
			this->listCardsOnDesk(player);
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
		}

		if (player->card.size() < 3) {
			this->msg << L"��ɫ��������ɫ������";
			this->breakLine();
			this->at(player->number);
			this->msg << L"��ʣ��" << player->card.size() << L"���ƣ�";
			this->breakLine();
			this->msg << L"---------------";
			this->breakLine();
		}

		this->msg << L"�ϻغϣ�" << this->lastCardType;
		for (unsigned m = 0; m < this->lastCard.size(); m++) {
			this->msg << L"[" << this->lastCard.at(m) << L"]";
		}
		this->breakLine();

		this->setNextPlayerIndex();
		this->msg << L"---------------";
		this->breakLine();
		this->msg << L"��" << this->turn + 1 << L"�غϣ�";
		this->breakLine();
		this->msg << L"��ǰ���ֱ�����" << this->multiple << L"x";
		this->breakLine();
		this->msg << L"��ǰʣ����������";
		this->breakLine();
		for (unsigned i = 0; i < this->players.size(); i++) {
			this->msg << i + 1 << L":";
			this->msg << L"[" << (i == this->bossIndex && state == STATE_GAMEING ? L"����" : L"ũ��") << L"]";
			this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]";
			this->msg << L"��" << static_cast<int>(this->players[i]->card.size());
			this->breakLine();
		}
		this->breakLine();
		this->msg << L"�����ֵ�";
		this->at(this->players[this->currentPlayIndex]->number);
		this->msg << L"����";
		this->breakLine();
	}
	else {
		this->at(this->players[this->currentPlayIndex]->number);
		this->breakLine();
		this->msg << L"ɵ�����ѣ����ʲô�������⣡ѧ������ٴ�";
		this->breakLine();
	}
}

void Desk::discard(int64_t playNum)
{
	if (this->currentPlayIndex != this->getPlayer(playNum) || this->state != STATE_GAMEING) {
		return;
	}

	if (this->currentPlayIndex == this->lastPlayIndex) {
		this->msg << L"�����������ã��᲻���棬�㲻�ܹ����ˣ�����";
		return;
	}


	this->msg << L"�ϻغϣ�" << this->lastCardType;
	for (unsigned m = 0; m < this->lastCard.size(); m++) {
		this->msg << L"[" << this->lastCard.at(m) << L"]";
	}
	this->breakLine();
	this->msg << L"��λ��ң�����";
	this->breakLine();
	this->msg << L"---------------";
	this->breakLine();

	this->setNextPlayerIndex();
	this->msg << L"��" << this->turn + 1 << L"�غϣ���ǰʣ����������";
	this->breakLine();
	for (unsigned i = 0; i < this->players.size(); i++) {
		this->msg << i + 1 << L":";
		this->msg << L"[" << (i == this->bossIndex && state == STATE_GAMEING ? L"����" : L"ũ��") << L"]";
		this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]";
		this->msg << L"��" << static_cast<int>(this->players[i]->card.size());
		this->breakLine();
	}
	this->breakLine();
	this->msg << L"�����ֵ�";
	this->at(this->players[this->currentPlayIndex]->number);
	this->msg << L"����";
	this->breakLine();
}

void Desk::surrender(int64_t playNum)
{
	int index = this->getPlayer(playNum);
	if (index == -1 || this->state != STATE_GAMEING || this->players[index]->isSurrender) {
		return;
	}

	Player *player = this->players[index];

	player->isSurrender = true;

	if (index == this->bossIndex) {
		this->whoIsWinner = 2;//ũ��Ӯ
	}
	else {
		for (size_t i = 0; i < this->players.size(); i++) {
			if (players[i]->isSurrender && i != this->bossIndex && i != index) {
				this->whoIsWinner = 1;
				break;
			}
		}
	}

	if (this->whoIsWinner > 0) {
		this->msg << L"��������Ϸ����";
		this->breakLine();
		this->listPlayers(3);

		this->msg << (this->whoIsWinner == 1 ? L"����Ӯ��" : L"ũ��Ӯ��");
		casino.gameOver(this->number);
		return;
	}


	if (this->currentPlayIndex == index) {
		this->msg << L"�ϻغϣ�" << this->lastCardType;
		for (unsigned m = 0; m < this->lastCard.size(); m++) {
			this->msg << L"[" << this->lastCard.at(m) << L"]";
		}
		this->breakLine();
		this->msg << L"��λ��ң����ƣ����䣩";
		this->breakLine();
		this->msg << L"---------------";
		this->breakLine();
		this->setNextPlayerIndex();
		this->msg << L"�����ֵ�";
		this->at(this->players[this->currentPlayIndex]->number);
		this->msg << L"����";
		this->breakLine();
	}
	else {
		this->at(playNum);
		this->msg << L"���ƣ����䣩��";
		this->breakLine();
	}

}

void Desk::openCard(int64_t playNum)
{

	int index = this->getPlayer(playNum);
	if (index == -1 || this->state > STATE_READYTOGO || this->state < STATE_START) {
		return;
	}
	Player *player = this->players[index];

	if (!player->isOpenCard) {
		player->isOpenCard = true;
		this->multiple += 2;
	}

	this->at(playNum);
	this->msg << L"���ƣ����ֱ���+2��";
	this->breakLine();

	this->listCardsOnDesk(player);
	this->breakLine();

	this->msg << L"---------------";
	this->breakLine();
	this->msg << L"��ǰ���ֱ�����" << this->multiple << L"x";


}

void Desk::getPlayerInfo(int64_t playNum)
{
	this->at(playNum);
	this->breakLine();
	this->msg << L"��Ļ���Ϊ��" << Admin::readScore(playNum);
	this->breakLine();
}

void Desk::getScore(int64_t playNum)
{
	this->at(playNum);
	if (Admin::getScore(playNum)) {
		this->breakLine();
		this->msg << L"���ǽ����500����֣�ף�����ճ�Ϊ������ר�ң�";
		this->breakLine();
		this->msg << L"�����ڵĻ����ܶ�Ϊ" << Admin::readScore(playNum) << L"��";
		this->msg << L"��ȡ��������������������Ǻ͹���Ա";
		this->at((int64_t)Admin::readAdmin());
		this->msg << L"����py���ף�";
	}
	else {
		this->msg << L"������Ѿ��ù����֣�";
		this->breakLine(); 
		this->msg << L"��ȡ��������������������Ǻ͹���Ա";
		this->at((int64_t)Admin::readAdmin());
		this->msg << L"����py���ף�";
	}
	this->breakLine();
}

void Desks::gameOver(int64_t number)
{
	int index = casino.getDesk(number);
	if (index == -1) {
		return;
	}
	vector<Desk*>::iterator it = casino.desks.begin() + index;
	casino.desks.erase(it);
	Util::sendGroupMsg(number, "��Ϸ����");
}

void Desk::exit(int64_t number)
{
	if (this->state == STATE_WAIT) {
		int index = this->getPlayer(number);
		if (index != -1) {
			vector<Player*>::iterator it = this->players.begin() + index;
			this->players.erase(it);
			this->msg << L"�˳��ɹ���ʣ�����" << this->players.size() << L"��";
			if (this->players.size() > 0) {
				this->msg << L"���ֱ��ǣ�";
				this->breakLine();
				for (unsigned i = 0; i < this->players.size(); i++) {
					this->msg << i + 1 << L":";
					this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]�����֣�";
					this->msg << Admin::readScore(this->players[i]->number);
					this->breakLine();
				}
			}
			else {
				this->msg << L"��";
			}
		}

	}
	else {
		this->msg << L"��Ϸ�Ѿ���ʼ�����˳���";
		this->breakLine();
		this->msg << L"�������ʹ��[����]��[����]���˳���Ϸ��";
	}
}

void Desk::commandList()
{
	this->msg << L"------ �����б� ------" << "\r\n"
		<< L"------ ������������ħ�İ棩------" << "\r\n"
		<< L"------ *�ű�ʾ֧�ֺ������ ------" << "\r\n"
		<< L"1*. ����|���ƣ�������Ϸ\r\n"
		<< L"2*. ��|�򣺳��� ���� ��23456��\r\n"
		<< L"3*. ��(��)|��Ҫ|pass������\r\n"
		<< L"4*. ��(����)|�������Ƿ�������\r\n"
		<< L"5*. ��(��)|����(��)���Ƿ�ӱ�\r\n"
		<< L"6*. ��ʼ|����|GO���Ƿ�ʼ��Ϸ\r\n"
		<< L"7*. ����|�����ˣ��˳���Ϸ��ֻ����׼������ʹ��\r\n"
		<< L"8. ����б���ǰ����Ϸ�е������Ϣ\r\n"
		<< L"9*. ���ƣ���ʾ�Լ����Ƹ�������ң����ƻᵼ�»��ַ�����ֻ���ڷ����ƺ��Լ�����֮ǰʹ�á�\r\n"
		<< L"10*. ����|���䣺����������Ϸ����������������ũ��������Ϸ����������ũ�����Ӯ�˲��÷֣�����˫���۷�" << "\r\n"
		<< L"11. ��ȡ���֣���ȡ���֣�ÿ��ɻ�ȡ1w�֡�" << "\r\n"
		<< L"12. �ҵ���Ϣ|�ҵĻ��֣��鿴�ҵĻ�����Ϣ" << "\r\n"
		<< L"A1. " << L"���ǹ�������Ϸ����ԱΪ��ǰ������Ϣ��qq������Ա��ʹ�ù�������������ú��ܸ���" << "\r\n"
		<< L"A2. " << L"���ö�������ɾ���������á����ú�������趨����Ա" << "\r\n"
		<< L"A3. " << L"������Ϸ[Ⱥ��]������ָ��Ⱥ�ŵ���Ϸ�����磺������Ϸ123456" << "\r\n"
		<< L"A4. " << L"�������[qq��]=[����]����ָ��qq������֣��磺�������123456=500"
		;
	this->breakLine();
}

void Desk::setNextPlayerIndex()
{
	this->currentPlayIndex = this->currentPlayIndex == 2 ? 0 : this->currentPlayIndex + 1;

	if (this->currentPlayIndex == this->lastPlayIndex) {
		this->lastCard.clear();
		this->lastWeights->clear();
		this->lastCardType = L"";
	}

	//�����һ���ó��Ƶ�������������� ������set��һλ���
	//���ڲ����ܴ���2�������� ������һ����һ��û������
	if (this->players[this->currentPlayIndex]->isSurrender) {
		this->setNextPlayerIndex();
	}


}

void Desk::deal() {
	unsigned i, k, j;

	for (i = k = 0; i < 3; i++) {
		Player *player = players[i];

		for (j = 0; j < 17; j++) {
			player->card.push_back(cards[k++]);
		}

		sort(player->card.begin(), player->card.end(), Util::compareCard);

		for (unsigned m = 0; m < player->card.size(); m++) {
			player->msg << L"[" << player->card.at(m) << L"]";
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
		this->msg << L"[CQ:at,qq=" << playNum << L"] ";
		this->breakLine();
		this->msg << L"���Ѿ�������Ϸ��";
		this->breakLine();
		return;
	}

	if (this->players.size() == 3) {
		this->msg << L"[CQ:at,qq=" << playNum << L"] ";
		this->breakLine();
		this->msg << L"���ź�������������";
		this->breakLine();
		return;
	}

	if (Admin::readScore(playNum) < 1) {
		this->msg << L"[CQ:at,qq=" << playNum << L"] ";
		this->breakLine();
		this->msg << L"��Ļ����Ѿ�����ˣ��޷��μӶ�������";
		this->breakLine();
		this->msg << L"������[��ȡ����]��ȡÿ�ջ��ֻ��ߺ͹���Ա";
		this->at((int64_t)Admin::readAdmin());
		this->msg << L"����py���ף�";
		return;//С��1��return�ˣ����治�����
	}

	Player *player = new Player;
	player->number = playNum;
	this->players.push_back(player);

	this->msg << L"[CQ:at,qq=" << playNum << L"] ������" << Admin::readScore(playNum);
	this->breakLine();
	this->msg << L"����ɹ����������" << this->players.size() << L"�����ֱ��ǣ�";
	this->breakLine();
	for (unsigned i = 0; i < this->players.size(); i++) {
		this->msg << i + 1 << L":";
		this->msg << L"[CQ:at,qq=" << this->players[i]->number << L"]�����֣�";
		this->msg << Admin::readScore(this->players[i]->number);
		this->breakLine();
	}

	if (Admin::readScore(playNum) < 300) {
		this->msg << "---------------";
		this->breakLine();
		this->msg << L"[CQ:at,qq=" << playNum << L"] ";
		this->breakLine();
		this->msg << L"��Ļ��ֽ�Ϊ" << Admin::readScore(playNum) << L"�㣬������van��Ϸ����̫�������ܵ����ۣ�";
		this->breakLine();
		this->msg << L"��������ϰ���������Ƽ���";
		this->breakLine();
		this->msg << "---------------";
		this->breakLine();
	}
	

	if (this->players.size() == 3) {
		this->breakLine();
		this->msg << L"����������";
		this->msg << L"������[��ʼ]��[GO]��������Ϸ��";
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
		desk = casino.desks[deskIndex];
	}

	return desk;
}

void Desk::startGame() {
	if (this->players.size() == 3 && this->state == STATE_WAIT) {
		this->state = STATE_START;
		this->msg << L"��Ϸ��ʼ���������׼�����ڣ�׼�����ڿ��Խ���[����]���������ƻ�ʹ���ֱ���+2�����������";
		this->breakLine();
		this->msg << L"---------------";
		this->breakLine();

		this->listPlayers(1);

		this->shuffle();

		this->deal();

		this->creataBoss();
	}
	else {
		this->msg << L"û���㹻����һ����Ѿ���ʼ��Ϸ��";
		this->breakLine();
		this->listPlayers(1);
	}
}

bool Desks::game(bool subType, int64_t deskNum, int64_t playNum, const char* msgArray) {

	string tmp = msgArray;

	wstring msg = Util::string2wstring(tmp);
	Util::trim(msg);
	Util::toUpper(msg);

	Desk *desk = casino.getOrCreatDesk(deskNum);

	if (msg.find(L"����") == 0 || msg.find(L"����") == 0 || msg.find(L"����") == 0 
		|| msg.find(L"����") == 0) {
		desk->join(playNum);
	}
	else if (msg.find(L"��") == 0 || msg.find(L"��") == 0) {//���ƽ׶�
		desk->play(playNum, msg);
	}
	else if (msg.find(L"��") == 0 || msg.find(L"����") == 0 || msg.find(L"����") == 0 
		|| msg.find(L"û��") == 0 || msg.find(L"�򲻳�") == 0 || msg.find(L"Ҫ����") == 0
		|| msg.find(L"��Ҫ") == 0 || msg.find(L"PASS") == 0) {//�������ƽ׶�
		desk->discard(playNum);
	}
	else if (msg.find(L"�˳���Ϸ") == 0 || msg.find(L"����") == 0 || msg.find(L"����") == 0
		|| msg.find(L"������") == 0) {//������Ϸ
		desk->exit(playNum);
	}
	else if (msg == L"�����������б�" || msg == L"�����������ȫ") {
		desk->commandList();
	}
	else if (msg == L"����б�") {
		desk->listPlayers(1);
	}
	else if (msg.find(L"GO") == 0 || msg.find(L"��ʼ") == 0 || msg.find(L"����") == 0) {
		desk->startGame();
	}
	else if (msg.find(L"��") == 0 || msg.find(L"Ҫ") == 0) {
		desk->getBoss(playNum);
	}
	else if (msg.find(L"����") == 0 || msg.find(L"��Ҫ") == 0) {
		desk->dontBoss(playNum);
	}
	else if (msg.find(L"��") == 0) {
		desk->getMultiple(playNum);
	}
	else if (msg.find(L"����") == 0) {
		desk->dontMultiple(playNum);
	}
	else if (msg.find(L"����") == 0) {
		desk->openCard(playNum);
	}
	else if (msg.find(L"����") == 0 || msg.find(L"����") == 0) {
		desk->surrender(playNum);
	}
	else if (msg == L"������") {
		desk->msg << L"û�����أ�";
	}
	else if (msg == L"�ҵ���Ϣ" || msg == L"�ҵĻ���") {
		desk->getPlayerInfo(playNum);
	}
	else if (msg == L"��ȡ����" || msg == L"�������") {
		desk->getScore(playNum);
	}
	else {
		return false;
	}

	desk->sendMsg(subType);
	desk->sendPlayerMsg();
	return true;
}

bool Desks::game(int64_t playNum, const char * msgArray)
{
	string tmp = msgArray;

	wstring msg = Util::string2wstring(tmp);
	Util::trim(msg);
	Util::toUpper(msg);


	bool result;
	if (msg == L"���ǹ���") {
		result = Admin::IAmAdmin(playNum);
	}
	else if (msg == L"���ö�����" || msg == L"��ʼ��������") {
		result = Admin::resetGame(playNum);
	}
	else if (regex_match(msg, allotReg)) {
		result = Admin::allotScoreTo(msg, playNum);
	}
	else if (msg.find(L"������Ϸ") == 0) {//������Ϸ
		result = Admin::gameOver(msg, playNum);
	}
	else {
		return false;
	}

	msg = result ? L"�����ɹ������Ĺ���Ա" : L"�ǳ���Ǹ������ʧ��";
	Util::sendPrivateMsg(playNum, Util::wstring2string(msg).data());
	return true;
}

void Player::listCards()
{
	for (unsigned m = 0; m < this->card.size(); m++) {
		this->msg << L"[" << this->card.at(m) << L"]";
	}

}

void Desk::listCardsOnDesk(Player* player)
{
	for (unsigned m = 0; m < player->card.size(); m++) {
		this->msg << L"[" << player->card.at(m) << L"]";
	}
}

void Player::breakLine()
{
	this->msg << L"\r\n";
}


int64_t Admin::readAdmin()
{
	wstring model = L"admin";
	wstring key = L"admin";
	return GetPrivateProfileInt(model.c_str(), key.c_str(), 0, CONFIG_PATH.c_str());
}

bool Admin::isAdmin(int64_t playNum)
{
	return playNum == Admin::readAdmin();
}

wstring Admin::readString() {
	WCHAR tmp[15];
	GetPrivateProfileString(L"admin", L"admin", L"", tmp, 15, CONFIG_PATH.c_str());
	return wstring(tmp);
}

bool Admin::allotScoreTo(wstring msg, int64_t playNum)
{


	int score;
	int64_t playerNum;

	wsmatch mr;
	wstring::const_iterator src_it = msg.begin(); // ��ȡ��ʼλ��
	wstring::const_iterator src_end = msg.end(); // ��ȡ����λ��
	regex_search(src_it, src_end, mr, numberReg);
	wstringstream ss;
	ss << mr[0].str();
	ss >> playerNum;
	ss.str(L"");
	src_it = mr[0].second;
	regex_search(src_it, src_end, mr, numberReg);
	wstringstream scoress;
	scoress << mr[0].str();
	scoress >> score;
	scoress.str(L"");

	return Admin::isAdmin(playNum) && Admin::writeScore(playerNum, score < 0 ? 0 : score);
}

bool Admin::gameOver(wstring msg, int64_t playNum)
{
	if (Admin::isAdmin(playNum)) {
		wstringstream ss;
		ss << msg.substr(4, msg.size());
		int64_t destNum;
		ss >> destNum;
		ss.str(L"");
		casino.gameOver(destNum);
		return true;
	}
	return false;
}

bool Admin::writeAdmin(int64_t playerNum)
{
	wstring model = L"admin";
	wstring key = L"admin";
	wstringstream ss;
	ss << playerNum;
	wstring value = ss.str();
	ss.str(L"");
	return WritePrivateProfileString(model.c_str(), key.c_str(), value.c_str(), CONFIG_PATH.c_str());

}

int64_t Admin::readScore(int64_t playerNum)
{
	wstring model = L"score";
	wstringstream ss;
	ss << playerNum;
	wstring key = ss.str();
	ss.str(L"");
	return GetPrivateProfileInt(model.c_str(), key.c_str(), 0, CONFIG_PATH.c_str());
}

bool Admin::getScore(int64_t playerNum)
{
	wstring model = L"time";
	wstringstream ss;
	ss << playerNum;
	wstring key = ss.str();
	ss.str(L"");
	int64_t lastGetScoreTime = GetPrivateProfileInt(model.c_str(), key.c_str(), 0, CONFIG_PATH.c_str());
	time_t rawtime;
	int64_t now = time(&rawtime);

	if (now / (24 * 60 * 60) > lastGetScoreTime / (24 * 60 * 60)) {
		Admin::addScore(playerNum, CONIFG_INIT_SCORE);
		ss << now;
		wstring value = ss.str();
		ss.str(L"");
		return WritePrivateProfileString(model.c_str(), key.c_str(), value.c_str(), CONFIG_PATH.c_str());
	}
	return false;
}

bool Admin::writeScore(int64_t playerNum, int64_t score)
{
	wstring model = L"score";
	wstringstream ss;
	ss << playerNum;
	wstring key = ss.str();
	ss.str(L"");
	ss << score;
	wstring value = ss.str();
	ss.str(L"");
	return WritePrivateProfileString(model.c_str(), key.c_str(), value.c_str(), CONFIG_PATH.c_str());
}

bool Admin::addScore(int64_t playerNum, int score)
{
	int64_t hasScore = Admin::readScore(playerNum) + score;
	return  Admin::writeScore(playerNum, hasScore < 0 ? 0 : hasScore);
}

bool Admin::IAmAdmin(int64_t playerNum)
{
	return  Admin::readAdmin() == 0 && Admin::writeAdmin(playerNum);
}

bool Admin::resetGame(int64_t playNum)
{
	return playNum == Admin::readAdmin() && DeleteFile(CONFIG_PATH.c_str());
}
