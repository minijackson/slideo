#pragma once

#include "projectmanager.hpp"

#include <list>

/*! \brief Class used to undo/redo in the project.
 *
 * This store a list of ProjectManager and alternate between nodes on
 * undo/redo.
 */
class History {
public:
	/*! \brief History main constructor.
	 *
	 * Will construct a history with one state.
	 */
	History(ProjectManager& firstState);

	/*! \brief History default constructor.
	 *
	 * Will construct a dummy history.
	 */
	History() = default;

	/*! \brief Get the current state of the project.
	 *
	 * This might NOT return the last state in the list
	 * (ex: if the user has undo).
	 *
	 * \return a copy of the current state.
	 */
	ProjectManager getCurrentState() const;

	/*! \brief Go back in the history by one.
	 *
	 * \return a copy of the current state after going back.
	 */
	ProjectManager goBack();

	/*! \brief Advance in the history by one.
	 *
	 * \return a copy of the current state after advancing.
	 */
	ProjectManager advance();

	/*! \brief Add a new state at the next position.
	 *
	 * \param state the state to add.
	 */
	void push_back(ProjectManager const& state);

	/*! \brief Set the current state as "saved".
	 */
	void setSaved();
protected:
	std::list<ProjectManager> states;
	std::list<ProjectManager>::iterator currentState;
	std::list<ProjectManager>::iterator savedState;
};
