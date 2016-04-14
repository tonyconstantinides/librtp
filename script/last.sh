 gst-launch-1.0 rtpbin name=rtpbin rtspsrc location=rtsp://192.168.6.49:88/videoMain user_id=tony user_pw=Cyprus2016 latency=0 buffer-mode=0 rtp-blocksize=50000 protocols=tcp debug=TRUE retry=30 do-rtcp=TRUE do-rtsp-keep-alive=FALSE short-header=FALSE ! .send_rtp_sink rtpsession name=session .send_rtp_src ! rtpptdemux ! udpsink port=5000 session.send_rtcp_src ! udpsink port=5001

