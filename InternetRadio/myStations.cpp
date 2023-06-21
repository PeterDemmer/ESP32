// myStations.cpp
// list of Internet Radio stations


/*
  Ref: Netherlands:  https://www.webradiostreams.nl/
  Ref: Flemish (nl_BE): https://www.vrt.be/nl/aanbod/kijk-en-luister/radio-luisteren/streamingslinks-radio/
*/

#include "Arduino.h"
#include "myStations.h"


const char *urls[] = {
  "https://icecast.omroep.nl/radio1-bb-mp3",                                         // NL NPO Radio 1
  "http://77.174.214.237:82/radio/mhoom.laatste.mp3",                                // last Met Het Oog Op Morgen (23:00-24:00), local MP3
  "https://25333.live.streamtheworld.com:443/CLASSICFM_SC",                          // Classic-FM NL
  "http://classicfm.ice.infomaniak.ch/classic-fm.mp3",                               // Classic-FM FR
  "https://icecast.omroep.nl/radio4-bb-mp3",                                         // NL NPO Radio 4
  "http://icecast-servers.vrtcdn.be/klara-high.mp3",                                 // VRT - radio Klara
  "https://wdr-wdr3-live.icecastssl.wdr.de/wdr/wdr3/live/mp3/128/stream.mp3",        // WDR 3 klassik
  "http://icecast.radiofrance.fr/francemusique-midfi.mp3",                           // France Musique
  "http://stream.live.vc.bbcmedia.co.uk/bbc_radio_three",                            // BBC classical music
  "https://playerservices.streamtheworld.com/api/livestream-redirect/TLPSTR18.mp3",  // Radio 10 60s 70s hits
  "https://icecast.omroep.nl/radio2-bb-mp3",                                         // NL NPO Radio 2
  "https://icecast.omroep.nl/radio5-bb-mp3",                                         // NL NPO Radio 5
  "https://stream.player.arrow.nl/arrow",                                            // Arrow Classic Rock
  "https://stream.qmusic.nl/qmusic/mp3",                                             // Q Music
  "https://playerservices.streamtheworld.com/api/livestream-redirect/RADIO538.mp3",  // Radio 538
  "https://playerservices.streamtheworld.com/api/livestream-redirect/RADIO10.mp3",   // Radio 10
  "https://playerservices.streamtheworld.com/api/livestream-redirect/SKYRADIO.mp3",  // Sky Radio
  "http://77.174.214.237:82/tv/Nieuwsuur.laatste.mp3",                               // last Nieuwsuur (21:30-22:20), local MP3
  "http://77.174.214.237:82/tv/TV-journaal.laatste.mp3",                             // last TV-Journaal (18:00-18:15), local MP3
  "http://77.174.214.237:82/tv/Buitenhof.laatste.mp3",                               // last Buitenhof (zondag 12:10-13:00), local MP3
  "http://stream.mediagroep-eva.nl:8001/radio",                                      // Amersfoort local radio
  "http://stream.live.vc.bbcmedia.co.uk/bbc_world_service",                          // BBC World Service
  "https://st01.sslstream.dlf.de/dlf/01/128/mp3/stream.mp3?aggregator=web",          // Deutschlandfunk
  "https://icecast.radiofrance.fr/fip-midfi.mp3?id=radiofrance",                     // FIP
  //"https://icecast.radiofrance.fr/fiprock-midfi.mp3?id=radiofrance",
  //"https://icecast.radiofrance.fr/fipjazz-midfi.mp3?id=radiofrance",
  //"https://icecast.radiofrance.fr/fipgroove-midfi.mp3?id=radiofrance",
  //"https://icecast.radiofrance.fr/fipworld-midfi.mp3?id=radiofrance",
  //"https://icecast.radiofrance.fr/fipnouveautes-midfi.mp3?id=radiofrance",
  //"https://icecast.radiofrance.fr/fipreggae-midfi.mp3?id=radiofrance",
  //"https://icecast.radiofrance.fr/fipelectro-midfi.mp3?id=radiofrance",
};
const uint8_t noUrls = sizeof(urls) / sizeof urls[0];

const char *stations[] = {
  "NPO Radio 1",          // 0
  "Met het oog ...",      // 1
  "Classic-NL",           // 2
  "Classic-FR",           // 3
  "NPO Radio 4",          // 4
  "VRT Klara",            // 5
  "WDR 3 klassik",        // 6
  "France Musique",       // 7
  "BBC Radio 3",          // 8
  "R.10 60s 70s hits",    // 9
  "NPO Radio 2",          // 10
  "NPO Radio 5",          // 11
  "Arrow classic rock",   // 12
  "QMusic",               // 13
  "Radio 538",            // 14
  "Radio 10",             // 15
  "Sky Radio",            // 16
  "Nieuwsuur 21:30",      // 17
  "TV-Journaal 18:00",    // 18
  "Buitenhof zo12:10",    // 19
  "EVA",                  // 20
  "BBC World Service",    // 21
  "Deutschlandfunk",      // 22
  "FIP",                  // 23
  //"FIP rock",
  //"FIP jazz",
  //"FIP groove",
  //"FIP world",
  //"FIP nouvautes",
  //"FIP reggae",
  //"FIP electro",
};
const uint8_t noStations = sizeof(stations) / sizeof stations[0];
uint8_t kStation;
