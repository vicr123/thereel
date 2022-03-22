import QtQuick 2.0
import QtMultimedia 5.13

Item {
    Rectangle {
        color: "black"
        width: parent.width
        height: parent.height

        VideoOutput {
            id: video
            anchors.centerIn: parent
            width: videoWidth
            height: videoHeight
            flushMode: VideoOutput.LastFrame
            fillMode: VideoOutput.PreserveAspectFit
            source: sourceVideo
            transformOrigin: Item.Center
        }
    }
}
