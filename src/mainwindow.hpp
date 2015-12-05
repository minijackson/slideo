#pragma once

#include "projectmanager.hpp"
#include "videoplayermanager.hpp"
#include "history.hpp"
#include "doubleclickablelabel.hpp"

#include <QMainWindow>

#include <QAction>

#include <QPushButton>
#include <QSlider>
#include <QLabel>

#include <QListView>
#include <QStringListModel>

/*! \brief Main window of slideo.
 */
class MainWindow : public QMainWindow {

	Q_OBJECT

public:
	MainWindow();

	/*! \brief Initialize the central zone
	 *
	 * This includes the video player, the video player UI (the seek bar, the
	 * play/pause button, etc…)
	 */
	void initCentralZone();

	/*! \brief Initialize the QActions and widgets providing QActions
	 *
	 * This includes the menus, the dock and the toolbar
	 */
	void initActionWidgets();

	/*! \brief Get the current project.
	 *
	 * \return the current project.
	 */
	ProjectManager const& getProject() const;

	/*! \brief Add a breakpoint to the current project.
	 *
	 * \param position the position of the breakpoint.
	 */
	void addProjectBreakpoint(qint64 position);

	/*! \brief Get the video player manager.
	 *
	 * \return the video player manager.
	 */
	VideoPlayerManager const& getVideoPlayer() const;

	/*! \brief Set the position for the current video.
	 *
	 * \param position the position to set.
	 */
	void setVideoPlayerPosition(qint64 position);

public slots:

	/*! \brief Create a new project.
	 *
	 * This will open a dialog for selecting the project location and the video location.
	 */
	void newProject();

	/*! \brief Open a project.
	 *
	 * This will open a dialog for selecting a project file and load it.
	 */
	void openProject();

	/*! \brief Save the current project.
	 *
	 * Will also show the message "Project saved." for 5 secs on the status bar.
	 */
	void saveProject();

	/*! \brief Start the slideshow in fullscreen mode.
	 */
	void startSlideshow();

	/*! \brief Start the slideshow in fullscreen mode at the current position.
	 */
	void startSlideshowFromHere();

	/*! \brief Connect project-dependant signals/slots.
	 *
	 * This includes the update of the dock when the project's breakpoints are
	 * changed and the window title update when the project's breakpoints are
	 * changed.
	 */
	void projectConnections();

	/*! \brief Update the title of the window.
	 *
	 * The window title must be updated when, for example, a project is loaded
	 * or there is a change in the project
	 */
	void updateWindowTitle();

	/*! \brief Update the breakpoints of the project.
	 *
	 * Called when the user change a value directly on the dock of the main window.
	 *
	 * \param index the index of the item modified on the dock list.
	 */
	void updateProjectBreakpoints(QModelIndex const& index);

	/*! \brief Update the dock breakpoints.
	 *
	 * Called when the user change a value directly on the dock of the main window.
	 */
	void updateDockBreakpoints();

	/*! \brief Save the current state in the history.
	 *
	 * Will be called when the breakpoints or the video file path changed.
	 */
	void saveState();

	/*! \brief Undo the last action.
	 *
	 * Restore the previous state if existing.
	 */
	void undo();

	/*! \brief Redo the last action.
	 *
	 * Restore the next state if existing.
	 */
	void redo();

	/*! \brief Show the dock's contextual menu.
	 *
	 * Called when the user right-clicked on the dock.
	 *
	 * \param relativePos the relative coordinates of the contextual menu.
	 */
	void showDockContextMenu(QPoint const& relativePos);

	/*! \brief Change the play/pause button icon according to the player's state.
	 *
	 * \param state the state of the player.
	 */
	void updatePlayPauseButtonIcon(QMediaPlayer::State state);

	/*! \brief Update the range of the slider according to the video duration.
	 *
	 * \param duration the duration of the current video.
	 */
	void updateSliderRange(qint64 duration);

	/*! \brief Update the position of the slider according to the current position
	 * in the video.
	 *
	 * \param position the position in the current video.
	 */
	void updateSliderPosition(qint64 position);

	/*! \brief Update the range of the range viewer according to the video duration.
	 *
	 * \param duration the duration of the current video.
	 */
	void updateDurationViewer(qint64 duration);

	/*! \brief Update the position of the position viewer according to the current
	 * position in the video.
	 *
	 * \param position the position in the current video.
	 */
	void updatePositionViewer(qint64 position);

	/*! \brief Show the "Add Breakpoint" dialog.
	 *
	 * Upon successful completion, it will add the specified breakpoint.
	 */
	void showAddBreakpointDialog();

	/*! \brief Show the "Jump to time" dialog.
	 *
	 * Upon successful completion, it will jump to the specified time.
	 */
	void showJumpToTimeDialog();

	/*! \brief Add a breakpoint at current position.
	 */
	void addBreakpointHere();

	/*! \brief Remove the selected breakpoints in the dock.
	 *
	 */
	void removeDockBreakpoints();

	/*! \brief Alternate between fullscreen and non-fullscreen.
	 *
	 * \param value true will make the window fullscreen, false will do the
	 *        opposite.
	 */
	void alternateFullscreen(bool value);

signals:
	/*! \brief Signal triggered when a project is loaded
	 *
	 * This will update the window title, enable the project-related QActions
	 * and make some connections.
	 *
	 * \param _t1 true to enable the QActions using setEnabled
	 */
	void projectActivated(bool);

protected:
	ProjectManager project;
	VideoPlayerManager videoPlayer;
	History history;

	QAction undoAction;
	QAction redoAction;
	QAction addBreakpointAction;
	QAction addBreakpointHereAction;
	QAction removeBreakpointAction;

	QPushButton playerPlayPauseButton;
	QSlider playerSeekBar;

	DoubleClickableLabel playerPositionViewer;
	DoubleClickableLabel playerDurationViewer;

	QListView breakpointListView;
	QStringListModel breakpointListModel;
private:
};
