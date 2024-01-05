#include "utils.h"


namespace fs = std::filesystem;

std::vector<std::string> getAllDirectories(std::string path) {
    std::vector<std::string> names;

    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (fs::is_directory(entry.status())) {
                names.push_back(entry.path().filename());

            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }

    return names;
}



std::string fifoFileName (std::string path , std::string builingName){
    return (path+"/"+builingName);
}


void makingFifos(std::vector<std::string>allBuilingName , std::vector<std::string> &fifos){
    std::string fifosPath = "/tmp";
    for (std::string b : allBuilingName){
        std::string fifoFile = fifoFileName(fifosPath , b);
        if (mkfifo(fifoFile.c_str() , 0666 ) < 0){
            perror("mkfifo Failed");
            exit(EXIT_FAILURE);
        }
        fifos.push_back(fifoFile);
        
    }
}

void getFifosNames(std::vector<std::string>allBuilingName , std::vector<std::string> &fifos){
    std::string fifosPath = "/tmp";
    for (std::string b : allBuilingName){
        std::string fifoFile = fifoFileName(fifosPath , b);
        fifos.push_back(fifoFile);

    }
    
}


