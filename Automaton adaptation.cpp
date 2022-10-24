#include <iostream>
#include <string>
#include <list>
#include <map>

class WhitespaceDelimitedTokensAutomaton
{
	enum class State
	{
		TokenExpexted,
		TokenBegin,
		TokenContinues,
		TokenEnd,
	};

	State state_{ State::TokenExpexted };

public:
	bool TokenBegin() const { return state_ == State::TokenBegin; }

	bool TokenBeingObserved() const { return state_ == State::TokenContinues || state_ == State::TokenBegin; }

	bool TokenEnd() const { return state_ == State::TokenEnd; }

	void Input(char c);

private:
	// �������� �� �����������
	// � ���� ������ ����� ������� ����������� �������� �� ������� ������ ���������
	static bool IsWhitespace_(char c)
	{
		switch (c)
		{
		case ' ':
		case '\t':
		case '\n':
		case '\r':
			return true;
		}
		return false;
	}

	static bool IsEol_(char c) { return c == '\0'; }

	static bool IsWhitespaceOrEol_(char c) { return IsWhitespace_(c) || IsEol_(c); }
};

void WhitespaceDelimitedTokensAutomaton::Input(char c)
{
	// ��������� ��������: �������� ������, ������, ����������� �����������, ����� ������
	switch (state_)
	{
		// ���������� ����� ��� ������������ ���� ��� ������ ������
	case State::TokenExpexted:
		if (IsWhitespaceOrEol_(c)) { state_ = State::TokenExpexted; }
		else state_ = State::TokenBegin;
		break;
		// ���������� ����� ��� ������� ������, ��������� ���� ���������� ��������� �����, ���� ����������� ���
	case State::TokenBegin:
		if (IsWhitespaceOrEol_(c)) { state_ = State::TokenEnd; }
		else state_ = State::TokenContinues;
		break;
		// ����� ������������, ���� ������� ������ �� �����������
	case State::TokenContinues:
		if (IsWhitespaceOrEol_(c)) { state_ = State::TokenEnd; }
		else state_ = State::TokenContinues;
		break;
		// ��� ������ �����������, ������ ���� ��� ���� �����������, ���� ������ ������ ������
	case State::TokenEnd:
		if (IsWhitespaceOrEol_(c)) { state_ = State::TokenExpexted; }
		else state_ = State::TokenBegin;
		break;
	default:
		throw std::logic_error("Unexpected state");
		break;
	}
}

//� ���������� ���������� ������� ��������������� ��� ������ �����
// ����� ������ ��������������� �����������
// � ���������� ������ ����� ���� ���������������� ����������� ���� ��������:
// " " "/" *�����/�����*
// ����� �� ��� ������������ ������� ���������� ��������� ����� ������, �� ��������� ������� �� �������� ��� ����������
// TODO: ����������� ������ �������� ������� �������� �������� ������ �����������, ������� ��� ���� ������ ��� ���� ��������
// 
// �������� �� ��������� �������� �����
//


void ParceLine(std::string string)
{
	std::list<std::string> stringTokens;
	std::map<std::string, std::string> params;

	// ��� �������, ��� �������� ������ �� �����, ���� �� ��������� �������
	WhitespaceDelimitedTokensAutomaton automaton;

	// ������ ����� ��� �����������
	std::string token;
	std::string prev;
	std::string current;
	std::string name;
	std::string value;
	bool lastWasName{false};

	for (size_t i = 0; i <= string.size(); i++)
	{
		auto c = string[i];
		automaton.Input(c);

		if (automaton.TokenBegin())
			token.clear();

		if (automaton.TokenBeingObserved())
			token.push_back(c);

		if (automaton.TokenEnd())
		{
			std::cout << token;

			if (lastWasName)
			{
				if (*token.begin() == '/')
					std::cout << "Two params name in a row WTF\n";
				else
				{
					std::cout << "  <- token is parameter value!\n";
					lastWasName = 0;
					params[prev] = token;
				}
			}
			else
				if (*token.begin() == '/')
				{
					std::cout << "  <- token is parameter name!\n";
					lastWasName = 1;
					params[token];
				}
				else
					std::cout << "  <- value without name!!!!\n";
			prev = token;
		}
			
	}
	for (auto i = params.cbegin(); i != params.cend(); i++)
	{
		std::cout << "Name: " << i->first << "\t Value: " << i->second << std::endl;
	}
}


int main(int argc, char* argv[])
{
	if (argc < 2) return -1;

	std::string string{ argv[1] };

	ParceLine(string);

	return 0;
}