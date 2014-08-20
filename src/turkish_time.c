#include "turkish_time.h"
#include "string.h"

static const char* const VARSAATLER[] = {
  "sıfıra",
  "bire",
  "ikiye",
  "üçe",
  "dörde",
  "beşe",
  "altıya",
  "yediye",
  "sekize",
  "dokuza",
  "ona",
  "onbire",
  "onikiye"
};
static const char* const GECESAATLER[] = {
  "sıfırı",
  "biri",
  "ikiyi",
  "üçü",
  "dördü",
  "beşi",
  "altıyı",
  "yediyi",
  "sekizi",
  "dokuzu",
  "onu",
  "onbiri",
  "onikiyi"
};
static const char* const ONETEENS[] = {
  "sıfır",
  "bir",
  "iki",
  "üç",
  "dört",
  "beş",
  "altı",
  "yedi",
  "sekiz",
  "dokuz",
  "on",
  "onbir",
  "oniki",
  "onüç",
  "ondört",
  "onbeş",
  "onaltı",
  "onyedi",
  "onsekiz",
  "ondokuz"
};

static const char* const TWENS[] = {
  "yirmi",
  "otuz",
  "kırk",
  "elli",
};

static const char* STR_TEEN = "on";
static const char* STR_AZ_DK = "bir kaç dakika";
static const char* STR_AZ = "az bir zaman ";
static const char* STR_NOON = "öğle";
static const char* STR_MIDNIGHT = "geceyarısı";
static const char* STR_QUARTER = "çeyrek";
static const char* STR_TO = "var";
static const char* STR_PAST = "geçiyor";
static const char* STR_HALF = "buçuk";
static const char* STR_ITS = "saat";
static const char* STR_NOW = "şimdi";
static const char* STR_ALMOST = "nerdeyse";
static const char* STR_JUST = "Hemen Şimdi";
static const char* STR_ALREADY = "hemen hemen";

void fuzzy_time(int hours, int minutes, char* line1, char* line2, char* line3) {

  strcpy(line1, "");
  strcpy(line2, "");
  strcpy(line3, "");

  if (hours > 12) hours -= 12;
  if (hours == 0) hours = 12;
  
  if (minutes >= 0 && minutes < 5) {
    strcat(line1,STR_ITS);
    strcat(line2,ONETEENS[hours]);
    strcat(line3, "Oldu!");
  }
  if (minutes >= 1 && minutes < 35) {
    strcpy(line1, "");
    strcpy(line2, "");
    strcpy(line3, "");  
  
    strcat(line1,GECESAATLER[hours]);
    strcat(line3,STR_PAST);
    if (minutes < 5) {
      strcat(line2,STR_AZ_DK);
    }
    else if ((minutes >= 5 && minutes < 10)) {
      strcat(line2,ONETEENS[5]);
    }
    else if ((minutes >= 10 && minutes < 15)) {
      strcat(line2,ONETEENS[10]);
    }
    else if ((minutes >= 15 && minutes < 20)) {
      strcat(line2,STR_QUARTER);
    }
    else if ((minutes >= 20 && minutes < 25)) {
      strcat(line2,TWENS[0]);
    }
    else if ((minutes >= 25 && minutes < 30)) {
      strcpy(line1, "");
      strcat(line1,ONETEENS[hours]);
      strcpy(line2, "");
      strcat(line2,"buçuğa beş");
      strcpy(line3,"");
      strcat(line3,"var");
    }
  }
  if (minutes >= 30 && minutes < 35) {
      strcpy(line1, "");
      strcpy(line2, "");
      strcpy(line3, "");
      strcat(line1,STR_ITS);
      strcat(line2,ONETEENS[hours]);
      strcat(line3,STR_HALF);
    }
  if (minutes >= 35) {
    strcat(line3,STR_TO);
    if (hours == 12) {strcat(line1,"bire");}
    else
      strcat(line1,VARSAATLER[hours+1]);
    if (minutes >= 58) {
      strcat(line2,STR_AZ);
    }
    else if ((minutes >= 55 && minutes < 58)) {
      strcat(line2,ONETEENS[5]);
    }
    else if ((minutes >= 50 && minutes < 55)) {
      strcat(line2,ONETEENS[10]);
    }
    else if ((minutes >= 45 && minutes < 50)) {
      strcat(line2,STR_QUARTER);
    }
    else if ((minutes >= 40 && minutes < 45)) {
      strcat(line2,TWENS[0]);
    }
    else if ((minutes >= 35 && minutes < 40)) {
      strcpy(line1, "");
      strcat(line1,ONETEENS[hours]);
      strcpy(line2, "");
      strcat(line2,"buçuğu beş");
      strcpy(line3, "");
      strcat(line3,"geçiyor");
    }
  }
} 
