#include "videoplayermanager.hpp"

#include "mainwindow.hpp"

#include <QMediaContent>

#include <QKeyEvent>

#include <QMessageBox>

#include <QDir>

// std::abs
#include <cstdlib>
// std::find_if
#include <algorithm>

VideoPlayerManager::VideoPlayerManager(QWidget& parent, qint64 position, bool presentationMode)
      : QVideoWidget(&parent)
      , parent(parent)
      , player(&parent)
      , playlist(this)
      , presentationMode(presentationMode)
      , initialPosition(position) {
	player.setVideoOutput(this);
	player.setPlaylist(&playlist);
	player.setNotifyInterval(9);

	setFocusPolicy(Qt::ClickFocus);

	connect(&player, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));
	connect(&player, SIGNAL(durationChanged(qint64)), this, SLOT(updateSeekDuration(qint64)));
	connect(&player, SIGNAL(positionChanged(qint64)), this, SLOT(pauseOnBreakpoint(qint64)));
	connect(&playlist, SIGNAL(currentMediaChanged(QMediaContent const&)), this, SLOT(resetBreakpointsIterators()));

	if(presentationMode) {
		this->setWindowFlags(Qt::Window);
		this->setWindowState(Qt::WindowFullScreen);
		parent.hide();
		this->show();
	}
}

qint64 VideoPlayerManager::getPosition() const {
	return player.position();
}

void VideoPlayerManager::activateVideo() {
	playlist.clear();

	MainWindow& parent = dynamic_cast<MainWindow&>(this->parent);

	std::string basePath = parent.getProject().getProjectFileLocation(),
	            filePath = parent.getProject().getVideoFile();

	QDir qBaseDirectory = QDir(QString::fromStdString(basePath));
	QString qFilePath = qBaseDirectory.filePath(QString::fromStdString(filePath));

	playlist.addMedia(QMediaContent(QUrl::fromLocalFile(qFilePath)));

	playlist.setCurrentIndex(0);
	player.setPosition(initialPosition);
	// Hack to show the first frame
	player.play();
	player.pause();
}

void VideoPlayerManager::updateSeekDuration(qint64 videoDuration) {
	seekDuration = (videoDuration > 10'000)? 1'000 : videoDuration / 10;
}

void VideoPlayerManager::handleError() {
	QMessageBox::critical(this, "Player error", "Player error: " + player.errorString());
}

void VideoPlayerManager::playPause() {
	if(player.state() == QMediaPlayer::PlayingState) {
		player.pause();
	} else {
		player.play();
	}
}

void VideoPlayerManager::play() {
	player.play();
}

void VideoPlayerManager::pause() {
	player.pause();
}

void VideoPlayerManager::setPosition(qint64 position) {
	player.setPosition(position);
	resetBreakpointsIterators();
}

void VideoPlayerManager::setPosition(int position) {
	player.setPosition(position);
	resetBreakpointsIterators();
}

void VideoPlayerManager::seekForward() {
	player.setPosition(player.position() + seekDuration);
	resetBreakpointsIterators();
}

void VideoPlayerManager::seekBackward() {
	player.setPosition(player.position() - seekDuration);
	resetBreakpointsIterators();
}

QMediaPlayer const& VideoPlayerManager::getPlayer() const {
	return player;
}

void VideoPlayerManager::pauseOnBreakpoint(qint64 const& position) {
	if(player.state() == QMediaPlayer::PlayingState && position) {
		if(nextBreakpointIt == breakpointsEndIt) {
			return;
		} else if(std::abs(position - *nextBreakpointIt) <= 10) {
			player.pause();
			++nextBreakpointIt;
			return;
		}
	}
}

void VideoPlayerManager::resetBreakpointsIterators() {
	MainWindow& parent = dynamic_cast<MainWindow&>(this->parent);
	nextBreakpointIt = std::find_if(parent.getProject().getBreakpoints().cbegin(),
	                                parent.getProject().getBreakpoints().cend(),
	                                [this](qint64 value) { return value > player.position(); });
	breakpointsEndIt = parent.getProject().getBreakpoints().cend();
}

void VideoPlayerManager::keyPressEvent(QKeyEvent* event) {
	if(presentationMode) {
		if(event->key() == Qt::Key_Space) {
			playPause();
		} else if(event->key() == Qt::Key_Escape) {
			this->close();
		} else {
			QVideoWidget::keyPressEvent(event);
		}
	} else {
		switch(event->key()) {
			case Qt::Key_Space:
				playPause();
				break;
			case Qt::Key_Up:
				seekForward();
				break;
			case Qt::Key_Down:
				seekBackward();
				break;
			default:
				QVideoWidget::keyPressEvent(event);
				break;
		}
	}
}

void VideoPlayerManager::closeEvent(QCloseEvent* event) {
	if(presentationMode) {
		this->parentWidget()->show();
	}
	QVideoWidget::closeEvent(event);
}
