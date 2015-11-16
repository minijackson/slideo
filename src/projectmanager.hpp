#pragma once

#include <QObject>

#include <set>
#include <yaml-cpp/yaml.h>

/*! \brief Class used to handle a project
 *
 * Handles the project file, the yaml and the breakpoints.
 */
class ProjectManager : public QObject {

	Q_OBJECT

public:
	/*! \brief ProjectManager default constructor.
	 *
	 * This will construct a dummy project, for when no project is loaded yet.
	 */
	ProjectManager() = default;

	/*! \brief ProjectManager copy constructor.
	 */
	explicit ProjectManager(ProjectManager const& other);

	/*! \brief ProjectManager move constructor.
	 */
	ProjectManager(ProjectManager&& other) noexcept;

	/*! \brief ProjectManager copy assignment operator.
	 */
	ProjectManager& operator=(ProjectManager const& other) noexcept;

	/*! \brief ProjectManager move assignment operator.
	 */
	ProjectManager& operator=(ProjectManager&& other) noexcept;

	/*! \brief ProjectManager constructor.
	 *
	 * Loads the project file into Yaml node.
	 *
	 * \param projectFile the project file path
	 */
	explicit ProjectManager(std::string projectFile);

	/*! \brief projectFile attribute getter.
	 */
	std::string const& getProjectFile() const;

	/*! \brief isSaved attribute getter.
	 */
	bool const& isItSaved() const;

	/*! \brief project attribute getter.
	 */
	YAML::Node const& getProjectNode() const;

	/*! \brief Get the path of the video file for the project.
	 *
	 * \return the video file for this project.
	 */
	std::string getVideoFile() const;

	/*! \brief Get the path leading to the project file
	 *
	 * \return the path to the project file.
	 */
	std::string getProjectFileLocation() const;

	/*! \brief Get the name of the project file.
	 *
	 * \return the name of the project file.
	 */
	std::string getProjectFileBaseName() const;

	/*! \brief Get the breakpoints for this project.
	 *
	 * \return the breakpoints of this project.
	 */
	std::set<qint64> const& getBreakpoints() const;

	/*! \brief Set the breakpoints for this project.
	 *
	 * \param breakpoints Breakpoints to set as the project's breakpoints.
	 */
	void setBreakpoints(std::set<qint64> const& breakpoints);

	/*! \brief Set the breakpoints for this project.
	 *
	 * \param breakpoints Breakpoints to set as the project's breakpoints.
	 */
	void setBreakpoints(std::set<qint64>&& breakpoints);

	/*! \brief Add a breakpoint to the project.
	 *
	 * The breakpoint must be in msecs.
	 *
	 * \param breakpoint the breakpoint to add.
	 */
	void addBreakpoint(qint64 breakpoint);

	/*! \brief Remove a breakpoint to the project;
	 *
	 * The breakpoint must be in msecs.
	 *
	 * \param breakpoint the breakpoint to be removed.
	 */
	void removeBreakpoint(qint64 breakpoint);

	/*! \brief Replace a breakpoint by an other.
	 *
	 * The breakpoints must be in msecs. This is mainly used when the user
	 * change a value directly on the dock of the main window.
	 *
	 * \param oldPosition the position of the breakpoint to be replaced.
	 * \param newPosition the new position of the breakpoint.
	 */
	void replaceBreakpoint(qint64 oldPosition, qint64 newPosition);

public slots:
	/*! \brief Sets the "saved" status to false.
	 */
	void changeProjectSavedStatus();

	/*! \brief Saves the project to the project file.
	 */
	void saveProject();

signals:
	/*! \brief Signal emitted when the project's breakpoints are changed.
	 */
	void breakpointsChanged() const;

protected:
	std::string projectFile;
	bool isSaved = false;
	YAML::Node project;
	std::set<qint64> breakpoints;
private:
};
