#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>

struct Argument {
    std::string var;
    int val;
};

class ArgumentParser{
    /*
     * Helper class to parse command line inputs
     */
    public:
        std::string* info;
        std::set<std::string> args;
        int nargs;
        //std::vector<Argument> args;
        ArgumentParser(std::string *description, std::string[] argnames){
            this->info = description;
            try {
                std::string arg = argnames[0];
            } catch (std::runtime_error &e) 
            {
                
            }

        }
        /*
        InputParser (int &argc, char **argv){
            for (int i=1; i < argc; ++i)
                this->tokens.push_back(std::string(argv[i]));
        }
        */
        /// @author iain
        const std::string& getCmdOption(const std::string &option) const{
            std::vector<std::string>::const_iterator itr;
            itr =  std::find(this->tokens.begin(), this->tokens.end(), option);
            if (itr != this->tokens.end() && ++itr != this->tokens.end()){
                return *itr;
            }
            static const std::string empty_string("");
            return empty_string;
        }
        /// @author iain
        bool cmdOptionExists(const std::string &option) const{
            return std::find(this->tokens.begin(), this->tokens.end(), option)
                   != this->tokens.end();
        }
    private:
        std::vector <std::string> tokens;
};

int main(int argc, char **argv){
    InputParser input(argc, argv);
    if(input.cmdOptionExists("-h")){
        // Do stuff
    }
    const std::string &filename = input.getCmdOption("-f");
    if (!filename.empty()){
        // Do interesting things ...
    }
    return 0;
}  
