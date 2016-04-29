#ifndef VideoDataManager_hpp
#define VideoDataManager_hpp

class VideoDataManager
{
public:
 	VideoDataManager();
    virtual ~VideoDataManager();
    VideoDataManager(VideoDataManager const&)              = delete;
    VideoDataManager(VideoDataManager&&)                   = delete;
    VideoDataManager& operator=(VideoDataManager const&)   = default;
    VideoDataManager& operator=(VideoDataManager&&)        = default;
};

#endif