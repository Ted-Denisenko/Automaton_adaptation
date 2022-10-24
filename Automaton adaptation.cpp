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
	// проверка на разделитель
	// в моем случае стоит сделать аналогичную проверку на признак нового параметра
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
	// состояния автомата: ожидание токена, начало, продолжение начавшегося, конец токена
	switch (state_)
	{
		// предыдущий токен был разделителем либо это начало строки
	case State::TokenExpexted:
		if (IsWhitespaceOrEol_(c)) { state_ = State::TokenExpexted; }
		else state_ = State::TokenBegin;
		break;
		// предудыщий токен был началом токена, следующий либо продолжает имеющийся токен, либо заканчивает его
	case State::TokenBegin:
		if (IsWhitespaceOrEol_(c)) { state_ = State::TokenEnd; }
		else state_ = State::TokenContinues;
		break;
		// токен продолжается, если текущий символ не разделитель
	case State::TokenContinues:
		if (IsWhitespaceOrEol_(c)) { state_ = State::TokenEnd; }
		else state_ = State::TokenContinues;
		break;
		// был найден разделитель, дальше либо еще один разделитель, либо начало нового токена
	case State::TokenEnd:
		if (IsWhitespaceOrEol_(c)) { state_ = State::TokenExpexted; }
		else state_ = State::TokenBegin;
		break;
	default:
		throw std::logic_error("Unexpected state");
		break;
	}
}

//в предыдущем исполнении парсера рассматривалась вся строка сразу
// здесь строка рассматривается посимвольно
// в предыдущей версии новая пара характризовалась комбинацией трех символов:
// " " "/" *буква/цифра*
// здесь же при рассмотрении пробела поставится состояние конца токена, на следующем символе он сотрется для перезаписи
// TODO: изначальная версия автомата считала условием окнчания токена разделитель, поэтому для моей задачи его надо поменять
// 
// проверка на поддержку русского языка
//


void ParceLine(std::string string)
{
	std::list<std::string> stringTokens;
	std::map<std::string, std::string> params;

	// сам автомат, как работает внутри не знаем, пока не посмотрим начинку
	WhitespaceDelimitedTokensAutomaton automaton;

	// хранит токен без разделителя
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