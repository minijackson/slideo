#include "history.hpp"

History::History(ProjectManager& firstState)
      : states{std::move(firstState)}
      , currentState(states.begin())
      , savedState(firstState.isSaved() ? states.begin() : states.end()) {}

ProjectManager History::getCurrentState() const {
	if(currentState == savedState) {
		currentState->saved = true;
	} else {
		currentState->saved = false;
	}
	return *currentState;
}

ProjectManager History::goBack() {
	if(currentState != states.begin()) {
		--currentState;
	}
	return getCurrentState();
}

ProjectManager History::advance() {
	if(currentState != --states.end()) {
		++currentState;
	}
	return getCurrentState();
}

void History::push_back(ProjectManager const& state) {
	if(currentState != --states.end()) {
		size_t newSize = std::distance(states.begin(), currentState) + 1;
		states.resize(newSize);
	}

	states.push_back(state);
	currentState = --states.end();
}

void History::setSaved() {
	savedState = currentState;
}
