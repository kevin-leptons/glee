#include <glee/api.hpp>
#include <iostream>
#include <unordered_set>
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <glee/media_file_iterator.hpp>

namespace glee {

/*
        List of common extensions of multi-media files.

        https://en.wikipedia.org/wiki/Audio_file_format
        https://en.wikipedia.org/wiki/Video_file_format
*/
std::unordered_set<std::string> _media_ext = {
        ".3gp",          // multimedia container
        ".aa",          // low-bitrate audiobook container
        ".aac",         // advanced audio coding
        ".aax",         // audiobook 
        ".act",         // lossy compressed audio 
        ".aiff",        // Apple audio file format
        ".amr",         // used primarily for speec
        ".ape",         // lossless audio compression
        ".au",          // Sun, Unix and Java audio file
        ".awb",         // used primarily for speech
        ".dct",         // variable codec for dictation
        ".dss",         // Olympus proprietary
        ".dvf",         // Sony compressed audio
        ".flac",        // free lossless audio codec
        ".gsm",         // designed for telephone use in Europe
        ".iklax",       // iKlax Media audio
        ".ivs",         // 3D Solar UK Ltd audio
        ".m4a",         // Apple unprotected music
        ".m4b",         // audiobook, podcast audio
        ".m4p",         // Apple music from iTunes
        ".mmf",         // Samsung audio, developed by Yamaha
        ".mp3",         // lossy audio
        ".mpc",         // open source lossy audio codec
        ".msv",         // Sony properietary
        ".ogg",         // free, open source container
        ".oga",         // same as .ogg
        ".mogg",        // same as .ogg
        ".opus",        // lossy audio compression ddeveloped by IETF
        ".ra",          // RealAudio format
        ".rm",          // same as .ra
        ".raw",         // contain any audio format
        ".sln",         // singed linear PCM
        ".tta",         // real-time lossless audio codec
        ".vox",         // similar to wave files
        ".wav",         // audio file container
        ".wma",         // Microsoft audio
        ".wv",          // format for wavpack file
        ".8svx",        // 8-bit sound samples

        ".webm",        // HTML5 video
        ".mkv",         // Matroska
        ".flv",         // web-based streaming video
        ".vob",         // stored in the VIDEO_TS folder at the root of DVD
        ".ogv",         // open source video container
        ".drc",         // open video compression
        ".gif",         // simple animation compression, no sound
        ".gifv",        // HTML5 video, not standard
        ".mng",         // multi-image network graphics, animation 
        ".avi",         // Microsoft video
        ".mov",         // QuickTime video
        ".qt",          // same as .mov
        ".wmv",         // Window Media Video
        ".yuv",         // supports all resolutions, sampling and frame rates
        ".rmvb",        // RealMedia
        ".asf",         // Advanced Systems Format
        ".amv",         // proprietary video
        ".mp4",         // MPEG-5 Part 14
        ".m4v",         // same as .mp4
        ".mpg",         // MPGE-1, MPEG-2
        ".mp2",         // same as .mpg
        ".mpeg",        // same as .mpg
        ".mpe",         // same as .mpg 
        ".mpv",         // same as .mpg
        ".m2v",         // same as .mpg
        ".svi",         // Samsung video
        ".3g2",         // common video format for cell phones
        ".mxf",         // Meterial Exchange Format
        ".roq",         // used by Quake 3
        ".nsv",         // for streaming video content over the internet
        ".f4v",         // flash video
        ".f4p",         // same as f4v 
        ".f4a",         // same as f4v
        ".f4b",         // same as f4v
};

void api::scan_by_dir_ids(std::vector<size_t> dir_ids)
{
        glee::media_dir media_dir;

        for (auto it = dir_ids.begin(); it != dir_ids.end(); ++it) {
                media_dir = _media_dir_table.find_by_id(*it);
                _media_file_table.del_by_dir_id(media_dir.id);
                _scan_directory(media_dir.id, media_dir.path);
        }
}

void api::show_media_file(size_t identity)
{
        media_file media_file;
        boost::filesystem::path file_path;
        boost::filesystem::path exec = boost::process::search_path("vlc");

        media_file = _media_file_table.find_by_id(identity);
        file_path = media_file.dir_path /= media_file.file_name;
        boost::process::spawn(exec, file_path,
                              boost::process::std_out > boost::process::null,
                              boost::process::std_in < boost::process::null,
                              boost::process::std_err > boost::process::null);
}

void api::_scan_directory(size_t dir_id, const boost::filesystem::path &path)
{
        glee::media_file_iterator media_file(path);
        std::string file_ext;

        for (auto it = media_file.begin(); it != media_file.end(); ++it) {
                boost::this_thread::interruption_point();
                file_ext = it.path().extension().string();
                if (_media_ext.find(file_ext) == _media_ext.end())
                        continue;
                _media_file_table.add(dir_id, it.path());
        }
}

}
