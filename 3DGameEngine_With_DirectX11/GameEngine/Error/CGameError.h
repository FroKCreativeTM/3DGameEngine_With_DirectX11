#pragma once

#include "../Game.h"

namespace NSGameError {
	const int FATAL_ERROR = -1;
	const int WARNING = 1;
}

class CGameError :public std::exception {
private:
	int					m_errorCode;	// ���� �ڵ带 �����Ѵ�.
	std::string			m_message;		// �� ������ �� ������ ǥ���ϱ� ���� ���ڿ� �����

public:
	CGameError() throw() : m_errorCode(NSGameError::FATAL_ERROR),
		m_message("Undefined Error in game.") {}
	CGameError(const CGameError& error) throw() : std::exception(error),
		m_errorCode(error.m_errorCode),
		m_message(error.m_message) {}
	CGameError(int code, const std::string& rhs) :
		m_errorCode(code),
		m_message(rhs)
	{}

	CGameError operator=(const CGameError& rhs) {
		std::exception::operator=(rhs);
		this->m_errorCode = rhs.m_errorCode;
		this->m_message = rhs.m_message;
	}

	// �Ҹ���
	virtual ~CGameError() {}

	const char* GetCharMessage() const throw() {
		return this->m_message.c_str();
	}

	int GetCode() const throw() {
		return this->m_errorCode;
	}

	virtual const char* what() const throw() {
		return this->GetCharMessage();
	}
};

