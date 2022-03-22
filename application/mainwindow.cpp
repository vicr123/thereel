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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMenu>
#include <QTimer>
#include <QFileDialog>
#include "controlstrip.h"
#include <tcsdtools.h>
#include <tvariantanimation.h>
#include <QDesktopServices>
#include <QQmlContext>
#include <QMediaMetaData>
#include <taboutdialog.h>
#include <math.h>

struct MainWindowPrivate {
    tCsdTools csd;
    QMediaPlaylist* playlist;
    QMediaPlayer* player;

    ControlStrip* strip;
    tVariantAnimation* uiAnimation;
    QTimer* uiHideTimer;

    Qt::WindowStates windowStateBeforeFullScreen;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->setMouseTracking(true);
    ui->centralwidget->setMouseTracking(true);

    d = new MainWindowPrivate();
    d->csd.installMoveAction(ui->topWidget);
    d->csd.installResizeAction(this);

    if (tCsdGlobal::windowControlsEdge() == tCsdGlobal::Left) {
        ui->leftCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    } else {
        ui->rightCsdLayout->addWidget(d->csd.csdBoxForWidget(this));
    }

    QMenu* menu = new QMenu(this);

    QMenu* helpMenu = new QMenu(this);
    helpMenu->setTitle(tr("Help"));
    helpMenu->addAction(ui->actionFileBug);
    helpMenu->addAction(ui->actionSources);
    helpMenu->addSeparator();
    helpMenu->addAction(ui->actionAbout);

    menu->addAction(ui->actionOpen);
    menu->addSeparator();
    menu->addMenu(helpMenu);
    menu->addAction(ui->actionExit);

    ui->menuButton->setIconSize(SC_DPI_T(QSize(24, 24), QSize));
    ui->menuButton->setMenu(menu);
    ui->stackedWidget->setCurrentAnimation(tStackedWidget::Fade);
    ui->videoStack->setCurrentAnimation(tStackedWidget::Fade);

    d->uiAnimation = new tVariantAnimation(this);
    d->uiAnimation->setStartValue(0.0);
    d->uiAnimation->setEndValue(1.0);
    d->uiAnimation->setEasingCurve(QEasingCurve::OutCubic);
    d->uiAnimation->setDuration(250);
    connect(d->uiAnimation, &tVariantAnimation::valueChanged, this, &MainWindow::updateUiPosition);

    d->uiHideTimer = new QTimer(this);
    d->uiHideTimer->setInterval(3000);
    d->uiHideTimer->setSingleShot(true);
    connect(d->uiHideTimer, &QTimer::timeout, this, &MainWindow::hideUi);
    d->uiHideTimer->start();

    qApp->installEventFilter(this);

    d->strip = new ControlStrip(ui->centralwidget);
    connect(d->strip, &ControlStrip::changeFullScreen, this, [ = ](bool isFullScreen) {
        if (isFullScreen && !this->isFullScreen()) {
            d->windowStateBeforeFullScreen = this->windowState();
            this->showFullScreen();
        } else if (!isFullScreen && this->isFullScreen()) {
            this->setWindowState(d->windowStateBeforeFullScreen);
        }
    });
    ui->centralwidget->layout()->removeWidget(ui->topWidget);
    ui->topWidget->raise();

    d->player = new QMediaPlayer(this);
    d->playlist = new QMediaPlaylist(this);
    d->player->setPlaylist(d->playlist);

    QQmlContext* qml = ui->videoQmlWidget->rootContext();
    qml->setContextProperty("sourceVideo", this);

    connect(d->player, &QMediaPlayer::videoAvailableChanged, this, &MainWindow::updateVideoAvailable);
    connect(d->player, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &MainWindow::updateVideoGeometry);
    updateVideoAvailable();

    QIcon icon(":/icons/sleeping.svg");
    ui->idleLabel->setPixmap(icon.pixmap(SC_DPI_T(QSize(128, 128), QSize)));

    d->strip->setMediaPlayer(d->player);
}

MainWindow::~MainWindow() {
    delete d;
    delete ui;
}

QMediaObject* MainWindow::mediaObject() const {
    return d->player;
}

void MainWindow::on_actionOpen_triggered() {
    QFileDialog* dialog = new QFileDialog(this);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->setFileMode(QFileDialog::ExistingFiles);
    dialog->setNameFilters({tr("MPEG-4 Video (*.mp4)"), tr("AVI Video (*.avi)"), tr("Matroska Video (*.mkv)"), tr("Ogg Vorbis Video (*.ogv)")});
    connect(dialog, &QFileDialog::filesSelected, this, [ = ](QStringList files) {
        d->playlist->clear();
        d->playlist->addMedia(QMediaContent(QUrl::fromLocalFile(files.first())));
        d->player->play();
    });
    connect(dialog, &QFileDialog::urlsSelected, this, [ = ](QList<QUrl> urls) {
        d->playlist->clear();
        d->playlist->addMedia(QMediaContent(urls.first()));
        d->player->play();
    });
    connect(dialog, &QFileDialog::finished, dialog, &QFileDialog::deleteLater);
    dialog->open();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    updateUiPosition();
    updateVideoGeometry();
    d->strip->setFullScreen(this->isFullScreen());
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseMove) showUi();
    return false;
}

void MainWindow::showUi() {
    if (d->uiAnimation->endValue().toDouble() == 1.0) {
        d->uiAnimation->stop();
        d->uiAnimation->setStartValue(d->uiAnimation->currentValue());
        d->uiAnimation->setEndValue(0.0);
        d->uiAnimation->start();
    }

    d->uiHideTimer->stop();
    d->uiHideTimer->start();
    QApplication::restoreOverrideCursor();
}

void MainWindow::hideUi() {
    QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
    d->uiAnimation->setStartValue(0.0);
    d->uiAnimation->setEndValue(1.0);
    d->uiAnimation->start();
}

void MainWindow::updateUiPosition() {
    ui->topWidget->setGeometry(0, -ui->topWidget->sizeHint().height() * d->uiAnimation->currentValue().toDouble(), ui->centralwidget->width(), ui->topWidget->sizeHint().height());
    d->strip->setGeometry(0, ui->centralwidget->height() - d->strip->sizeHint().height() + d->strip->sizeHint().height() * d->uiAnimation->currentValue().toDouble(), ui->centralwidget->width(), d->strip->sizeHint().height());
}

void MainWindow::updateVideoAvailable() {
    ui->videoStack->setCurrentWidget(d->player->isVideoAvailable() ? ui->videoAvailablePage : ui->videoUnavailablePage);
}

void MainWindow::updateVideoGeometry() {
    QSize res = d->player->metaData(QMediaMetaData::Resolution).toSize();
    QSize scaled = res.scaled(ui->videoQmlWidget->size(), Qt::KeepAspectRatio);
    ui->videoQmlWidget->rootContext()->setContextProperty("videoWidth", scaled.width());
    ui->videoQmlWidget->rootContext()->setContextProperty("videoHeight", scaled.height());
}

void MainWindow::on_actionFileBug_triggered() {
    QDesktopServices::openUrl(QUrl("http://github.com/vicr123/thereel/issues"));
}

void MainWindow::on_actionSources_triggered() {
    QDesktopServices::openUrl(QUrl("http://github.com/vicr123/thereel"));
}

void MainWindow::on_actionAbout_triggered() {
    tAboutDialog d;
    d.exec();
}

void MainWindow::on_actionExit_triggered() {
    QApplication::exit();
}
