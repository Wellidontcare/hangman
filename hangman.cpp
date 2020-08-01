/* *************************************************************/
/*                                                             */
/*                                                             */
/*  hangman.cpp                                                */
/*                                                             */
/*  By: Wellidontcare <djjorisdj@gmail.com>                    */
/*                                                             */
/*  created: 08/01/20 10:37:47 by Wellidontcare                */
/*  updated: 08/01/20 12:11:20 by Joris Nonnast                */
/*                                                             */
/*                                                             */
/* **********************************************************²**/
#include <fstream>
#include <array>
#include <iostream>
#include <set>
#include <random>
#include <string>

const std::string stages_file_path = "../hangman_stages.lst";
const std::string words_file_path = "../words.lst";

struct Point {
    unsigned x;
    unsigned y;
};

// canvas class for drawing in console
template <size_t width, size_t height>
class Canvas{
    std::array<char, width*height> buffer_;
public:
    Canvas() : buffer_() {
        unsigned pos_counter = 0;
        std::generate(buffer_.begin(), buffer_.end(), [&pos_counter](){
            pos_counter++;
            return pos_counter % width == 0 ? '\n' : ' ';
        });
    }

    void add_text(std::string text, Point point){
        std::copy(text.begin(), text.end(), buffer_.begin() + (point.x + point.y*width));
    }

    void show() {
        printf("\033c");
        std::string printable_buffer(buffer_.begin(), buffer_.end());
        printf("%s", printable_buffer.c_str());
    }

    void clear(){
        unsigned pos_counter = 0;
        std::generate(buffer_.begin(), buffer_.end(),[&pos_counter](){
        pos_counter++;
        return pos_counter % width == 0 ? '\n' : ' ';
        });
    }

    char& operator[](const Point point){return buffer_[point.x + point.y*width];}
};

template<size_t width, size_t height>
class HangmanGame{
    Canvas<width, height> canvas_;
    std::vector<std::string> words_;
    std::vector<std::vector<std::string>> stages_;
    std::set<char> guessed_letters_;
    std::set<char> word_set_;
    std::set<char> word_set_tracker_;
    unsigned guess_index_ = 0;
    std::string current_word_;

    bool game_over_ = false;
    bool game_ends_ = false;
    unsigned lives_ = 11;

    //ui
    Point guess_text_origin_ = {width - width/3, height - height/4};
    Point status_text_origin_ = {0, 0};
    Point guessed_letters_text_origin_ = {width - width/3, height - height/4 + 2};
    std::string status_message = "Game starts now!";

    char input_;

public:
    explicit HangmanGame(const std::string& wordlist_file_path) : input_() {
        std::ifstream words_file(wordlist_file_path);
        std::string file_input;
        while(words_file >> file_input){
            words_.emplace_back(file_input);
        }
        std::ifstream stages_file(stages_file_path);
        std::vector<std::string> stage;
        unsigned line_counter = 7;
        while(std::getline(stages_file, file_input, '\n')){
            if(line_counter > 0){
                stage.emplace_back(file_input);
                line_counter--;
            }
            else{
                line_counter = 7;
                stage.emplace_back(file_input);
                stages_.emplace_back(stage);
                stage.clear();
            }
        }
        stages_.emplace_back(std::vector<std::string>{" ", " ", " ", " ", " ", " ", " ", " "});
        std::transform(words_.begin(), words_.end(), words_.begin(),
                       [](std::string& word){
            std::transform(word.begin(), word.end(), word.begin(), ::toupper); return word;
        });

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(words_.begin(), words_.end(), g);
    }
    void start_new_game() {
        if(guess_index_ > words_.size()){
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(words_.begin(), words_.end(), g);
            guess_index_ = 0;
        }
        current_word_ = words_[guess_index_];
        guess_index_++;
        word_set_.clear();
        guessed_letters_.clear();
        canvas_.clear();
        lives_ = 11;
        std::copy(current_word_.begin(), current_word_.end(), std::inserter(word_set_, word_set_.begin()));
        word_set_tracker_ = word_set_;
        game_ends_ = false;
        game_over_ = false;
        game_loop();
    }

private:
    void game_loop(){
        while(!game_ends_) {
            game_ends_ = game_over_;
            canvas_.add_text(status_message, status_text_origin_);
            std::string guess_indicator(current_word_);
            std::set<char> falsely_guessed_letters;
            std::set_difference(guessed_letters_.begin(), guessed_letters_.end(), word_set_.begin(), word_set_.end(), std::inserter(falsely_guessed_letters, falsely_guessed_letters.begin()));
            std::string guessed_letters_str(falsely_guessed_letters.begin(), falsely_guessed_letters.end());
            std::transform(guess_indicator.begin(), guess_indicator.end(), guess_indicator.begin(),
                           [this](char c){
                return guessed_letters_.contains(c) ? c : '_';
            });
            canvas_.add_text(guess_indicator, guess_text_origin_);
            canvas_.add_text(guessed_letters_str, guessed_letters_text_origin_);
            Point hangman_loc = {width/2, height/2};
            for(std::string& line : stages_[lives_]){
                canvas_.add_text(line, hangman_loc);
                hangman_loc.y++;
            }
            canvas_.show();
            if(!game_over_){
                printf("\nWhat's your guess?: ");
            }
            else{
                printf("\nAgain? (Y/N)");
                std::cin >> input_;
                input_ = std::toupper(input_);
                if(input_ == 'Y'){
                    start_new_game();
                    return;
                }else{
                    game_over_ = true;
                    return;
                }
            }
            std::cin >> input_;
            input_ = std::toupper(input_);
            if(lives_ < 1) {
                status_message = "You lost... The word was " + current_word_;
                game_over_ = true;
            } else if(guessed_letters_.contains(input_)) {
                status_message = "You already guessed that letter!";
                lives_--;
            } else if(word_set_.contains(input_)) {
                guessed_letters_.emplace(input_);
                status_message = "Right!";
                word_set_tracker_.erase(input_);
                if (word_set_tracker_.empty()) {
                    status_message = "You win!";
                    game_over_ = true;
                }
            } else {
                guessed_letters_.emplace(input_);
                status_message = "Wrong!";
                lives_--;
            }
            canvas_.clear();
        }
    }
};


int main(){
    HangmanGame<50, 25> game(words_file_path);
    game.start_new_game();
    return 0;
}
