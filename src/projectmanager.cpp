#include "projectmanager.hpp"

#include <QFileInfo>

#include <fstream>

// Conversion of std::set<qint64> to/from YAML::Node
namespace YAML {
	template<>
	struct convert<std::set<qint64>> {
		static Node encode(const std::set<qint64>& rhs) {
			Node node;
			for(qint64 i : rhs) {
				node.push_back(i);
			}
			return node;
		}

		static bool decode(const Node& node, std::set<qint64>& rhs) {
			if(!node.IsSequence()) {
				return false;
			}
			std::size_t size = node.size();
			for(std::size_t i = 0 ; i < size ; ++i) {
				rhs.insert(node[i].as<qint64>());
			}
			return true;
		}
	};
}

ProjectManager::ProjectManager(std::string projectFile)
      : QObject()
      , projectFile(projectFile)
      , project(YAML::LoadFile(projectFile))
      , breakpoints(project["breakpoints"].as<std::set<qint64>>()) {
	connect(this, SIGNAL(breakpointsChanged()), this, SLOT(changeProjectSavedStatus()));
}

ProjectManager::ProjectManager(ProjectManager const& other)
      : QObject()
      , projectFile(other.getProjectFile())
      , isSaved(other.isItSaved())
      , project(other.getProjectNode())
      , breakpoints(other.getBreakpoints()) {
	connect(this, SIGNAL(breakpointsChanged()), this, SLOT(changeProjectSavedStatus()));
}

ProjectManager::ProjectManager(ProjectManager&& other)
      : QObject()
      , projectFile(std::move(other.getProjectFile()))
      , isSaved(std::move(other.isItSaved()))
      , project(std::move(other.getProjectNode()))
      , breakpoints(std::move(other.getBreakpoints())) {
	connect(this, SIGNAL(breakpointsChanged()), this, SLOT(changeProjectSavedStatus()));
}

ProjectManager& ProjectManager::operator=(ProjectManager const& other) {
	if(&other != this) {
		projectFile = std::string(other.getProjectFile());
		isSaved = other.isItSaved();
		project = other.getProjectNode();
		breakpoints = other.getBreakpoints();
		emit breakpointsChanged();
	}
	return *this;
}

ProjectManager& ProjectManager::operator=(ProjectManager&& other) {
	if(&other != this) {
		projectFile = std::move(other.getProjectFile());
		isSaved = std::move(other.isItSaved());
		project = std::move(other.getProjectNode());
		breakpoints = std::move(other.getBreakpoints());
		emit breakpointsChanged();
	}
	return *this;
}

std::string const& ProjectManager::getProjectFile() const {
	return projectFile;
}

bool const& ProjectManager::isItSaved() const {
	return isSaved;
}

YAML::Node const& ProjectManager::getProjectNode() const {
	return project;
}

std::string ProjectManager::getVideoFile() const {
	return project["video-file"].as<std::string>();
}

std::string ProjectManager::getProjectFileLocation() const {
	return QFileInfo(QString::fromStdString(projectFile)).absolutePath().toStdString();
}

std::string ProjectManager::getProjectFileBaseName() const {
	return QFileInfo(QString::fromStdString(projectFile)).baseName().toStdString();
}

std::set<qint64> const& ProjectManager::getBreakpoints() const {
	return breakpoints;
}

void ProjectManager::changeProjectSavedStatus() {
	isSaved = false;
}

void ProjectManager::setBreakpoints(std::set<qint64> const& breakpoints) {
	this->breakpoints = breakpoints;
	emit breakpointsChanged();
}

void ProjectManager::setBreakpoints(std::set<qint64>&& breakpoints) {
	this->breakpoints = breakpoints;
	emit breakpointsChanged();
}

void ProjectManager::addBreakpoint(qint64 breakpoint) {
	breakpoints.insert(breakpoint);
	emit breakpointsChanged();
}

void ProjectManager::removeBreakpoint(qint64 breakpoint) {
	breakpoints.erase(breakpoint);
	emit breakpointsChanged();
}

void ProjectManager::replaceBreakpoint(qint64 oldPosition, qint64 newPosition) {
	removeBreakpoint(oldPosition);
	addBreakpoint(newPosition);
	emit breakpointsChanged();
}

void ProjectManager::saveProject() {
	project["breakpoints"] = breakpoints;

	std::ofstream fileStream(projectFile);
	fileStream << project << std::endl;
	isSaved = true;
}
