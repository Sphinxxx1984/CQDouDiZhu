#pragma once 

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

#include "landlords_Util.h"
#include "landlords_Admin.h"
#include "landlords_Desks.h"
#include "landlords_Player.h"
#include "landlords_Watcher.h"

using namespace std;

int Util::AC = 0;

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
		<< L"10*. ���ƣ�����������Ϸ����������������ũ��������Ϸ����������ũ�����Ӯ�˲��÷֣�����˫���۷�" << "\r\n"
		<< L"11. ��ȡ���֣���ȡ���֣�ÿ��ɻ�ȡ200���֡�" << "\r\n"
		<< L"12. �ҵ���Ϣ|�ҵĻ��֣��鿴�ҵĻ�����Ϣ" << "\r\n"
		<< L"13. �����ս�����й۲�" << "\r\n"
		<< L"14. �˳���ս����������Ŀ�����" << "\r\n"
		<< L"A1. " << L"���ǹ�������Ϸ����ԱΪ��ǰ������Ϣ��qq������Ա��ʹ�ù�������������ú��ܸ���" << "\r\n"
		<< L"A2. " << L"���ö�������ɾ���������á����ú�������趨����Ա" << "\r\n"
		<< L"A3. " << L"������Ϸ[Ⱥ��]������ָ��Ⱥ�ŵ���Ϸ�����磺������Ϸ123456" << "\r\n"
		<< L"A4. " << L"�������[qq��]=[����]����ָ��qq������֣��磺�������123456=500"
		;
	this->breakLine();
}

bool Desks::game(bool subType, int64_t deskNum, int64_t playNum, const char* msgArray) {

	string tmp = msgArray;

	wstring msg = Util::string2wstring(tmp);
	Util::trim(msg);
	Util::toUpper(msg);

	Desk *desk = casino.getOrCreatDesk(deskNum);

	if (msg.find(L"�������汾") == 0) {
		desk->msg << L"3.1.1 dev23 201802061909";
		desk->breakLine();
		desk->msg << L"Դ���������������https://github.com/doowzs/CQDouDiZhu";
		desk->breakLine();
		desk->msg << L"ԭ������2.0.1Դ���룺https://github.com/lsjspl/CQDouDiZhu";
	}
	else if (msg.find(L"����") == 0 || msg.find(L"����") == 0
		|| msg.find(L"����") == 0) {
		desk->join(playNum);
	}
	else if ((desk->state >= STATE_READYTOGO) &&
		(msg.find(L"��") == 0 || msg.find(L"��") == 0)) {//���ƽ׶�
		desk->play(playNum, msg);
	}
	else if ((desk->state >= STATE_READYTOGO) &&
		(msg.find(L"��") == 0 || msg.find(L"����") == 0 || msg.find(L"����") == 0
			|| msg.find(L"û��") == 0 || msg.find(L"�򲻳�") == 0 || msg.find(L"Ҫ����") == 0
			|| msg.find(L"��Ҫ") == 0 || msg.find(L"PASS") == 0)) {//�������ƽ׶�
		desk->discard(playNum);
	}
	else if (msg.find(L"����") == 0 || msg.find(L"����") == 0
		|| msg.find(L"������") == 0) {//������Ϸ
		desk->exit(playNum);
	}
	else if (msg == L"�����������б�" || msg == L"�����������ȫ") {
		desk->commandList();
	}
	else if (msg == L"����б�") {
		desk->listPlayers(1);
	}
	else if (msg.find(L"GO") == 0 || msg.find(L"����") == 0) {
		desk->startGame();
	}
	else if ((msg.find(L"��") == 0 || msg.find(L"Ҫ") == 0) && desk->state == STATE_BOSSING) {
		desk->getBoss(playNum);
	}
	else if (msg.find(L"��") == 0 && desk->state == STATE_BOSSING) {
		desk->dontBoss(playNum);
	}
	else if (msg.find(L"��") == 0 && desk->state == STATE_MULTIPLING) {
		desk->getMultiple(playNum);
	}
	else if (msg.find(L"��") == 0 && desk->state == STATE_MULTIPLING) {
		desk->dontMultiple(playNum);
	}
	else if (msg.find(L"����") == 0) {
		desk->openCard(playNum);
	}
	else if ((msg.find(L"����") == 0)
		&& desk->state >= STATE_BOSSING) {
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
	else if (msg.find(L"�����ս") == 0) {
		desk->joinWatching(playNum);
	}
	else if (msg.find(L"�˳���ս") == 0) {
		desk->exitWatching(playNum);
	}
	else {
		return false;
	}

	desk->sendMsg(subType);
	desk->sendPlayerMsg();
	desk->sendWatcherMsg();
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