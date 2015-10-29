#pragma once

#include <QVideoWidget>

#include <QMediaPlayer>
#include <QMediaPlaylist>

/*! \brief Class used to handle the video player
 *
 * It handle both the view and the model as the video management is pretty simple.
 */
class VideoPlayerManager : public QVideoWidget {

	Q_OBJECT

public:
	/*! \brief VideoPlayerManager constructor
	 *
	 * It will create the widget, the player and the playlist
	 *
	 * \param parent the parent widget (the main window)
	 * \param position the initial position in the video (in msecs)
	 * \param presentationMode true if the VideoPlayerManager must be started in
	 *        a presentation mode.
	 */
	VideoPlayerManager(QWidget& parent, qint64 position = 0, bool presentationMode = false);

	/*! \brief Return the current position in the video
	 */
	qint64 getPosition() const;

public slots:
	/*! \brief Activate the video.
	 *
	 * Loads the video from the current projet. Called when a project is loaded.
	 */
	void activateVideo();

	/*! \brief Pause if the current position is a breakpoint.
	 *
	 * Called if the position in the video has changed.
	 *
	 * \param position the position in the video.
	 */
	void pauseOnBreakpoint(qint64 const& position);

	/*! \brief Update the seek forward/backward duration.
	 *
	 * \param videoDuration duration of the current video.
	 */
	void updateSeekDuration(qint64 videoDuration);

	/*! \brief Handle any video error.
	 */
	void handleError();

	/*! \brief Alternate between play and pause states.
	 */
	void playPause();

	/*! \brief Set the position in the video.
	 *
	 * The position must be in msecs.
	 * Used when the user jump to a specific time.
	 *
	 * \param position the position to set.
	 */
	void setPosition(qint64 position);

	/*! \brief Set the position in the video.
	 *
	 * The position must be in msecs.
	 * Used when the seek bar is moved by the user.
	 *
	 * \param position the position to set.
	 */
	void setPosition(int position);

	/*! \brief Seek forward in the video.
	 *
	 * Uses the seekDuration property.
	 */
	void seekForward();

	/*! \brief Seek backward in the video.
	 *
	 * Uses the seekDuration property.
	 */
	void seekBackward();

	/*! \brief Returns the player (the model part).
	 *
	 * \return the player.
	 */
	QMediaPlayer const& getPlayer() const;

protected:
	/*! \brief keyPressEvent description
	 *
	 * Detailed description
	 *
	 * \param event argument description
	 */
	virtual void keyPressEvent(QKeyEvent* event);

	QWidget& parent;

	QMediaPlayer player;
	QMediaPlaylist playlist;

	bool presentationMode;
	qint64 initialPosition;
	qint64 seekDuration = 1;
private:
};
