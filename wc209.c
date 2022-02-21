/* wc209.c
 Hailemichael Molla
 STUDENT_ID : 20190771
 Assignment 1*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


int count_char, count_word, count_line;
int unterminated_line = 0;  // for keeping track of unterminated comment line number
int keep1 = 0, keep2 = 0;  // variables for detecting comments


enum MYSTATE {BEGIN, WORD, SPACE, SLASH, LINE_COMMENT, BLOCK_COMMENT, ASTERISK}; //defining constants
enum MYSTATE current_state = BEGIN; //initializing

/*Function declaration*/

enum MYSTATE begin(int i);
enum MYSTATE space(int i);
enum MYSTATE word(int i);
enum MYSTATE slash(int i);
enum MYSTATE line_comment(int i);
enum MYSTATE block_comment(int i);
enum MYSTATE asterisk(int i);


int main () {
    int i; 
    while ( (i = getchar())!=EOF) {       // condition where the function stops reading
        switch (current_state)                  
        {
            case BEGIN:
                current_state = begin(i);
                break; 
            case SPACE:
                current_state = space(i);
                break;
            case WORD:
                current_state = word(i);
                break;
            case SLASH:
                current_state = slash(i);
                break;
            case LINE_COMMENT:
                current_state = line_comment(i);
                break;
            case BLOCK_COMMENT:
                current_state = block_comment(i);
                break;
            case ASTERISK:
                current_state = asterisk(i);
                break;
        }
    }
    if (current_state == BLOCK_COMMENT || current_state == ASTERISK) {
        if (i == EOF) {              //for corner case errors like unterminated comment at line 1 or last line which ends without newline
            if (current_state == BLOCK_COMMENT){
                current_state = block_comment(i);  
        }
            else if(current_state == ASTERISK){
                current_state = asterisk(i);
        }
            fprintf(stderr, "Error: line %d: unterminated comment\n", unterminated_line);
            return EXIT_FAILURE;
        }
    }
    else {
        printf("%d %d %d\n", count_line, count_word, count_char);
        return EXIT_SUCCESS;
    }

}

/*defining the functions*/


enum MYSTATE begin(int i){     //start state
    if ((char)i == '\n') {
        (int) count_char++;
        (int) count_line++;
        current_state = BEGIN;
    }

    else if (isspace(i)) {
        (int) count_char++;
        current_state = SPACE;
    }
    else if ((char)i == '/') {
        (int) count_char++;
        (int) count_word++;
        keep1 = 1;  //to check if what comes after is also a slash
        current_state = SLASH;
    }
    else {
        (int) count_char++;
        (int) count_word++;
        current_state = WORD;
    }
    return current_state;
}

enum MYSTATE space(int i){  //space state

    if((char)i == '\n'){
        (int) count_char++;
        (int) count_line++;
        current_state = BEGIN;
    }
    else if (isspace(i)) {
        (int) count_char++;
        current_state = SPACE;
    }
    else if ((char)i == '/') {
        keep2 = 1;   // to check if what comes after is also a slash
        (int) count_char++;
        current_state = SLASH;
    }
   
    else {
        (int) count_char++;
        (int) count_word++;
        current_state = WORD;
    }
    return current_state;
}

enum MYSTATE word(int i){    //for non-space, non-newline, non-slash characters 
    if((char)i == '\n'){
        (int) count_line++;
        (int) count_char++;
        current_state = BEGIN;
    }
    else if (isspace(i)) {
        (int) count_char++;
        current_state = SPACE;
    }
    else if ((char)i == '/') {
        (int) count_char++;
        current_state = SLASH;
    }
    else {
        (int) count_char++;
        current_state = WORD;
    }
    return current_state;

}

enum MYSTATE slash(int i){    //state after one slash has been read

    if((char)i == '\n'){
        (int) count_char++;
        (int) count_line++;
        current_state = BEGIN;
    }
    
    else if (isspace(i)) {
        (int) count_char++;
        if ((int) keep2 == 1) {   //the character before the slash was space
            (int) count_word++;  // space-slash-space is a word
            keep2 = 0;
        }
        current_state = SPACE;
        }
    else if ((char)i == '/') { 
        (int) count_char--;   
        if (keep1 == 1){  //if the state before was begin, count_word should be decreased
            (int) count_word--;
            keep1 = 0;
        }
        current_state = LINE_COMMENT;
    }
    else if ((char)i == '*'){
        
        current_state = BLOCK_COMMENT;
    }
    
    else {
        (int) count_char++;
        if ((int) keep2 == 1){    //case when a word starting with slash comes after space
            (int) count_word++;
        }
        current_state = WORD;
    }
    keep2 = 0;      //zeroing keep2
    return current_state;
}

enum MYSTATE line_comment(int i){ // in a line comment state
    
    if((char)i == '\n'){
        (int) count_char++;
        (int) count_line++;
        current_state = BEGIN;
    }
    else {
        current_state = LINE_COMMENT;
    }
    return current_state;
}

enum MYSTATE block_comment(int i){ // in a block comment state
    
    if ((char)i == '\n') {  
  
        if ((int)count_line !=0){  //when it is not the first line
            (int) count_line++;
            (int) count_char++;
            if ((int) unterminated_line == 0) {  // to check if this is not a nested comment
                unterminated_line = (int) count_line;
                current_state = BLOCK_COMMENT;
            }
            else{
                current_state = BLOCK_COMMENT;  // don't do anything if it is a nested comment
            }
        }
        else{    //first line case
            (int) count_line++;
            (int) count_char++;
            unterminated_line = (int) count_line;
            current_state = BLOCK_COMMENT;
        }
    }
    else if((char)i == '*'){
        current_state = ASTERISK;
    }
    else if (i ==EOF){  // when there is unterminated comment at the last line
        if ((int) unterminated_line == 0){  // when the last line is also the first line (one line case)
            if ((int) count_line == 0){
                unterminated_line = 1;
            }
            else {
                unterminated_line = ((int) count_line) + 1; // last line but no newline character
            }
        }
        else {
            current_state = BLOCK_COMMENT;
        }
    }
    else {
        current_state = BLOCK_COMMENT;
    }
    return current_state;
}

enum MYSTATE asterisk(int i){  //state for a potential comment closing
    
    if ((char)i == '/') {
        unterminated_line = 0;   // the unterminated_line variable is zeroed everytime a block comment is terminated
        current_state = SPACE;
    }
    else if((char)i == '\n'){
        (int) count_char++;
        (int) count_line++;
        current_state = BLOCK_COMMENT;
    }
    
    else if((char)i == '*'){
        current_state = ASTERISK;
    }
    else if ( i == EOF){  // unterminated comment at the last line
        if ((int) unterminated_line == 0){  //check if it's not a nested comment
            if ((int) count_line == 0){    //one line case
                unterminated_line = 1;
        }
            else {
                unterminated_line = ((int) count_line) + 1; //multiple lines case with no newline character at the end
        }
        }
        else {
            current_state = ASTERISK;
        }
    } 

    else {
        current_state = BLOCK_COMMENT;
    }
    return current_state;
}