#include "history.hpp"

History::History(ProjectManager& firstState)
      : states{std::move(firstState)}
      , currentState(states.begin()) {}

ProjectManager History::getCurrentState() const {
	return *currentState;
}

ProjectManager History::goBack() {
	if(currentState != states.begin()) {
		--currentState;
	}
	return *currentState;
}

ProjectManager History::advance() {
	if(currentState != --states.end()) {
		++currentState;
	}
	return *currentState;
}

void History::push_back(ProjectManager const& state) {
	if(currentState != --states.end()) {
		size_t newSize = std::distance(states.begin(), currentState) + 1;
		states.resize(newSize);
	}

	states.push_back(state);
	currentState = --states.end();
}
