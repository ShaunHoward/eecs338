/*
 * sentence_generator.c
 *
 *  Created on: Apr 24, 2016
 *      Author: shaun
 */

// code obtained from http://codereview.stackexchange.com/questions/23845/random-sentences-generator
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "sentence_generator.h"

const int SEN_LEN = 80;

const char* ARTICLES[] = {"the", "a", "one", "some", "any"};
const char* NOUNS[] =  {"boy", "girl", "dog", "town", "car"};
const char* VERBS[] =  {"drove", "jumped", "ran", "walked", "skipped"};
const char* PREPOSITIONS[] =  {"to", "from", "over", "under", "on"};
const int ARTICLES_SIZE = sizeof(ARTICLES)/sizeof(ARTICLES[0]);
const int NOUNS_SIZE = sizeof(NOUNS)/sizeof(NOUNS[0]);
const int VERBS_SIZE = sizeof(VERBS)/sizeof(VERBS[0]);
const int PREPOSITIONS_SIZE = sizeof(PREPOSITIONS)/sizeof(PREPOSITIONS[0]);

char* generateSentence() {
  char* sentence = calloc((SEN_LEN+1), sizeof(char));

  //Build Sentence
  strcat(sentence, ARTICLES[rand()%ARTICLES_SIZE]);

  strcat(sentence, " ");
  strcat(sentence, NOUNS[rand()%NOUNS_SIZE]);

  strcat(sentence, " ");
  strcat(sentence, VERBS[rand()%VERBS_SIZE]);

  strcat(sentence, " ");
  strcat(sentence, PREPOSITIONS[rand()%PREPOSITIONS_SIZE]);

  // had to add this in for significance
  strcat(sentence, " Eamon's office to hand in their final EECS 338 assignment on RPC.");

  //Capitalize first letter
  sentence[0] = toupper(sentence[0]);

  return sentence;
}
