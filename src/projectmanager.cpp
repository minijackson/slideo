#include "projectmanager.hpp"

#include <QFileInfo>

#include <fstream>

// Conversion of std::set<qint64> to/from YAML::Node
namespace YAML {
	template<>
	struct convert<std::set<qint64>> {
		static Node encode(const std::set<qint64>& rhs) {
			Node node;
			if(rhs.empty()) {
				node = YAML::Load("[]");
			} else {
				for(qint64 i : rhs) {
					node.push_back(i);
				}
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
      , breakpoints(project["breakpoints"].as<std::set<qint64>>()) {}

ProjectManager::ProjectManager(std::string projectFile, std::string videoFile)
      : QObject()
      , projectFile(projectFile)
      , project()
      , breakpoints() {

	project["breakpoints"] = YAML::Load("[]");
	project["video-file"] = videoFile;

	saveProject();
}

ProjectManager::ProjectManager(ProjectManager const& other)
      : QObject()
      , projectFile(other.getProjectFile())
      , saved(other.isSaved())
      , project(other.getProjectNode())
      , breakpoints(other.getBreakpoints()) {}

ProjectManager::ProjectManager(ProjectManager&& other) noexcept
      : QObject()
      , projectFile(std::move(other.getProjectFile()))
      , saved(std::move(other.isSaved()))
      , project(std::move(other.getProjectNode()))
      , breakpoints(std::move(other.getBreakpoints())) {}

ProjectManager& ProjectManager::operator=(ProjectManager const& other) noexcept {
	if(&other != this) {
		projectFile = std::string(other.getProjectFile());
		saved = other.isSaved();
		project = other.getProjectNode();
		breakpoints = other.getBreakpoints();
	}
	return *this;
}

ProjectManager& ProjectManager::operator=(ProjectManager&& other) noexcept {
	if(&other != this) {
		projectFile = std::move(other.getProjectFile());
		saved = std::move(other.isSaved());
		project = std::move(other.getProjectNode());
		breakpoints = std::move(other.getBreakpoints());
	}
	return *this;
}

std::string const& ProjectManager::getProjectFile() const {
	return projectFile;
}

bool const& ProjectManager::isSaved() const {
	return saved;
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

void ProjectManager::setBreakpoints(std::set<qint64> const& breakpoints) {
	if(this->breakpoints != breakpoints) {
		this->breakpoints = breakpoints;
		saved = false;
		emit breakpointsChanged();
	}
}

void ProjectManager::setBreakpoints(std::set<qint64>&& breakpoints) {
	if(this->breakpoints != breakpoints) {
		this->breakpoints = breakpoints;
		saved = false;
		emit breakpointsChanged();
	}
}

void ProjectManager::addBreakpoint(qint64 const breakpoint) {
	breakpoints.insert(breakpoint);
	saved = false;
	emit breakpointsChanged();
}

void ProjectManager::removeBreakpoint(qint64 const breakpoint) {
	breakpoints.erase(breakpoint);
	saved = false;
	emit breakpointsChanged();
}

void ProjectManager::replaceBreakpoint(qint64 const oldPosition, qint64 const newPosition) {
	if(oldPosition != newPosition) {
		removeBreakpoint(oldPosition);
		addBreakpoint(newPosition);
		saved = false;
		emit breakpointsChanged();
	}
}

void ProjectManager::saveProject() {
	project["breakpoints"] = breakpoints;

	std::ofstream fileStream(projectFile);
	fileStream << project << std::endl;
	saved = true;
}
