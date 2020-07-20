/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2020 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#ifndef CONTROLSTRIP_H
#define CONTROLSTRIP_H

#include <QWidget>

namespace Ui {
    class ControlStrip;
}

class QMediaPlayer;
struct ControlStripPrivate;
class ControlStrip : public QWidget {
        Q_OBJECT

    public:
        explicit ControlStrip(QWidget* parent = nullptr);
        ~ControlStrip();

        void setMediaPlayer(QMediaPlayer* player);
        void setFullScreen(bool isFullScreen);

    private slots:
        void on_playPauseButton_clicked();

        void on_slider_valueChanged(int value);

        void on_fullScreenButton_toggled(bool checked);

    signals:
        void changeFullScreen(bool isFullScreen);

    private:
        Ui::ControlStrip* ui;
        ControlStripPrivate* d;

        void updateState();
        void updateSlider();
        QString msToString(qint64 ms);
};

#endif // CONTROLSTRIP_H
