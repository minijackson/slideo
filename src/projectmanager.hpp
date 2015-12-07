#pragma once

#include <QObject>

#include <set>
#include <vector>
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
	ProjectManager(ProjectManager const& other);

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
	 * \param projectFile the project file path.
	 */
	explicit ProjectManager(std::string projectFile);

	/*! \brief ProjectManager constructor specifying the video file path.
	 *
	 * Creates the project file with a default template.
	 *
	 * \param projectFile the project file path.
	 * \param videoFile the path of the video file.
	 */
	ProjectManager(std::string projectFile, std::string videoFile);

	/*! \brief projectFile attribute getter.
	 */
	std::string const& getProjectFile() const;

	/*! \brief saved attribute getter.
	 */
	bool const& isSaved() const;

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
	void addBreakpoint(qint64 const breakpoint);

	/*! \brief Add several breakpoints to the project.
	 *
	 * The breakpoints must be in msecs.
	 *
	 * \param breakpoints the breakpoints to add.
	 */
	void addBreakpoints(std::vector<qint64> const& breakpoints);

	/*! \brief Remove a breakpoint from the project.
	 *
	 * The breakpoint must be in msecs.
	 *
	 * \param breakpoint the breakpoint to be removed.
	 */
	void removeBreakpoint(qint64 const breakpoint);

	/*! \brief Remove several breakpoint from the project.
	 *
	 * The breakpoints must be in msecs.
	 *
	 * \param breakpoints the breakpoints to be removed.
	 */
	void removeBreakpoints(std::vector<qint64> const& breakpoints);

	/*! \brief Replace a breakpoint by an other.
	 *
	 * The breakpoints must be in msecs. This is mainly used when the user
	 * change a value directly on the dock of the main window.
	 *
	 * \param oldPosition the position of the breakpoint to be replaced.
	 * \param newPosition the new position of the breakpoint.
	 */
	void replaceBreakpoint(qint64 const oldPosition, qint64 const newPosition);

public slots:
	/*! \brief Saves the project to the project file.
	 */
	void saveProject();

signals:
	/*! \brief Signal emitted when the project's breakpoints are changed.
	 */
	void breakpointsChanged() const;

protected:
	std::string projectFile;
	bool saved = true;
	YAML::Node project;
	std::set<qint64> breakpoints;

	// Needed to modify the "saved" state
	friend class History;
};
