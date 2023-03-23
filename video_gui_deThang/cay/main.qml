import QtQuick 2.11
import QtQuick.Window 2.11
import QtMultimedia 5.11

//import API_Lib 1.0
//import Thread_Lib 1.0
//property string name: "adela"
//    API_Request {
//        id: request
//    }
//    Thread_1minute {
//        id: thread_1minute
//    }

Rectangle {
    property int dem
    id: video_rec
    anchors.fill: parent
    color: "#ffffff"
    border.color: "#1866c8"
    signal doSomething();
    property string name: "3"
    property int i: -1
    function readTextFile(fileUrl){

        var xhr = new XMLHttpRequest;
        xhr.open("GET", fileUrl); // set Method and File
        xhr.onreadystatechange = function () {
            if(xhr.readyState === XMLHttpRequest.DONE){ // if request_status == DONE
                var response = xhr.responseText.toString().split('\n');
                name = response[response.length - 1];

                //name = name.substring(0,String(name.length-1));
                console.log("response: " + response);
                console.log("name: " + name);
                if (name == "")
                {
                    mediaPlayer.source = "file:///home/pi/Desktop/ADELA/video_gui_deThang/Video_coDinh/Adela3s.wmv"
                }
                else
                {
                    mediaPlayer.source = "file:///home/pi/Desktop/ADELA/VideoQuangCao_server_thaydoi/" + name;

                }
            }
        }
        xhr.send(); // begin the request
    }

    MediaPlayer {
        id: mediaPlayer
        autoPlay: true
        autoLoad: true
        source: "file:///home/pi/Desktop/ADELA/video_gui_deThang/Video_coDinh/Adela3s.wmv"
        volume: 0.5 //0.0->1.0
        //neu phat lap lai thi thay onPlaying = onStopped
        onStopped:
        {
            if (mediaPlayer.status == MediaPlayer.EndOfMedia)
            {
                readTextFile("file:///home/pi/Desktop/ADELA/video_gui_deThang/serverToAdvertisement.txt");
            }
            play()
        }

    }
    VideoOutput {
        width: parent.width
        height: parent.height
        anchors.centerIn: parent
        source: mediaPlayer
        anchors.fill: parent

    }
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {

            if (mediaPlayer.autoPlay == false) {
                mediaPlayer.autoPlay = true
            }
        }
    }
}
