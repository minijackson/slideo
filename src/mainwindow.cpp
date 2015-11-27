#include "mainwindow.hpp"

#include "timeselectdialog.hpp"

#include <QApplication>

#include <QFileDialog>
#include <QMessageBox>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>

#include <QTime>

#include <iterator>

MainWindow::MainWindow()
      : QMainWindow(0)
      , videoPlayer(*this)
      , addBreakpointAction(QIcon::fromTheme("list-add"), "&Add breakpoint", this)
      , addBreakpointHereAction("Add breakpoint at &current position", this)
      , removeBreakpointAction(QIcon::fromTheme("list-remove"), "&Remove selected breakpoint(s)",
                               this)
      , playerPlayPauseButton(QIcon::fromTheme("media-playback-start"), "")
      , playerSeekBar(Qt::Horizontal)
      , playerPositionViewer("00:00:00")
      , playerDurationViewer("00:00:00.000")
      , breakpointListView()
      , breakpointListModel() {
	initCentralZone();
	initActionWidgets();

	updateWindowTitle();
	connect(this, SIGNAL(projectActivated(bool)), this, SLOT(updateWindowTitle()));

	statusBar()->showMessage("");
	resize(800, 600);
}

void MainWindow::initCentralZone() {
	QWidget* centralZone = new QWidget;
	QVBoxLayout* centralZoneLayout = new QVBoxLayout;

	/*======================*/
	/*== Video Player {{{ ==*/
	/*======================*/

	centralZoneLayout->addWidget(&videoPlayer);

	connect(&videoPlayer.getPlayer(), SIGNAL(stateChanged(QMediaPlayer::State)), this,
	        SLOT(updatePlayPauseButtonIcon(QMediaPlayer::State)));
	connect(&videoPlayer.getPlayer(), SIGNAL(durationChanged(qint64)), this,
	        SLOT(updateSliderRange(qint64)));
	connect(&videoPlayer.getPlayer(), SIGNAL(durationChanged(qint64)), this,
	        SLOT(updateDurationViewer(qint64)));
	connect(&videoPlayer.getPlayer(), SIGNAL(positionChanged(qint64)), this,
	        SLOT(updateSliderPosition(qint64)));
	connect(&videoPlayer.getPlayer(), SIGNAL(positionChanged(qint64)), this,
	        SLOT(updatePositionViewer(qint64)));

	// }}}

	/*=========================*/
	/*== Video Player UI {{{ ==*/
	/*=========================*/

	QHBoxLayout* playerUILayout = new QHBoxLayout;


	QPushButton* playerSeekBackwardButton =
	  new QPushButton(QIcon::fromTheme("media-seek-backward"), "");
	playerSeekBackwardButton->setEnabled(false);
	playerUILayout->addWidget(playerSeekBackwardButton);
	connect(playerSeekBackwardButton, SIGNAL(clicked()), &videoPlayer, SLOT(seekBackward()));

	playerPlayPauseButton.setEnabled(false);
	playerUILayout->addWidget(&playerPlayPauseButton);
	connect(&playerPlayPauseButton, SIGNAL(clicked()), &videoPlayer, SLOT(playPause()));

	QPushButton* playerSeekForwardButton =
	  new QPushButton(QIcon::fromTheme("media-seek-forward"), "");
	playerSeekForwardButton->setEnabled(false);
	playerUILayout->addWidget(playerSeekForwardButton);
	connect(playerSeekForwardButton, SIGNAL(clicked()), &videoPlayer, SLOT(seekForward()));

	playerSeekBar.setRange(0, 0);
	playerSeekBar.setEnabled(false);
	connect(&playerSeekBar, SIGNAL(sliderMoved(int)), &videoPlayer, SLOT(setPosition(int)));
	connect(&playerSeekBar, SIGNAL(sliderPressed()), &videoPlayer, SLOT(pause()));
	connect(&playerSeekBar, SIGNAL(sliderReleased()), &videoPlayer, SLOT(resetBreakpointsIterators()));
	playerUILayout->addWidget(&playerSeekBar);

	QWidget* playerTimeViewerWidget = new QWidget;
	QHBoxLayout* playerTimeViewerLayout = new QHBoxLayout;
	playerTimeViewerLayout->setSpacing(0);
	playerTimeViewerLayout->setContentsMargins(0, 0, 0, 0);

	DoubleClickableLabel* playerTimeSeparator = new DoubleClickableLabel(" / ");

	playerPositionViewer.setEnabled(false);
	playerTimeSeparator->setEnabled(false);
	playerDurationViewer.setEnabled(false);

	connect(&playerPositionViewer, SIGNAL(mouseDoubleClicked()), this, SLOT(showJumpToTimeDialog()));
	connect(playerTimeSeparator, SIGNAL(mouseDoubleClicked()), this, SLOT(showJumpToTimeDialog()));
	connect(&playerDurationViewer, SIGNAL(mouseDoubleClicked()), this, SLOT(showJumpToTimeDialog()));

	playerTimeViewerLayout->addWidget(&playerPositionViewer);
	playerTimeViewerLayout->addWidget(playerTimeSeparator);
	playerTimeViewerLayout->addWidget(&playerDurationViewer);

	playerTimeViewerWidget->setLayout(playerTimeViewerLayout);
	playerUILayout->addWidget(playerTimeViewerWidget);
	playerUILayout->setContentsMargins(0, 0, 0, 0);

	QWidget* playerUIWidget = new QWidget;
	playerUIWidget->setLayout(playerUILayout);
	centralZoneLayout->addWidget(playerUIWidget);

	// }}}

	/*==================*/
	/*== Timeline {{{ ==*/
	/*==================*/

	// QWidget timeline;
	// centralZone.addWidget(&timeline);

	// }}}

	centralZone->setLayout(centralZoneLayout);
	setCentralWidget(centralZone);

	/*=====================*/
	/*== Connections {{{ ==*/
	/*=====================*/

	// Activations
	connect(this, SIGNAL(projectActivated(bool)), playerSeekBackwardButton, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), &playerPlayPauseButton, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), playerSeekForwardButton, SLOT(setEnabled(bool)));

	connect(this, SIGNAL(projectActivated(bool)), &playerSeekBar, SLOT(setEnabled(bool)));

	connect(this, SIGNAL(projectActivated(bool)), &playerPositionViewer, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), playerTimeSeparator, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), &playerDurationViewer, SLOT(setEnabled(bool)));

	connect(this, SIGNAL(projectActivated(bool)), &videoPlayer, SLOT(activateVideo()));
	connect(this, SIGNAL(projectActivated(bool)), &videoPlayer, SLOT(setFocus()));

	// }}}
}

void MainWindow::initActionWidgets() {

	/*===================*/
	/*== File Menu {{{ ==*/
	/*===================*/

	QMenu& fileMenu = *menuBar()->addMenu("&File");

	QAction* newPresentationAction =
	  new QAction(QIcon::fromTheme("document-new"), "&New presentation", this);
	newPresentationAction->setShortcut(QKeySequence("Ctrl+N"));
	connect(newPresentationAction, SIGNAL(triggered()), this, SLOT(newProject()));
	fileMenu.addAction(newPresentationAction);

	QAction* openPresentationAction =
	  new QAction(QIcon::fromTheme("document-open"), "&Open presentation", this);
	openPresentationAction->setShortcut(QKeySequence("Ctrl+O"));
	connect(openPresentationAction, SIGNAL(triggered()), this, SLOT(openProject()));
	fileMenu.addAction(openPresentationAction);

	fileMenu.addSeparator();

	QAction* saveProjectAction =
	  new QAction(QIcon::fromTheme("document-save"), "&Save presentation", this);
	saveProjectAction->setShortcut(QKeySequence("Ctrl+S"));
	saveProjectAction->setEnabled(false);
	connect(saveProjectAction, SIGNAL(triggered()), this, SLOT(saveProject()));
	fileMenu.addAction(saveProjectAction);

	QAction* saveProjectAsAction =
	  new QAction(QIcon::fromTheme("document-save-as"), "Save presentation &as...", this);
	saveProjectAsAction->setShortcut(QKeySequence("Ctrl+Shift+S"));
	saveProjectAsAction->setEnabled(false);
	fileMenu.addAction(saveProjectAsAction);

	fileMenu.addSeparator();

	QAction* quitAction = new QAction(QIcon::fromTheme("application-exit"), "E&xit", this);
	quitAction->setShortcut(QKeySequence("Ctrl+Q"));
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
	fileMenu.addAction(quitAction);

	// }}}

	/*=================*/
	/*== Toolbar {{{ ==*/
	/*=================*/

	QAction* startSlideshowAction = new QAction(QIcon::fromTheme("media-playback-start"), "&Start slideshow", this);
	startSlideshowAction->setShortcut(QKeySequence("F5"));
	startSlideshowAction->setEnabled(false);

	QToolBar& toolbar = *addToolBar("Main Toolbar");
	toolbar.addAction(newPresentationAction);
	toolbar.addAction(openPresentationAction);
	toolbar.addSeparator();
	toolbar.addAction(startSlideshowAction);

	// }}}

	/*==============*/
	/*== Dock {{{ ==*/
	/*==============*/

	breakpointListView.setEnabled(false);
	breakpointListView.setSelectionMode(QListView::ExtendedSelection);
	breakpointListView.setModel(&breakpointListModel);

	connect(&breakpointListModel,
	        SIGNAL(dataChanged(QModelIndex const&, QModelIndex const&, QVector<int> const&)), this,
	        SLOT(updateProjectBreakpoints(QModelIndex const&)));

	QDockWidget* breakpointsDock = new QDockWidget("Breakpoints", this);
	breakpointsDock->setWidget(&breakpointListView);
	breakpointsDock->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(breakpointsDock, SIGNAL(customContextMenuRequested(QPoint const&)), this,
	        SLOT(showDockContextMenu(QPoint const&)));

	addDockWidget(Qt::LeftDockWidgetArea, breakpointsDock);

	// }}}

	/*===================*/
	/*== Edit Menu {{{ ==*/
	/*===================*/

	QMenu& editMenu = *menuBar()->addMenu("&Edit");

	addBreakpointAction.setShortcut(QKeySequence("Ctrl+B"));
	addBreakpointAction.setEnabled(false);
	connect(&addBreakpointAction, SIGNAL(triggered()), this, SLOT(showAddBreakpointDialog()));
	editMenu.addAction(&addBreakpointAction);

	addBreakpointHereAction.setShortcut(QKeySequence("Ctrl+Shift+B"));
	addBreakpointHereAction.setEnabled(false);
	connect(&addBreakpointHereAction, SIGNAL(triggered()), this, SLOT(addBreakpointHere()));
	editMenu.addAction(&addBreakpointHereAction);

	removeBreakpointAction.setShortcut(QKeySequence("Ctrl+D"));
	removeBreakpointAction.setEnabled(false);
	connect(&removeBreakpointAction, SIGNAL(triggered()), this, SLOT(removeDockBreakpoints()));
	editMenu.addAction(&removeBreakpointAction);

	// }}}

	/*===================*/
	/*== View Menu {{{ ==*/
	/*===================*/

	QMenu& viewMenu = *menuBar()->addMenu("&View");

	QAction* fullscreenAction = new QAction("&Fullscreen", this);
	fullscreenAction->setCheckable(true);
	fullscreenAction->setShortcut(QKeySequence("F11"));
	connect(fullscreenAction, SIGNAL(toggled(bool)), this, SLOT(alternateFullscreen(bool)));
	viewMenu.addAction(fullscreenAction);

	viewMenu.addSeparator();

	viewMenu.addAction(startSlideshowAction);

	connect(startSlideshowAction, SIGNAL(triggered()), this, SLOT(startSlideshow()));

	QAction* startFromHereAction = new QAction("&Start slideshow from here", this);
	startFromHereAction->setShortcut(QKeySequence("Shift+F5"));
	startFromHereAction->setEnabled(false);
	viewMenu.addAction(startFromHereAction);

	connect(startFromHereAction, SIGNAL(triggered()), this, SLOT(startSlideshowFromHere()));

	viewMenu.addSeparator();

	QAction* jumpToTimeAction =
	  new QAction(QIcon::fromTheme("go-jump"), "&Jump to specific time", this);
	jumpToTimeAction->setShortcut(QKeySequence("Ctrl+T"));
	jumpToTimeAction->setEnabled(false);
	connect(jumpToTimeAction, SIGNAL(triggered()), this, SLOT(showJumpToTimeDialog()));
	viewMenu.addAction(jumpToTimeAction);

	viewMenu.addSeparator();

	viewMenu.addAction(toolbar.toggleViewAction());
	viewMenu.addAction(breakpointsDock->toggleViewAction());

	// }}}

	/*=====================*/
	/*== Connections {{{ ==*/
	/*=====================*/

	connect(this, SIGNAL(projectActivated(bool)), saveProjectAction, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), saveProjectAsAction, SLOT(setEnabled(bool)));

	connect(this, SIGNAL(projectActivated(bool)), &addBreakpointAction, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), &addBreakpointHereAction, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), &removeBreakpointAction, SLOT(setEnabled(bool)));

	connect(this, SIGNAL(projectActivated(bool)), startSlideshowAction, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), startFromHereAction, SLOT(setEnabled(bool)));

	connect(this, SIGNAL(projectActivated(bool)), jumpToTimeAction, SLOT(setEnabled(bool)));

	connect(this, SIGNAL(projectActivated(bool)), &breakpointListView, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(projectActivated(bool)), this, SLOT(projectConnections()));

	// }}}
}

ProjectManager const& MainWindow::getProject() const {
	return project;
}

void MainWindow::addProjectBreakpoint(qint64 position) {
	project.addBreakpoint(position);
}

VideoPlayerManager const& MainWindow::getVideoPlayer() const {
	return videoPlayer;
}

void MainWindow::setVideoPlayerPosition(qint64 position) {
	videoPlayer.setPosition(position);
}

void MainWindow::newProject() {
	QString projectFile =
	  QFileDialog::getSaveFileName(this, "Save project as...", QDir::homePath(), "Slideo project file (*.eo)");

	QDir projectDir =  QFileInfo(projectFile).absoluteDir();

	if(projectFile != "") {
		if(!projectFile.endsWith(".eo")) {
			projectFile += ".eo";
		}

		QString videoFile =
		  QFileDialog::getOpenFileName(this, "Select the video file", projectDir.path());

		if(videoFile != "") {
			QString videoFileRelativePath = projectDir.relativeFilePath(videoFile);

			if(videoFileRelativePath.startsWith("..")) {
				QMessageBox::warning(this, "Video file not relative to the project file",
				                     "The video file is not in a sub-directory of the project "
				                     "file's directory.\nIt is likely that if you move the "
				                     "project's directory, you will need to re-specify the video "
				                     "file's location.");
			}

			project =
			  ProjectManager(projectFile.toStdString(), videoFileRelativePath.toStdString());
			emit projectActivated(true);
			updateDockBreakpoints();
		}
	}
}

void MainWindow::openProject() {
	QString projectFile = QFileDialog::getOpenFileName(this, "Open project", QDir::homePath(),
	                                                   "Slideo project file (*.eo)");
	if(projectFile != "") {
		project = ProjectManager(projectFile.toStdString());
		emit projectActivated(true);
		updateDockBreakpoints();
	}
}

void MainWindow::saveProject() {
	project.saveProject();
	statusBar()->showMessage("Project saved.", 5'000);
	updateWindowTitle();
}

void MainWindow::startSlideshow() {
	VideoPlayerManager* fullScreenPlayer =
	  new VideoPlayerManager(*this, /* position = */ 0, /* presentationMode = */ true);
	fullScreenPlayer->activateVideo();
}

void MainWindow::startSlideshowFromHere() {
	VideoPlayerManager* fullScreenPlayer =
	  new VideoPlayerManager(*this, videoPlayer.getPosition(), /* presentationMode = */ true);
	fullScreenPlayer->activateVideo();
}

void MainWindow::projectConnections() {
	connect(&project, SIGNAL(breakpointsChanged()), this, SLOT(updateDockBreakpoints()));
	connect(&project, SIGNAL(breakpointsChanged()), this, SLOT(updateWindowTitle()));
}

void MainWindow::updateWindowTitle() {
	setWindowTitle(project.isSaved() ? "Slideo" : "*Slideo");
}

void MainWindow::updateProjectBreakpoints(QModelIndex const& index) {
	auto oldPositionIt = project.getBreakpoints().cbegin();
	std::advance(oldPositionIt, index.row());
	qint64 oldPosition = *oldPositionIt,
	       newPosition = QTime(0, 0, 0, 0).msecsTo(QTime::fromString(
	         breakpointListModel.data(index, Qt::DisplayRole).toString(), "HH:mm:ss.zzz"));
	project.replaceBreakpoint(oldPosition, newPosition);
}

void MainWindow::updateDockBreakpoints() {
	QStringList breakpoints{};
	for(qint64 const& breakpoint : project.getBreakpoints()) {
		breakpoints << QTime(0, 0, 0, 0).addMSecs(breakpoint).toString("HH:mm:ss.zzz");
	}
	breakpointListModel.setStringList(breakpoints);
}

void MainWindow::showDockContextMenu(QPoint const& relativePos) {
	QPoint absolutePos = breakpointListView.mapToGlobal(relativePos);
	QMenu contextualMenu;
	contextualMenu.addAction(&addBreakpointAction);
	contextualMenu.addAction(&addBreakpointHereAction);
	contextualMenu.addAction(&removeBreakpointAction);
	contextualMenu.exec(absolutePos);
}

void MainWindow::updatePlayPauseButtonIcon(QMediaPlayer::State state) {
	if(videoPlayer.getPosition()) {
		if(state == QMediaPlayer::PlayingState) {
			playerPlayPauseButton.setIcon(QIcon::fromTheme("media-playback-pause"));
		} else {
			playerPlayPauseButton.setIcon(QIcon::fromTheme("media-playback-start"));
		}
	}
}

void MainWindow::updateSliderRange(qint64 duration) {
	playerSeekBar.setRange(0, duration);
}

void MainWindow::updateSliderPosition(qint64 position) {
	if(position) {
		playerSeekBar.setValue(position);
	}
}

void MainWindow::updateDurationViewer(qint64 duration) {
	QTime qDuration = QTime(0, 0, 0).addMSecs(duration);
	playerDurationViewer.setText(qDuration.toString("HH:mm:ss.zzz"));
}

void MainWindow::updatePositionViewer(qint64 position) {
	if(position) {
		QTime qPosition = QTime(0, 0, 0).addMSecs(position);
		playerPositionViewer.setText(qPosition.toString("HH:mm:ss.zzz"));
	}
}

void MainWindow::showAddBreakpointDialog() {
	AddBreakpointDialog dialog(*this);
	dialog.exec();
}

void MainWindow::showJumpToTimeDialog() {
	JumpToTimeDialog dialog(*this);
	dialog.exec();
}

void MainWindow::addBreakpointHere() {
	project.addBreakpoint(videoPlayer.getPosition());
}

void MainWindow::removeDockBreakpoints() {
	QModelIndexList indexes = breakpointListView.selectionModel()->selectedIndexes();
	for(QModelIndex const& index : indexes) {
		auto positionIt = project.getBreakpoints().cbegin();
		std::advance(positionIt, index.row());
		project.removeBreakpoint(*positionIt);
	}
}

void MainWindow::alternateFullscreen(bool value) {
	if(value) {
		showFullScreen();
	} else {
		showNormal();
	}
}

// vim: fdm=marker
