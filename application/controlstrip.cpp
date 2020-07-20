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
#include "controlstrip.h"
#include "ui_controlstrip.h"

#include <QMediaPlayer>

struct ControlStripPrivate {
    QMediaPlayer* player;
};

ControlStrip::ControlStrip(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ControlStrip) {
    ui->setupUi(this);
    d = new ControlStripPrivate();
}

ControlStrip::~ControlStrip() {
    delete d;
    delete ui;
}

void ControlStrip::setMediaPlayer(QMediaPlayer* player) {
    d->player = player;

    connect(player, &QMediaPlayer::stateChanged, this, &ControlStrip::updateState);
    updateState();

    connect(player, &QMediaPlayer::positionChanged, this, &ControlStrip::updateSlider);
    connect(player, &QMediaPlayer::durationChanged, this, &ControlStrip::updateSlider);
    updateSlider();
}

void ControlStrip::setFullScreen(bool isFullScreen) {
    ui->fullScreenButton->setChecked(isFullScreen);
}

void ControlStrip::on_playPauseButton_clicked() {
    switch (d->player->state()) {
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            d->player->play();
            break;
        case QMediaPlayer::PlayingState:
            d->player->pause();
            break;
    }
}

void ControlStrip::updateState() {
    switch (d->player->state()) {
        case QMediaPlayer::StoppedState:
        case QMediaPlayer::PausedState:
            ui->playPauseButton->setIcon(QIcon::fromTheme("media-playback-start"));
            break;
        case QMediaPlayer::PlayingState:
            ui->playPauseButton->setIcon(QIcon::fromTheme("media-playback-pause"));
            break;
    }
}

void ControlStrip::updateSlider() {
    ui->slider->setMaximum(d->player->duration());
    ui->slider->setValue(d->player->position());
    ui->totalLabel->setText(msToString(d->player->duration()));
    ui->elapsedLabel->setText(msToString(d->player->position()));
}

QString ControlStrip::msToString(qint64 ms) {
    QStringList parts;

    qint64 seconds = ms / 1000 % 60;
    qint64 minutes = ms / 1000 / 60 % 60;
    qint64 hours = ms / 1000 / 60 / 60;

    if (hours > 0) parts.append(QString::number(hours));
    parts.append(QStringLiteral("%1").arg(minutes, 2, 10, QLatin1Char('0')));
    parts.append(QStringLiteral("%1").arg(seconds, 2, 10, QLatin1Char('0')));

    return parts.join(":");
}

void ControlStrip::on_slider_valueChanged(int value) {
    d->player->setPosition(value);
}

void ControlStrip::on_fullScreenButton_toggled(bool checked) {
    emit changeFullScreen(checked);
}
