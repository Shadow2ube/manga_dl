//
// Created by christian on 6/22/22.
//

#ifndef MANGA_DL_SRC_CONSTANTS_H_
#define MANGA_DL_SRC_CONSTANTS_H_

#include <regex>

namespace constants {

const std::regex url_exp = std::regex(R"(https?://[^\s()<>]+(?:\([\w\d]+\)|(?:[^[:punct:]\s]|/)))");
const std::regex domain_exp = std::regex(R"(https?://[^\s()<>/]+)");
const std::regex mangapark_chapter_exp = std::regex(R"(/comic.*/c[0-9]+-en)");
const std::regex mangapark_image_exp = std::regex(
    R"(https://xfs-\d+\.mpcdn\.net/comic/\d+/[\d\w]+/[\d\w]+/[\d\w_]+\.jpeg\?acc=[\d\w_]+&(?:amp;exp=\d+|exp\d+))");

}

#endif //MANGA_DL_SRC_CONSTANTS_H_
